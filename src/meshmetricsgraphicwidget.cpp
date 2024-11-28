/********************************************************************************
*  This file is part of PEMesh                                                  *
*  Copyright(C) 2022: Daniela Cabiddu                                           *

*                                                                               *
*  Author(s):                                                                   *
*                                                                               *
*     Daniela Cabiddu (daniela.cabiddu@cnr.it)                                  *
*                                                                               *
*     Italian National Research Council (CNR)                                   *
*     Institute for Applied Mathematics and Information Technologies (IMATI)    *
*     Via de Marini, 6                                                          *
*     16149 Genoa,                                                              *
*     Italy                                                                     *
*                                                                               *
*                                                                               *
*                                                                               *
*  This program is free software: you can redistribute it and/or modify it      *
*  under the terms of the GNU General Public License as published by the        *
*  Free Software Foundation, either version 3 of the License, or (at your       *
*  option) any later version.                                                   *
*                                                                               *
*  This program is distributed in the hope that it will be useful, but          *
*  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY   *
*  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
*  more details.                                                                *
*                                                                               *
*  You should have received a copy of the GNU General Public License along      *
*  with this program. If not, see <https://www.gnu.org/licenses/>.              *
*                                                                               *
*********************************************************************************/

#include "meshmetricsgraphicwidget.h"
#include "ui_meshmetricsgraphicwidget.h"

#include "quality_metrics.h"
#include <qdir.h>
#include <QDesktopServices>

MeshMetricsGraphicWidget::MeshMetricsGraphicWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MeshMetricsGraphicWidget)
{
    ui->setupUi(this);

    connect(ui->mesh_metrics_slider, SIGNAL(valueChanged(int)), this, SLOT(show_mesh(int)));
    connect(ui->inr_rb, SIGNAL(clicked()), this, SLOT(show_inr()));
    connect(ui->our_rb, SIGNAL(clicked()), this, SLOT(show_our()));
    connect(ui->cir_rb, SIGNAL(clicked()), this, SLOT(show_cir()));
    connect(ui->krr_rb, SIGNAL(clicked()), this, SLOT(show_krr()));
    connect(ui->kar_rb, SIGNAL(clicked()), this, SLOT(show_kar()));
    connect(ui->apr_rb, SIGNAL(clicked()), this, SLOT(show_apr()));
    connect(ui->mia_rb, SIGNAL(clicked()), this, SLOT(show_mia()));
    connect(ui->maa_rb, SIGNAL(clicked()), this, SLOT(show_maa()));
    connect(ui->anr_rb, SIGNAL(clicked()), this, SLOT(show_anr()));
    connect(ui->vem_rb, SIGNAL(clicked()), this, SLOT(show_vem()));
    connect(ui->jac_rb, SIGNAL(clicked()), this, SLOT(show_jac()));
    connect(ui->fro_rb, SIGNAL(clicked()), this, SLOT(show_fro()));

    connect(ui->min_rb, SIGNAL(clicked()), this, SLOT(show_min()));
    connect(ui->max_rb, SIGNAL(clicked()), this, SLOT(show_max()));
    connect(ui->avg_rb, SIGNAL(clicked()), this, SLOT(show_avg()));
    connect(ui->all_rb, SIGNAL(clicked()), this, SLOT(show_all()));
    connect(ui->tri_rb, SIGNAL(clicked()), this, SLOT(show_tri()));
    connect(ui->poly_rb, SIGNAL(clicked()), this, SLOT(show_poly()));

    ui->inr_rb->setText((metrics_acronym.at(0)  + " - " + metrics_names.at(0)).c_str());
    ui->our_rb->setText((metrics_acronym.at(1)  + " - " + metrics_names.at(1)).c_str());
    ui->cir_rb->setText((metrics_acronym.at(2)  + " - " + metrics_names.at(2)).c_str());
    ui->krr_rb->setText((metrics_acronym.at(3)  + " - " + metrics_names.at(3)).c_str());
    ui->kar_rb->setText((metrics_acronym.at(4)  + " - " + metrics_names.at(4)).c_str());
    ui->apr_rb->setText((metrics_acronym.at(5)  + " - " + metrics_names.at(5)).c_str());
    ui->mia_rb->setText((metrics_acronym.at(6)  + " - " + metrics_names.at(6)).c_str());
    ui->maa_rb->setText((metrics_acronym.at(7)  + " - " + metrics_names.at(7)).c_str());
    ui->anr_rb->setText((metrics_acronym.at(8)  + " - " + metrics_names.at(8)).c_str());
    ui->vem_rb->setText((metrics_acronym.at(9)  + " - " + metrics_names.at(9)).c_str());
    ui->jac_rb->setText((metrics_acronym.at(10) + " - " + metrics_names.at(10)).c_str());
    ui->fro_rb->setText((metrics_acronym.at(11) + " - " + metrics_names.at(11)).c_str());

    ui->min_rb->hide();
    ui->max_rb->hide();
    ui->avg_rb->hide();

    ui->groupBox_2->hide();
    ui->info_text->hide();
}

MeshMetricsGraphicWidget::~MeshMetricsGraphicWidget()
{
    delete ui;
}

void MeshMetricsGraphicWidget::clean_canvas()
{
    ui->mesh_metrics_canvas->clear();
    mesh_with_metrics.clear();
}

void MeshMetricsGraphicWidget::show_mesh (int i)
{
    if (i < 0) return;

    clean_canvas();

    std::cout << "V: " << (*d->get_parametric_mesh(i)).num_verts() << std::endl;

    ui->mesh_metrics_canvas->add_mesh(*d->get_parametric_mesh(i));
    // d->get_parametric_mesh(static_cast<uint>(i))->updateGL();
    // ui->mesh_metrics_canvas->push_obj(d->get_parametric_mesh(static_cast<uint>(i)), update_scene);
    ui->mesh_metrics_canvas->updateGL();

    update_scene=false;

    mesh_with_metrics.push_back(d->get_parametric_mesh(static_cast<uint>(i)));

    curr_mesh_id = static_cast<uint>(i);

    if      (ui->min_rb->isChecked()) show_min();
    else if (ui->max_rb->isChecked()) show_max();
    else if (ui->avg_rb->isChecked()) show_avg();
    else if (ui->all_rb->isChecked()) show_all();
    else show_poly();
}

void MeshMetricsGraphicWidget::set_dataset(Dataset *ds)
{
    clean_canvas();

    d = ds;
}

void MeshMetricsGraphicWidget::set_metrics (std::vector<MeshMetrics> *m)
{
    metrics = m;
}

void MeshMetricsGraphicWidget::set_min_color(const uint i)
{
    std::cout << "Min ID " << i << std::endl;

    if (i > mesh_with_metrics.at(0)->num_polys()-1) return;

    mesh_with_metrics.at(0)->poly_data(i).color = cinolib::Color::GREEN();
}

void MeshMetricsGraphicWidget::set_max_color(const uint i)
{
    std::cout << "Max ID " << i << std::endl;

    if (i > mesh_with_metrics.at(0)->num_polys()-1) return;

    mesh_with_metrics.at(0)->poly_data(i).color = cinolib::Color::RED();
}

void MeshMetricsGraphicWidget::set_all_color(const double min, const double max, const std::vector<double> all)
{

    QRectF rect = QRectF (ui->colorbar->rect());

    QLinearGradient colorGradient = QLinearGradient(rect.topLeft(), rect.topRight());

    uint n_polys = mesh_with_metrics.at(0)->num_polys();
    uint n_colors = n_polys < 100 ? n_polys : 100;
    for (uint pid=0; pid < n_polys; ++pid) {
        double quality = all.at(pid);
        double scaled_quality = max != min ? (quality - min) / (max - min) : min;
        cinolib::Color c = cinolib::Color::parula_ramp(n_colors+1, (int)(scaled_quality * (n_colors-1)));
        // cinolib::Color c = cinolib::Color::red_white_blue_ramp_01(scaled_quality);
        mesh_with_metrics.at(0)->poly_data(pid).color = c;

        colorGradient.setColorAt(scaled_quality, QColor(c.r * 255, c.g * 255, c.b * 255));
    }

    QBrush colorGradiantBrush = QBrush(colorGradient);

    QPalette palette = ui->colorbar->palette();
    palette.setBrush(QPalette::Base, colorGradiantBrush);
    ui->colorbar->setPalette(palette);
}

void MeshMetricsGraphicWidget::show_inr()
{
    mesh_with_metrics.at(0)->poly_set_color(cinolib::Color::WHITE());

    std::string message = "";

    uint id_min = metrics->at(curr_mesh_id).INR_min_id;
    uint id_max = metrics->at(curr_mesh_id).INR_max_id;

    if (ui->min_rb->isChecked() || ui->avg_rb->isChecked())
    {
        set_min_color(id_min);
        message += "<br><font color=\"red\">INR MIN : " + std::to_string(metrics->at(curr_mesh_id).INR_min) + "</font>";
    }

    if (ui->max_rb->isChecked() || ui->avg_rb->isChecked())
    {
        set_max_color(id_max);
        message += "<br><font color=\"blue\">INR MAX : " + std::to_string(metrics->at(curr_mesh_id).INR_max) + "</font>";
    }

    if (ui->avg_rb->isChecked())
    {
        message += "<br>INR AVG : " + std::to_string(metrics->at(curr_mesh_id).INR_avg);
    }

    if (ui->tri_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).INR_min_id;
        id_max = metrics->at(curr_mesh_id).INR_max_id;

        set_min_color(id_min);
        set_max_color(id_max);

        message += "<br><font color=\"red\">INR MIN : " + std::to_string(metrics->at(curr_mesh_id).INR_min) + "</font>";
        message += "<br><font color=\"blue\">INR MAX : " + std::to_string(metrics->at(curr_mesh_id).INR_max) + "</font>";
        message += "<br>INR AVG : " + std::to_string(metrics->at(curr_mesh_id).INR_avg);
    }

    if (ui->poly_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).INR_poly_min_id;
        id_max = metrics->at(curr_mesh_id).INR_poly_max_id;

        set_min_color(id_min);
        set_max_color(id_max);

        message += "<br><font color=\"red\">INR POLY MIN : " + std::to_string(metrics->at(curr_mesh_id).INR_poly_min) + "</font>";
        message += "<br><font color=\"blue\">INR POLY MAX : " + std::to_string(metrics->at(curr_mesh_id).INR_poly_max) + "</font>";
        message += "<br>INR POLY AVG : " + std::to_string(metrics->at(curr_mesh_id).INR_poly_avg);
    }

    if (ui->all_rb->isChecked())
    {
        double min = std::min(metrics->at(curr_mesh_id).INR_min, metrics->at(curr_mesh_id).INR_poly_min);
        double max = std::max(metrics->at(curr_mesh_id).INR_max, metrics->at(curr_mesh_id).INR_poly_max);

        if (metrics->at(curr_mesh_id).INR_min < metrics->at(curr_mesh_id).INR_poly_min)
            id_min = metrics->at(curr_mesh_id).INR_min_id;
        else
            id_min = metrics->at(curr_mesh_id).INR_poly_min_id;

        if (metrics->at(curr_mesh_id).INR_max > metrics->at(curr_mesh_id).INR_poly_max)
            id_max = metrics->at(curr_mesh_id).INR_max_id;
        else
            id_max = metrics->at(curr_mesh_id).INR_poly_max_id;

        std::vector<double> all = metrics->at(curr_mesh_id).INR_all;

        set_all_color(min, max, all);

        if (ui->highlight_min_max_cb->isChecked())
        {
            highlight_min_max_elements(id_min,id_max);
        }

        ui->min_label->setText(QString(std::to_string(min).c_str()));
        ui->max_label->setText(QString(std::to_string(max).c_str()));
    }

    ui->mesh_metrics_canvas->clear();
    ui->mesh_metrics_canvas->add_mesh(*mesh_with_metrics.at(0), true);

    ui->info_text->setHtml(message.c_str());
}

void MeshMetricsGraphicWidget::show_our()
{
    mesh_with_metrics.at(0)->poly_set_color(cinolib::Color::WHITE());

    std::string message = "";

    uint id_min = metrics->at(curr_mesh_id).OUR_min_id;
    uint id_max = metrics->at(curr_mesh_id).OUR_max_id;

    if (ui->min_rb->isChecked() || ui->avg_rb->isChecked())
    {
        set_min_color(id_min);
        message += "<br><font color=\"red\">OUR MIN : " + std::to_string(metrics->at(curr_mesh_id).OUR_min) + "</font>";
    }

    if (ui->max_rb->isChecked() || ui->avg_rb->isChecked())
    {
        set_max_color(id_max);
        message += "<br><font color=\"blue\">OUR MAX : " + std::to_string(metrics->at(curr_mesh_id).OUR_max) + "</font>";
    }

    if (ui->avg_rb->isChecked())
    {
        message += "<br>OUR AVG : " + std::to_string(metrics->at(curr_mesh_id).OUR_avg);
    }

    if (ui->tri_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).OUR_min_id;
        id_max = metrics->at(curr_mesh_id).OUR_max_id;

        set_min_color(id_min);
        set_max_color(id_max);

        message += "<br><font color=\"red\">OUR MIN : " + std::to_string(metrics->at(curr_mesh_id).OUR_min) + "</font>";
        message += "<br><font color=\"blue\">OUR MAX : " + std::to_string(metrics->at(curr_mesh_id).OUR_max) + "</font>";
        message += "<br>OUR AVG : " + std::to_string(metrics->at(curr_mesh_id).OUR_avg);
    }

    if (ui->poly_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).OUR_poly_min_id;
        id_max = metrics->at(curr_mesh_id).OUR_poly_max_id;

        set_min_color(id_min);
        set_max_color(id_max);

        message += "<br><font color=\"red\">OUR POLY MIN : " + std::to_string(metrics->at(curr_mesh_id).OUR_poly_min) + "</font>";
        message += "<br><font color=\"blue\">OUR POLY MAX : " + std::to_string(metrics->at(curr_mesh_id).OUR_poly_max) + "</font>";
        message += "<br>OUR POLY AVG : " + std::to_string(metrics->at(curr_mesh_id).OUR_poly_avg);
    }

    if (ui->all_rb->isChecked())
    {
        double min = std::min(metrics->at(curr_mesh_id).OUR_min, metrics->at(curr_mesh_id).OUR_poly_min);
        double max = std::max(metrics->at(curr_mesh_id).OUR_max, metrics->at(curr_mesh_id).OUR_poly_max);

        if (metrics->at(curr_mesh_id).OUR_min < metrics->at(curr_mesh_id).OUR_poly_min)
            id_min = metrics->at(curr_mesh_id).OUR_min_id;
        else
            id_min = metrics->at(curr_mesh_id).OUR_poly_min_id;

        if (metrics->at(curr_mesh_id).OUR_max > metrics->at(curr_mesh_id).OUR_poly_max)
            id_max = metrics->at(curr_mesh_id).OUR_max_id;
        else
            id_max = metrics->at(curr_mesh_id).OUR_poly_max_id;

        std::vector<double> all = metrics->at(curr_mesh_id).OUR_all;

        set_all_color(min, max, all);

        if (ui->highlight_min_max_cb->isChecked())
        {
            highlight_min_max_elements(id_min,id_max);
        }

        ui->min_label->setText(QString(std::to_string(min).c_str()));
        ui->max_label->setText(QString(std::to_string(max).c_str()));
    }

    ui->mesh_metrics_canvas->clear();
    ui->mesh_metrics_canvas->add_mesh(*mesh_with_metrics.at(0), true);

    ui->info_text->setHtml(message.c_str());
}

void MeshMetricsGraphicWidget::show_cir ()
{
    mesh_with_metrics.at(0)->poly_set_color(cinolib::Color::WHITE());

    std::string message = "";

    uint id_min = metrics->at(curr_mesh_id).CIR_min_id;
    uint id_max = metrics->at(curr_mesh_id).CIR_max_id;

    if (ui->min_rb->isChecked() || ui->avg_rb->isChecked())
    {
        set_min_color(id_min);
        message += "<br><font color=\"red\">CIR MIN : " + std::to_string(metrics->at(curr_mesh_id).CIR_min) + "</font>";
    }

    if (ui->max_rb->isChecked() || ui->avg_rb->isChecked())
    {
        set_max_color(id_max);
        message += "<br><font color=\"blue\">CIR MAX : " + std::to_string(metrics->at(curr_mesh_id).CIR_max)+ "</font>";
    }

    if (ui->avg_rb->isChecked())
    {
        message += "<br>CIR AVG : " + std::to_string(metrics->at(curr_mesh_id).CIR_avg);
    }

    if (ui->tri_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).CIR_min_id;
        id_max = metrics->at(curr_mesh_id).CIR_max_id;

        set_min_color(id_min);
        set_max_color(id_max);

        message += "<br><font color=\"red\">CIR MIN : " + std::to_string(metrics->at(curr_mesh_id).CIR_min) + "</font>";
        message += "<br><font color=\"blue\">CIR MAX : " + std::to_string(metrics->at(curr_mesh_id).CIR_max) + "</font>";
        message += "<br>CIR AVG : " + std::to_string(metrics->at(curr_mesh_id).CIR_avg);
    }

    if (ui->poly_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).CIR_poly_min_id;
        id_max = metrics->at(curr_mesh_id).CIR_poly_max_id;

        set_min_color(id_min);
        set_max_color(id_max);

        message += "<br><font color=\"red\">CIR POLY MIN : " + std::to_string(metrics->at(curr_mesh_id).CIR_poly_min) + "</font>";
        message += "<br><font color=\"blue\">CIR POLY MAX : " + std::to_string(metrics->at(curr_mesh_id).CIR_poly_max) + "</font>";
        message += "<br>CIR POLY AVG : " + std::to_string(metrics->at(curr_mesh_id).CIR_poly_avg);
    }

    if (ui->all_rb->isChecked())
    {
        double min = std::min(metrics->at(curr_mesh_id).CIR_min, metrics->at(curr_mesh_id).CIR_poly_min);
        double max = std::max(metrics->at(curr_mesh_id).CIR_max, metrics->at(curr_mesh_id).CIR_poly_max);

        if (metrics->at(curr_mesh_id).CIR_min < metrics->at(curr_mesh_id).CIR_poly_min)
            id_min = metrics->at(curr_mesh_id).CIR_min_id;
        else
            id_min = metrics->at(curr_mesh_id).CIR_poly_min_id;

        if (metrics->at(curr_mesh_id).CIR_max > metrics->at(curr_mesh_id).CIR_poly_max)
            id_max = metrics->at(curr_mesh_id).CIR_max_id;
        else
            id_max = metrics->at(curr_mesh_id).CIR_poly_max_id;

        std::vector<double> all = metrics->at(curr_mesh_id).CIR_all;

        set_all_color(min, max, all);

        if (ui->highlight_min_max_cb->isChecked())
        {
            highlight_min_max_elements(id_min,id_max);
        }

        ui->min_label->setText(QString(std::to_string(min).c_str()));
        ui->max_label->setText(QString(std::to_string(max).c_str()));
    }

    ui->mesh_metrics_canvas->clear();
    ui->mesh_metrics_canvas->add_mesh(*mesh_with_metrics.at(0), true);

    ui->info_text->setHtml(message.c_str());
}

void MeshMetricsGraphicWidget::show_krr ()
{
    mesh_with_metrics.at(0)->poly_set_color(cinolib::Color::WHITE());

    std::string message = "";

    uint id_min = metrics->at(curr_mesh_id).KRR_min_id;
    uint id_max = metrics->at(curr_mesh_id).KRR_max_id;

    if (ui->min_rb->isChecked() || ui->avg_rb->isChecked())
    {
        set_min_color(id_min);
        message += "<br><font color=\"red\">KRR MIN : " + std::to_string(metrics->at(curr_mesh_id).KRR_min) + "</font>";
    }

    if (ui->max_rb->isChecked() || ui->avg_rb->isChecked())
    {
        set_max_color(id_max);
        message += "<br><font color=\"blue\">KRR MAX : " + std::to_string(metrics->at(curr_mesh_id).KRR_max) + "</font>";
    }

    if (ui->avg_rb->isChecked())
    {
        message += "<br>KRR AVG : " + std::to_string(metrics->at(curr_mesh_id).KRR_avg);
    }

    if (ui->tri_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).KRR_min_id;
        id_max = metrics->at(curr_mesh_id).KRR_max_id;

        set_min_color(id_min);
        set_max_color(id_max);

        message += "<br><font color=\"red\">KRR MIN : " + std::to_string(metrics->at(curr_mesh_id).KRR_min) + "</font>";
        message += "<br><font color=\"blue\">KRR MAX : " + std::to_string(metrics->at(curr_mesh_id).KRR_max) + "</font>";
        message += "<br>KRR AVG : " + std::to_string(metrics->at(curr_mesh_id).KRR_avg);
    }

    if (ui->poly_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).KRR_poly_min_id;
        id_max = metrics->at(curr_mesh_id).KRR_poly_max_id;

        set_min_color(id_min);
        set_max_color(id_max);

        message += "<br><font color=\"red\">KRR POLY MIN : " + std::to_string(metrics->at(curr_mesh_id).KRR_poly_min) + "</font>";
        message += "<br><font color=\"blue\">KRR POLY MAX : " + std::to_string(metrics->at(curr_mesh_id).KRR_poly_max) + "</font>";
        message += "<br>KRR POLY AVG : " + std::to_string(metrics->at(curr_mesh_id).KRR_poly_avg);
    }

    if (ui->all_rb->isChecked())
    {
        double min = std::min(metrics->at(curr_mesh_id).KRR_min, metrics->at(curr_mesh_id).KRR_poly_min);
        double max = std::max(metrics->at(curr_mesh_id).KRR_max, metrics->at(curr_mesh_id).KRR_poly_max);

        if (metrics->at(curr_mesh_id).KRR_min < metrics->at(curr_mesh_id).KRR_poly_min)
            id_min = metrics->at(curr_mesh_id).KRR_min_id;
        else
            id_min = metrics->at(curr_mesh_id).KRR_poly_min_id;

        if (metrics->at(curr_mesh_id).KRR_max > metrics->at(curr_mesh_id).KRR_poly_max)
            id_max = metrics->at(curr_mesh_id).KRR_max_id;
        else
            id_max = metrics->at(curr_mesh_id).KRR_poly_max_id;

        std::vector<double> all = metrics->at(curr_mesh_id).KRR_all;

        set_all_color(min, max, all);

        if (ui->highlight_min_max_cb->isChecked())
        {
            highlight_min_max_elements(id_min,id_max);
        }

        ui->min_label->setText(QString(std::to_string(min).c_str()));
        ui->max_label->setText(QString(std::to_string(max).c_str()));
    }

    ui->mesh_metrics_canvas->clear();
    ui->mesh_metrics_canvas->add_mesh(*mesh_with_metrics.at(0), true);

    ui->info_text->setHtml(message.c_str());
}

void MeshMetricsGraphicWidget::show_kar ()
{
    mesh_with_metrics.at(0)->poly_set_color(cinolib::Color::WHITE());

    std::string message = "";

    uint id_min = metrics->at(curr_mesh_id).KAR_min_id;
    uint id_max = metrics->at(curr_mesh_id).KAR_max_id;

    if (ui->min_rb->isChecked() || ui->avg_rb->isChecked())
    {
        set_min_color(id_min);
        message += "<br><font color=\"red\">KAR MIN : " + std::to_string(metrics->at(curr_mesh_id).KAR_min) + "</font>";
    }

    if (ui->max_rb->isChecked() || ui->avg_rb->isChecked())
    {
        set_max_color(id_max);
        message += "<br><font color=\"blue\">KAR MAX : " + std::to_string(metrics->at(curr_mesh_id).KAR_max) + "</font>";
    }

    if (ui->avg_rb->isChecked())
    {
        message += "<br>KAR AVG : " + std::to_string(metrics->at(curr_mesh_id).KAR_avg);
    }

    if (ui->tri_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).KAR_min_id;
        id_max = metrics->at(curr_mesh_id).KAR_max_id;

        set_min_color(id_min);
        set_max_color(id_max);

        message += "<br><font color=\"red\">KAR MIN : " + std::to_string(metrics->at(curr_mesh_id).KAR_min) + "</font>";
        message += "<br><font color=\"blue\">KAR MAX : " + std::to_string(metrics->at(curr_mesh_id).KAR_max) + "</font>";
        message += "<br>KAR AVG : " + std::to_string(metrics->at(curr_mesh_id).KAR_avg);
    }

    if (ui->poly_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).KAR_poly_min_id;
        id_max = metrics->at(curr_mesh_id).KAR_poly_max_id;

        set_min_color(id_min);
        set_max_color(id_max);

        message += "<br><font color=\"red\">KAR POLY MIN : " + std::to_string(metrics->at(curr_mesh_id).KAR_poly_min) + "</font>";
        message += "<br><font color=\"blue\">KAR POLY MAX : " + std::to_string(metrics->at(curr_mesh_id).KAR_poly_max) + "</font>";
        message += "<br>KAR POLY AVG : " + std::to_string(metrics->at(curr_mesh_id).KAR_poly_avg);
    }

    if (ui->all_rb->isChecked())
    {
        double min = std::min(metrics->at(curr_mesh_id).KAR_min, metrics->at(curr_mesh_id).KAR_poly_min);
        double max = std::max(metrics->at(curr_mesh_id).KAR_max, metrics->at(curr_mesh_id).KAR_poly_max);

        if (metrics->at(curr_mesh_id).KAR_min < metrics->at(curr_mesh_id).KAR_poly_min)
            id_min = metrics->at(curr_mesh_id).KAR_min_id;
        else
            id_min = metrics->at(curr_mesh_id).KAR_poly_min_id;

        if (metrics->at(curr_mesh_id).KAR_max > metrics->at(curr_mesh_id).KAR_poly_max)
            id_max = metrics->at(curr_mesh_id).KAR_max_id;
        else
            id_max = metrics->at(curr_mesh_id).KAR_poly_max_id;

        std::vector<double> all = metrics->at(curr_mesh_id).KAR_all;

        set_all_color(min, max, all);

        if (ui->highlight_min_max_cb->isChecked())
        {
            highlight_min_max_elements(id_min,id_max);
        }

        ui->min_label->setText(QString(std::to_string(min).c_str()));
        ui->max_label->setText(QString(std::to_string(max).c_str()));
    }

    ui->mesh_metrics_canvas->clear();
    ui->mesh_metrics_canvas->add_mesh(*mesh_with_metrics.at(0), true);

    ui->info_text->setHtml(message.c_str());
}

void MeshMetricsGraphicWidget::show_apr ()
{
    mesh_with_metrics.at(0)->poly_set_color(cinolib::Color::WHITE());

    std::string message = "";

    uint id_min = metrics->at(curr_mesh_id).APR_min_id;
    uint id_max = metrics->at(curr_mesh_id).APR_max_id;

    if (ui->min_rb->isChecked() || ui->avg_rb->isChecked())
    {
        set_min_color(id_min);
        message += "<br><font color=\"red\">APR MIN : " + std::to_string(metrics->at(curr_mesh_id).APR_min) + "</font>";
    }

    if (ui->max_rb->isChecked() || ui->avg_rb->isChecked())
    {
        set_max_color(id_max);
        message += "<br><font color=\"blue\">APR MAX : " + std::to_string(metrics->at(curr_mesh_id).APR_max) + "</font>";
    }

    if (ui->avg_rb->isChecked())
    {
        message += "<br>APR AVG : " + std::to_string(metrics->at(curr_mesh_id).APR_avg);
    }

    if (ui->tri_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).APR_min_id;
        id_max = metrics->at(curr_mesh_id).APR_max_id;

        set_min_color(id_min);
        set_max_color(id_max);

        message += "<br><font color=\"red\">APR MIN : " + std::to_string(metrics->at(curr_mesh_id).APR_min) + "</font>";
        message += "<br><font color=\"blue\">APR MAX : " + std::to_string(metrics->at(curr_mesh_id).APR_max) + "</font>";
        message += "<br>APR AVG : " + std::to_string(metrics->at(curr_mesh_id).APR_avg);
    }

    if (ui->poly_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).APR_poly_min_id;
        id_max = metrics->at(curr_mesh_id).APR_poly_max_id;

        set_min_color(id_min);
        set_max_color(id_max);

        message += "<br><font color=\"red\">APR POLY MIN : " + std::to_string(metrics->at(curr_mesh_id).APR_poly_min) + "</font>";
        message += "<br><font color=\"blue\">APR POLY MAX : " + std::to_string(metrics->at(curr_mesh_id).APR_poly_max) + "</font>";
        message += "<br>APR POLY AVG : " + std::to_string(metrics->at(curr_mesh_id).APR_poly_avg);
    }

    if (ui->all_rb->isChecked())
    {
        double min = std::min(metrics->at(curr_mesh_id).APR_min, metrics->at(curr_mesh_id).APR_poly_min);
        double max = std::max(metrics->at(curr_mesh_id).APR_max, metrics->at(curr_mesh_id).APR_poly_max);

        if (metrics->at(curr_mesh_id).APR_min < metrics->at(curr_mesh_id).APR_poly_min)
            id_min = metrics->at(curr_mesh_id).APR_min_id;
        else
            id_min = metrics->at(curr_mesh_id).APR_poly_min_id;

        if (metrics->at(curr_mesh_id).APR_max > metrics->at(curr_mesh_id).APR_poly_max)
            id_max = metrics->at(curr_mesh_id).APR_max_id;
        else
            id_max = metrics->at(curr_mesh_id).APR_poly_max_id;

        std::vector<double> all = metrics->at(curr_mesh_id).APR_all;

        set_all_color(min, max, all);

        if (ui->highlight_min_max_cb->isChecked())
        {
            highlight_min_max_elements(id_min,id_max);
        }

        ui->min_label->setText(QString(std::to_string(min).c_str()));
        ui->max_label->setText(QString(std::to_string(max).c_str()));
    }

    ui->mesh_metrics_canvas->clear();
    ui->mesh_metrics_canvas->add_mesh(*mesh_with_metrics.at(0), true);

    ui->info_text->setHtml(message.c_str());
}

void MeshMetricsGraphicWidget::show_mia ()
{
    mesh_with_metrics.at(0)->poly_set_color(cinolib::Color::WHITE());

    std::string message = "";

    uint id_min = metrics->at(curr_mesh_id).MIA_min_id;
    uint id_max = metrics->at(curr_mesh_id).MIA_max_id;

    if (ui->min_rb->isChecked() || ui->avg_rb->isChecked())
    {
        set_min_color(id_min);
        message += "<br><font color=\"red\">MIA MIN : " + std::to_string(metrics->at(curr_mesh_id).MIA_min) + "</font>";
    }

    if (ui->max_rb->isChecked() || ui->avg_rb->isChecked())
    {
        set_max_color(id_max);
        message += "<br><font color=\"blue\">MIA MAX : " + std::to_string(metrics->at(curr_mesh_id).MIA_max) + "</font>";
    }

    if (ui->avg_rb->isChecked())
    {
        message += "<br>MIA AVG : " + std::to_string(metrics->at(curr_mesh_id).MIA_avg);
    }

    if (ui->tri_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).MIA_min_id;
        id_max = metrics->at(curr_mesh_id).MIA_max_id;

        set_min_color(id_min);
        set_max_color(id_max);

        message += "<br><font color=\"red\">MIA MIN : " + std::to_string(metrics->at(curr_mesh_id).MIA_min) + "</font>";
        message += "<br><font color=\"blue\">MIA MAX : " + std::to_string(metrics->at(curr_mesh_id).MIA_max) + "</font>";
        message += "<br>MIA AVG : " + std::to_string(metrics->at(curr_mesh_id).MIA_avg);
    }

    if (ui->poly_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).MIA_poly_min_id;
        id_max = metrics->at(curr_mesh_id).MIA_poly_max_id;

        set_min_color(id_min);
        set_max_color(id_max);

        message += "<br><font color=\"red\">MIA POLY MIN : " + std::to_string(metrics->at(curr_mesh_id).MIA_poly_min) + "</font>";
        message += "<br><font color=\"blue\">MIA POLY MAX : " + std::to_string(metrics->at(curr_mesh_id).MIA_poly_max) + "</font>";
        message += "<br>MIA POLY AVG : " + std::to_string(metrics->at(curr_mesh_id).MIA_poly_avg);
    }

    if (ui->all_rb->isChecked())
    {
        double min = std::min(metrics->at(curr_mesh_id).MIA_min, metrics->at(curr_mesh_id).MIA_poly_min);
        double max = std::max(metrics->at(curr_mesh_id).MIA_max, metrics->at(curr_mesh_id).MIA_poly_max);

        if (metrics->at(curr_mesh_id).MIA_min < metrics->at(curr_mesh_id).MIA_poly_min)
            id_min = metrics->at(curr_mesh_id).MIA_min_id;
        else
            id_min = metrics->at(curr_mesh_id).MIA_poly_min_id;

        if (metrics->at(curr_mesh_id).MIA_max > metrics->at(curr_mesh_id).MIA_poly_max)
            id_max = metrics->at(curr_mesh_id).MIA_max_id;
        else
            id_max = metrics->at(curr_mesh_id).MIA_poly_max_id;

        std::vector<double> all = metrics->at(curr_mesh_id).MIA_all;

        set_all_color(min, max, all);

        if (ui->highlight_min_max_cb->isChecked())
        {
            highlight_min_max_elements(id_min,id_max);
        }

        ui->min_label->setText(QString(std::to_string(min).c_str()));
        ui->max_label->setText(QString(std::to_string(max).c_str()));
    }

    ui->mesh_metrics_canvas->clear();
    ui->mesh_metrics_canvas->add_mesh(*mesh_with_metrics.at(0), true);

    ui->info_text->setHtml(message.c_str());
}

void MeshMetricsGraphicWidget::show_maa ()
{
    mesh_with_metrics.at(0)->poly_set_color(cinolib::Color::WHITE());

    std::string message = "";

    uint id_min = metrics->at(curr_mesh_id).MAA_min_id;
    uint id_max = metrics->at(curr_mesh_id).MAA_max_id;

    if (ui->min_rb->isChecked() || ui->avg_rb->isChecked())
    {
        set_min_color(id_min);
        message += "<br><font color=\"red\">MAA MIN : " + std::to_string(metrics->at(curr_mesh_id).MAA_min) + "</font>";
    }

    if (ui->max_rb->isChecked() || ui->avg_rb->isChecked())
    {
        set_max_color(id_max);
        message += "<br><font color=\"blue\">MAA MAX : " + std::to_string(metrics->at(curr_mesh_id).MAA_max) + "</font>";
    }

    if (ui->avg_rb->isChecked())
    {
        message += "<br>MAA AVG : " + std::to_string(metrics->at(curr_mesh_id).MAA_avg);
    }

    if (ui->tri_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).MAA_min_id;
        id_max = metrics->at(curr_mesh_id).MAA_max_id;

        set_min_color(id_min);
        set_max_color(id_max);

        message += "<br><font color=\"red\">MAA MIN : " + std::to_string(metrics->at(curr_mesh_id).MAA_min) + "</font>";
        message += "<br><font color=\"blue\">MAA MAX : " + std::to_string(metrics->at(curr_mesh_id).MAA_max) + "</font>";
        message += "<br>MAA AVG : " + std::to_string(metrics->at(curr_mesh_id).MAA_avg);
    }

    if (ui->poly_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).MAA_poly_min_id;
        id_max = metrics->at(curr_mesh_id).MAA_poly_max_id;

        set_min_color(id_min);
        set_max_color(id_max);

        message += "<br>MAA POLY MIN : " + std::to_string(metrics->at(curr_mesh_id).MAA_poly_min);
        message += "<br>MAA POLY MAX : " + std::to_string(metrics->at(curr_mesh_id).MAA_poly_max);
        message += "<br>MAA POLY AVG : " + std::to_string(metrics->at(curr_mesh_id).MAA_poly_avg);
    }

    if (ui->all_rb->isChecked())
    {
        double min = std::min(metrics->at(curr_mesh_id).MAA_min, metrics->at(curr_mesh_id).MAA_poly_min);
        double max = std::max(metrics->at(curr_mesh_id).MAA_max, metrics->at(curr_mesh_id).MAA_poly_max);

        if (metrics->at(curr_mesh_id).MAA_min < metrics->at(curr_mesh_id).MAA_poly_min)
            id_min = metrics->at(curr_mesh_id).MAA_min_id;
        else
            id_min = metrics->at(curr_mesh_id).MAA_poly_min_id;

        if (metrics->at(curr_mesh_id).MAA_max > metrics->at(curr_mesh_id).MAA_poly_max)
            id_max = metrics->at(curr_mesh_id).MAA_max_id;
        else
            id_max = metrics->at(curr_mesh_id).MAA_poly_max_id;

        std::vector<double> all = metrics->at(curr_mesh_id).MAA_all;

        set_all_color(min, max, all);

        if (ui->highlight_min_max_cb->isChecked())
        {
            highlight_min_max_elements(id_min,id_max);
        }

        ui->min_label->setText(QString(std::to_string(min).c_str()));
        ui->max_label->setText(QString(std::to_string(max).c_str()));
    }

    ui->mesh_metrics_canvas->clear();
    ui->mesh_metrics_canvas->add_mesh(*mesh_with_metrics.at(0), true);

    ui->info_text->setHtml(message.c_str());
}

void MeshMetricsGraphicWidget::show_anr ()
{
    mesh_with_metrics.at(0)->poly_set_color(cinolib::Color::WHITE());

    std::string message = "";

    uint id_min = metrics->at(curr_mesh_id).ANR_min_id;
    uint id_max = metrics->at(curr_mesh_id).ANR_max_id;

    if (ui->min_rb->isChecked() || ui->avg_rb->isChecked())
    {
        set_min_color(id_min);
        message += "<br><font color=\"red\">ANR MIN : " + std::to_string(metrics->at(curr_mesh_id).ANR_min) + "</font>";
    }

    if (ui->max_rb->isChecked() || ui->avg_rb->isChecked())
    {
        set_max_color(id_max);
        message += "<br><font color=\"blue\">ANR MAX : " + std::to_string(metrics->at(curr_mesh_id).ANR_max) + "</font>";
    }

    if (ui->avg_rb->isChecked())
    {
        message += "<br>ANR AVG : " + std::to_string(metrics->at(curr_mesh_id).ANR_avg);
    }

    if (ui->tri_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).ANR_min_id;
        id_max = metrics->at(curr_mesh_id).ANR_max_id;

        set_min_color(id_min);
        set_max_color(id_max);

        message += "<br><font color=\"red\">ANR MIN : " + std::to_string(metrics->at(curr_mesh_id).ANR_min) + "</font>";
        message += "<br><font color=\"blue\">ANR MAX : " + std::to_string(metrics->at(curr_mesh_id).ANR_max) + "</font>";
        message += "<br>ANR AVG : " + std::to_string(metrics->at(curr_mesh_id).ANR_avg);
    }

    if (ui->poly_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).ANR_poly_min_id;
        id_max = metrics->at(curr_mesh_id).ANR_poly_max_id;

        set_min_color(id_min);
        set_max_color(id_max);

        message += "<br>ANR POLY MIN : " + std::to_string(metrics->at(curr_mesh_id).ANR_poly_min);
        message += "<br>ANR POLY MAX : " + std::to_string(metrics->at(curr_mesh_id).ANR_poly_max);
        message += "<br>ANR POLY AVG : " + std::to_string(metrics->at(curr_mesh_id).ANR_poly_avg);
    }

    if (ui->all_rb->isChecked())
    {
        double min = std::min(metrics->at(curr_mesh_id).ANR_min, metrics->at(curr_mesh_id).ANR_poly_min);
        double max = std::max(metrics->at(curr_mesh_id).ANR_max, metrics->at(curr_mesh_id).ANR_poly_max);

        if (metrics->at(curr_mesh_id).ANR_min < metrics->at(curr_mesh_id).ANR_poly_min)
            id_min = metrics->at(curr_mesh_id).ANR_min_id;
        else
            id_min = metrics->at(curr_mesh_id).ANR_poly_min_id;

        if (metrics->at(curr_mesh_id).ANR_max > metrics->at(curr_mesh_id).ANR_poly_max)
            id_max = metrics->at(curr_mesh_id).ANR_max_id;
        else
            id_max = metrics->at(curr_mesh_id).ANR_poly_max_id;

        std::vector<double> all = metrics->at(curr_mesh_id).ANR_all;

        set_all_color(min, max, all);

        if (ui->highlight_min_max_cb->isChecked())
        {
            highlight_min_max_elements(id_min,id_max);
        }

        ui->min_label->setText(QString(std::to_string(min).c_str()));
        ui->max_label->setText(QString(std::to_string(max).c_str()));
    }

    ui->mesh_metrics_canvas->clear();
    ui->mesh_metrics_canvas->add_mesh(*mesh_with_metrics.at(0), true);

    ui->info_text->setHtml(message.c_str());
}

void MeshMetricsGraphicWidget::show_vem ()
{
    mesh_with_metrics.at(0)->poly_set_color(cinolib::Color::WHITE());

    std::string message = "";

    uint id_min = metrics->at(curr_mesh_id).VEM_min_id;
    uint id_max = metrics->at(curr_mesh_id).VEM_max_id;

    if (ui->min_rb->isChecked() || ui->avg_rb->isChecked())
    {
        set_min_color(id_min);
        message += "<br><font color=\"red\">VEM MIN : " + std::to_string(metrics->at(curr_mesh_id).VEM_min) + "</font>";
    }

    if (ui->max_rb->isChecked() || ui->avg_rb->isChecked())
    {
        set_max_color(id_max);
        message += "<br><font color=\"blue\">VEM MAX : " + std::to_string(metrics->at(curr_mesh_id).VEM_max)+ "</font>";
    }

    if (ui->avg_rb->isChecked())
    {
        message += "<br>VEM AVG : " + std::to_string(metrics->at(curr_mesh_id).VEM_avg);
    }

    if (ui->tri_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).VEM_min_id;
        id_max = metrics->at(curr_mesh_id).VEM_max_id;

        set_min_color(id_min);
        set_max_color(id_max);

        message += "<br><font color=\"red\">VEM MIN : " + std::to_string(metrics->at(curr_mesh_id).VEM_min) + "</font>";
        message += "<br><font color=\"blue\">VEM MAX : " + std::to_string(metrics->at(curr_mesh_id).VEM_max) + "</font>";
        message += "<br>VEM AVG : " + std::to_string(metrics->at(curr_mesh_id).VEM_avg);
    }

    if (ui->poly_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).VEM_poly_min_id;
        id_max = metrics->at(curr_mesh_id).VEM_poly_max_id;

        set_min_color(id_min);
        set_max_color(id_max);

        message += "<br><font color=\"red\">VEM POLY MIN : " + std::to_string(metrics->at(curr_mesh_id).VEM_poly_min) + "</font>";
        message += "<br><font color=\"blue\">VEM POLY MAX : " + std::to_string(metrics->at(curr_mesh_id).VEM_poly_max) + "</font>";
        message += "<br>VEM POLY AVG : " + std::to_string(metrics->at(curr_mesh_id).VEM_poly_avg);
    }

    if (ui->all_rb->isChecked())
    {
        double min = std::min(metrics->at(curr_mesh_id).VEM_min, metrics->at(curr_mesh_id).VEM_poly_min);
        double max = std::max(metrics->at(curr_mesh_id).VEM_max, metrics->at(curr_mesh_id).VEM_poly_max);

        if (metrics->at(curr_mesh_id).VEM_min < metrics->at(curr_mesh_id).VEM_poly_min)
            id_min = metrics->at(curr_mesh_id).VEM_min_id;
        else
            id_min = metrics->at(curr_mesh_id).VEM_poly_min_id;

        if (metrics->at(curr_mesh_id).VEM_max > metrics->at(curr_mesh_id).VEM_poly_max)
            id_max = metrics->at(curr_mesh_id).VEM_max_id;
        else
            id_max = metrics->at(curr_mesh_id).VEM_poly_max_id;

        std::vector<double> all = metrics->at(curr_mesh_id).VEM_all;

        set_all_color(min, max, all);

        if (ui->highlight_min_max_cb->isChecked())
        {
            highlight_min_max_elements(id_min,id_max);
        }

        ui->min_label->setText(QString(std::to_string(min).c_str()));
        ui->max_label->setText(QString(std::to_string(max).c_str()));
    }

    ui->mesh_metrics_canvas->clear();
    ui->mesh_metrics_canvas->add_mesh(*mesh_with_metrics.at(0), true);

    ui->info_text->setHtml(message.c_str());
}

void MeshMetricsGraphicWidget::show_jac ()
{
    mesh_with_metrics.at(0)->poly_set_color(cinolib::Color::WHITE());

    std::string message = "";

    uint id_min = metrics->at(curr_mesh_id).JAC_min_id;
    uint id_max = metrics->at(curr_mesh_id).JAC_max_id;

    if (ui->min_rb->isChecked() || ui->avg_rb->isChecked())
    {
        set_min_color(id_min);
        message += "<br><font color=\"red\">JAC MIN : " + std::to_string(metrics->at(curr_mesh_id).JAC_min) + "</font>";
    }

    if (ui->max_rb->isChecked() || ui->avg_rb->isChecked())
    {
        set_max_color(id_max);
        message += "<br><font color=\"blue\">JAC MAX : " + std::to_string(metrics->at(curr_mesh_id).JAC_max)+ "</font>";
    }

    if (ui->avg_rb->isChecked())
    {
        message += "<br>JAC AVG : " + std::to_string(metrics->at(curr_mesh_id).JAC_avg);
    }

    if (ui->tri_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).JAC_min_id;
        id_max = metrics->at(curr_mesh_id).JAC_max_id;

        set_min_color(id_min);
        set_max_color(id_max);

        message += "<br><font color=\"red\">JAC MIN : " + std::to_string(metrics->at(curr_mesh_id).JAC_min) + "</font>";
        message += "<br><font color=\"blue\">JAC MAX : " + std::to_string(metrics->at(curr_mesh_id).JAC_max) + "</font>";
        message += "<br>JAC AVG : " + std::to_string(metrics->at(curr_mesh_id).JAC_avg);
    }

    if (ui->poly_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).JAC_poly_min_id;
        id_max = metrics->at(curr_mesh_id).JAC_poly_max_id;

        set_min_color(id_min);
        set_max_color(id_max);

        message += "<br><font color=\"red\">JAC POLY MIN : " + std::to_string(metrics->at(curr_mesh_id).JAC_poly_min) + "</font>";
        message += "<br><font color=\"blue\">JAC POLY MAX : " + std::to_string(metrics->at(curr_mesh_id).JAC_poly_max) + "</font>";
        message += "<br>JAC POLY AVG : " + std::to_string(metrics->at(curr_mesh_id).JAC_poly_avg);
    }

    if (ui->all_rb->isChecked())
    {
        double min = std::min(metrics->at(curr_mesh_id).JAC_min, metrics->at(curr_mesh_id).JAC_poly_min);
        double max = std::max(metrics->at(curr_mesh_id).JAC_max, metrics->at(curr_mesh_id).JAC_poly_max);

        if (metrics->at(curr_mesh_id).JAC_min < metrics->at(curr_mesh_id).JAC_poly_min)
            id_min = metrics->at(curr_mesh_id).JAC_min_id;
        else
            id_min = metrics->at(curr_mesh_id).JAC_poly_min_id;

        if (metrics->at(curr_mesh_id).JAC_max > metrics->at(curr_mesh_id).JAC_poly_max)
            id_max = metrics->at(curr_mesh_id).JAC_max_id;
        else
            id_max = metrics->at(curr_mesh_id).JAC_poly_max_id;

        std::vector<double> all = metrics->at(curr_mesh_id).JAC_all;

        set_all_color(min, max, all);

        if (ui->highlight_min_max_cb->isChecked())
        {
            highlight_min_max_elements(id_min,id_max);
        }

        ui->min_label->setText(QString(std::to_string(min).c_str()));
        ui->max_label->setText(QString(std::to_string(max).c_str()));
    }

    ui->mesh_metrics_canvas->clear();
    ui->mesh_metrics_canvas->add_mesh(*mesh_with_metrics.at(0), true);

    ui->info_text->setHtml(message.c_str());
}

void MeshMetricsGraphicWidget::show_fro ()
{
    mesh_with_metrics.at(0)->poly_set_color(cinolib::Color::WHITE());

    std::string message = "";

    uint id_min = metrics->at(curr_mesh_id).FRO_min_id;
    uint id_max = metrics->at(curr_mesh_id).FRO_max_id;

    if (ui->min_rb->isChecked() || ui->avg_rb->isChecked())
    {
        set_min_color(id_min);
        message += "<br><font color=\"red\">FRO MIN : " + std::to_string(metrics->at(curr_mesh_id).FRO_min) + "</font>";
    }

    if (ui->max_rb->isChecked() || ui->avg_rb->isChecked())
    {
        set_max_color(id_max);
        message += "<br><font color=\"blue\">FRO MAX : " + std::to_string(metrics->at(curr_mesh_id).FRO_max)+ "</font>";
    }

    if (ui->avg_rb->isChecked())
    {
        message += "<br>FRO AVG : " + std::to_string(metrics->at(curr_mesh_id).FRO_avg);
    }

    if (ui->tri_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).FRO_min_id;
        id_max = metrics->at(curr_mesh_id).FRO_max_id;

        set_min_color(id_min);
        set_max_color(id_max);

        message += "<br><font color=\"red\">FRO MIN : " + std::to_string(metrics->at(curr_mesh_id).FRO_min) + "</font>";
        message += "<br><font color=\"blue\">FRO MAX : " + std::to_string(metrics->at(curr_mesh_id).FRO_max) + "</font>";
        message += "<br>FRO AVG : " + std::to_string(metrics->at(curr_mesh_id).FRO_avg);
    }

    if (ui->poly_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).FRO_poly_min_id;
        id_max = metrics->at(curr_mesh_id).FRO_poly_max_id;

        set_min_color(id_min);
        set_max_color(id_max);

        message += "<br><font color=\"red\">FRO POLY MIN : " + std::to_string(metrics->at(curr_mesh_id).FRO_poly_min) + "</font>";
        message += "<br><font color=\"blue\">FRO POLY MAX : " + std::to_string(metrics->at(curr_mesh_id).FRO_poly_max) + "</font>";
        message += "<br>FRO POLY AVG : " + std::to_string(metrics->at(curr_mesh_id).FRO_poly_avg);
    }

    if (ui->all_rb->isChecked())
    {
        double min = std::min(metrics->at(curr_mesh_id).FRO_min, metrics->at(curr_mesh_id).FRO_poly_min);
        double max = std::max(metrics->at(curr_mesh_id).FRO_max, metrics->at(curr_mesh_id).FRO_poly_max);

        if (metrics->at(curr_mesh_id).FRO_min < metrics->at(curr_mesh_id).FRO_poly_min)
            id_min = metrics->at(curr_mesh_id).FRO_min_id;
        else
            id_min = metrics->at(curr_mesh_id).FRO_poly_min_id;

        if (metrics->at(curr_mesh_id).FRO_max > metrics->at(curr_mesh_id).FRO_poly_max)
            id_max = metrics->at(curr_mesh_id).FRO_max_id;
        else
            id_max = metrics->at(curr_mesh_id).FRO_poly_max_id;

        std::vector<double> all = metrics->at(curr_mesh_id).FRO_all;

        set_all_color(min, max, all);

        if (ui->highlight_min_max_cb->isChecked())
        {
            highlight_min_max_elements(id_min,id_max);
        }

        ui->min_label->setText(QString(std::to_string(min).c_str()));
        ui->max_label->setText(QString(std::to_string(max).c_str()));
    }

    ui->mesh_metrics_canvas->clear();
    ui->mesh_metrics_canvas->add_mesh(*mesh_with_metrics.at(0), true);

    ui->info_text->setHtml(message.c_str());
}


void MeshMetricsGraphicWidget::show_min()
{
    mesh_with_metrics.at(0)->poly_set_color(cinolib::Color::WHITE());

    uint id;
    std::string message = "";

    if (ui->inr_rb->isChecked())
    {
        id = metrics->at(curr_mesh_id).INR_min_id;
        message = "INR MIN : " + std::to_string(metrics->at(curr_mesh_id).INR_min);
    }
    else
    if (ui->our_rb->isChecked())
    {
        id = metrics->at(curr_mesh_id).OUR_min_id;
        message = "OUR MIN : " + std::to_string(metrics->at(curr_mesh_id).OUR_min);
    }
    else
    if (ui->cir_rb->isChecked())
    {
        id = metrics->at(curr_mesh_id).CIR_min_id;
        message = "CIR MIN : " + std::to_string(metrics->at(curr_mesh_id).CIR_min);
    }
    else
    if (ui->krr_rb->isChecked())
    {
        id = metrics->at(curr_mesh_id).KRR_min_id;
        message = "KRR MIN : " + std::to_string(metrics->at(curr_mesh_id).KRR_min);
    }
    else
    if (ui->kar_rb->isChecked())
    {
        id = metrics->at(curr_mesh_id).KAR_min_id;
        message = "KAR MIN : " + std::to_string(metrics->at(curr_mesh_id).KAR_min);
    }
    else
    if (ui->apr_rb->isChecked())
    {
        id = metrics->at(curr_mesh_id).APR_min_id;
        message = "APR MIN : " + std::to_string(metrics->at(curr_mesh_id).APR_min);
    }
    else
    if (ui->mia_rb->isChecked())
    {
        id = metrics->at(curr_mesh_id).MIA_min_id;
        message = "MIA MIN : " + std::to_string(metrics->at(curr_mesh_id).MIA_min);
    }
    else
    if (ui->maa_rb->isChecked())
    {
        id = metrics->at(curr_mesh_id).MAA_min_id;
        message = "MAA MIN : " + std::to_string(metrics->at(curr_mesh_id).MAA_min);
    }
    else
    if (ui->anr_rb->isChecked())
    {
        id = metrics->at(curr_mesh_id).ANR_min_id;
        message = "ANR MIN : " + std::to_string(metrics->at(curr_mesh_id).ANR_min);
    }
    else
    if (ui->vem_rb->isChecked())
    {
        id = metrics->at(curr_mesh_id).VEM_min_id;
        message = "VEM MIN : " + std::to_string(metrics->at(curr_mesh_id).VEM_min);
    }
    else
    if (ui->jac_rb->isChecked())
    {
        id = metrics->at(curr_mesh_id).JAC_min_id;
        message = "JAC MIN : " + std::to_string(metrics->at(curr_mesh_id).JAC_min);
    }
    else
    if (ui->fro_rb->isChecked())
    {
        id = metrics->at(curr_mesh_id).FRO_min_id;
        message = "FRO MIN : " + std::to_string(metrics->at(curr_mesh_id).FRO_min);
    }
    else
    {
        return;
    }

    set_min_color(id);

    ui->mesh_metrics_canvas->clear();
    ui->mesh_metrics_canvas->add_mesh(*mesh_with_metrics.at(0), true);

    message = "<br><font color=\"red\">" + message + "</font>";

    ui->info_text->setHtml(message.c_str());
}

void MeshMetricsGraphicWidget::show_max()
{
    mesh_with_metrics.at(0)->poly_set_color(cinolib::Color::WHITE());

    uint id;
    std::string message = "";

    if (ui->inr_rb->isChecked())
    {
        id = metrics->at(curr_mesh_id).INR_max_id;
        message = "INR MAX : " + std::to_string(metrics->at(curr_mesh_id).INR_max);
    }
    else
    if (ui->our_rb->isChecked())
    {
        id = metrics->at(curr_mesh_id).OUR_max_id;
        message = "OUR MAX : " + std::to_string(metrics->at(curr_mesh_id).OUR_max);
    }
    else
    if (ui->cir_rb->isChecked())
    {
        id = metrics->at(curr_mesh_id).CIR_max_id;
        message = "CIR MAX : " + std::to_string(metrics->at(curr_mesh_id).CIR_max);
    }
    else
    if (ui->krr_rb->isChecked())
    {
        id = metrics->at(curr_mesh_id).KRR_max_id;
        message = "KRR MAX : " + std::to_string(metrics->at(curr_mesh_id).KRR_max);
    }
    else
    if (ui->kar_rb->isChecked())
    {
        id = metrics->at(curr_mesh_id).KAR_max_id;
        message = "KAR MAX : " + std::to_string(metrics->at(curr_mesh_id).KAR_max);
    }
    else
    if (ui->apr_rb->isChecked())
    {
        id = metrics->at(curr_mesh_id).APR_max_id;
        message = "APR MAX : " + std::to_string(metrics->at(curr_mesh_id).APR_max);
    }
    else
    if (ui->mia_rb->isChecked())
    {
        id = metrics->at(curr_mesh_id).MIA_max_id;
        message = "MIA MAX : " + std::to_string(metrics->at(curr_mesh_id).MIA_max);
    }
    else
    if (ui->maa_rb->isChecked())
    {
        id = metrics->at(curr_mesh_id).MAA_max_id;
        message = "MAA MAX : " + std::to_string(metrics->at(curr_mesh_id).MAA_max);
    }
    else
    if (ui->anr_rb->isChecked())
    {
        id = metrics->at(curr_mesh_id).ANR_max_id;
        message = "ANR MAX : " + std::to_string(metrics->at(curr_mesh_id).ANR_max);
    }
    else
    if (ui->vem_rb->isChecked())
    {
        id = metrics->at(curr_mesh_id).VEM_max_id;
        message = "VEM MAX : " + std::to_string(metrics->at(curr_mesh_id).VEM_max);
    }
    else
    if (ui->jac_rb->isChecked())
    {
        id = metrics->at(curr_mesh_id).JAC_max_id;
        message = "JAC MAX : " + std::to_string(metrics->at(curr_mesh_id).JAC_max);
    }
    else
    if (ui->fro_rb->isChecked())
    {
        id = metrics->at(curr_mesh_id).FRO_max_id;
        message = "FRO MAX : " + std::to_string(metrics->at(curr_mesh_id).FRO_max);
    }
    else
    {
        return;
    }

    set_max_color(id);

    ui->mesh_metrics_canvas->clear();
    ui->mesh_metrics_canvas->add_mesh(*mesh_with_metrics.at(0), true);

    message = "<br><font color=\"blue\">" + message + "</font>";

    ui->info_text->setHtml(message.c_str());
}

void MeshMetricsGraphicWidget::show_avg()
{
    mesh_with_metrics.at(0)->poly_set_color(cinolib::Color::WHITE());

    uint id_min, id_max;
    std::string message = "";

    if (ui->inr_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).INR_min_id;
        id_max = metrics->at(curr_mesh_id).INR_max_id;
        message += "<br><font color=\"red\">INR MIN : " + std::to_string(metrics->at(curr_mesh_id).INR_min) + "</font>";
        message += "<br><font color=\"blue\">INR MAX : "  + std::to_string(metrics->at(curr_mesh_id).INR_max) + "</font>";
        message += "<br>INR AVG : " + std::to_string(metrics->at(curr_mesh_id).INR_avg);
    }
    else
    if (ui->our_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).OUR_min_id;
        id_max = metrics->at(curr_mesh_id).OUR_max_id;
        message += "<br><font color=\"red\">OUR MIN : " + std::to_string(metrics->at(curr_mesh_id).OUR_min) + "</font>";
        message += "<br><font color=\"blue\">OUR MAX : "  + std::to_string(metrics->at(curr_mesh_id).OUR_max) + "</font>";
        message += "<br>OUR AVG : " + std::to_string(metrics->at(curr_mesh_id).OUR_avg);
    }
    else
    if (ui->cir_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).CIR_min_id;
        id_max = metrics->at(curr_mesh_id).CIR_max_id;
        message += "<br><font color=\"red\">CIR MIN : " + std::to_string(metrics->at(curr_mesh_id).CIR_min) + "</font>";
        message += "<br><font color=\"blue\">CIR MAX : "  + std::to_string(metrics->at(curr_mesh_id).CIR_max) + "</font>";
        message += "<br>CIR AVG : " + std::to_string(metrics->at(curr_mesh_id).CIR_avg);
    }
    else
    if (ui->krr_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).KRR_min_id;
        id_max = metrics->at(curr_mesh_id).KRR_max_id;
        message += "<br><font color=\"red\">KRR MIN : " + std::to_string(metrics->at(curr_mesh_id).KRR_min) + "</font>";
        message += "<br><font color=\"blue\">KRR MAX : "  + std::to_string(metrics->at(curr_mesh_id).KRR_max) + "</font>";
        message += "<br>KRR AVG : " + std::to_string(metrics->at(curr_mesh_id).KRR_avg);
    }
    else
    if (ui->kar_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).KAR_min_id;
        id_max = metrics->at(curr_mesh_id).KAR_max_id;
        message += "<br><font color=\"red\">KAR MIN : " + std::to_string(metrics->at(curr_mesh_id).KAR_min) + "</font>";
        message += "<br><font color=\"blue\">KAR MAX : "  + std::to_string(metrics->at(curr_mesh_id).KAR_max) + "</font>";
        message += "<br>KAR AVG : " + std::to_string(metrics->at(curr_mesh_id).KAR_avg);
    }
    else
    if (ui->apr_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).APR_min_id;
        id_max = metrics->at(curr_mesh_id).APR_max_id;
        message += "<br><font color=\"red\">APR MIN : " + std::to_string(metrics->at(curr_mesh_id).APR_min) + "</font>";
        message += "<br><font color=\"blue\">APR MAX : "  + std::to_string(metrics->at(curr_mesh_id).APR_max) + "</font>";
        message += "<br>APR AVG : " + std::to_string(metrics->at(curr_mesh_id).APR_avg);
    }
    else
    if (ui->mia_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).MIA_min_id;
        id_max = metrics->at(curr_mesh_id).MIA_max_id;
        message += "<br><font color=\"red\">MIA MIN : " + std::to_string(metrics->at(curr_mesh_id).MIA_min) + "</font>";
        message += "<br><font color=\"blue\">MIA MAX : "  + std::to_string(metrics->at(curr_mesh_id).MIA_max) + "</font>";
        message += "<br>MIA AVG : " + std::to_string(metrics->at(curr_mesh_id).MIA_avg);
    }
    else
    if (ui->maa_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).MAA_min_id;
        id_max = metrics->at(curr_mesh_id).MAA_max_id;
        message += "<br><font color=\"red\">MAA MIN : " + std::to_string(metrics->at(curr_mesh_id).MAA_min) + "</font>";
        message += "<br><font color=\"blue\">MAA MAX : "  + std::to_string(metrics->at(curr_mesh_id).MAA_max) + "</font>";
        message += "<br>MAA AVG : " + std::to_string(metrics->at(curr_mesh_id).MAA_avg);
    }
    else
    if (ui->anr_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).ANR_min_id;
        id_max = metrics->at(curr_mesh_id).ANR_max_id;
        message += "<br><font color=\"red\">ANR MIN : " + std::to_string(metrics->at(curr_mesh_id).ANR_min) + "</font>";
        message += "<br><font color=\"blue\">ANR MAX : "  + std::to_string(metrics->at(curr_mesh_id).ANR_max) + "</font>";
        message += "<br>ANR AVG : " + std::to_string(metrics->at(curr_mesh_id).ANR_avg);
    }
    else
    if (ui->vem_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).VEM_min_id;
        id_max = metrics->at(curr_mesh_id).VEM_max_id;
        message += "<br><font color=\"red\">VEM MIN : " + std::to_string(metrics->at(curr_mesh_id).VEM_min) + "</font>";
        message += "<br><font color=\"blue\">VEM MAX : "  + std::to_string(metrics->at(curr_mesh_id).VEM_max) + "</font>";
        message += "<br>VEM AVG : " + std::to_string(metrics->at(curr_mesh_id).VEM_avg);
    }
    else
    if (ui->jac_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).JAC_min_id;
        id_max = metrics->at(curr_mesh_id).JAC_max_id;
        message += "<br><font color=\"red\">JAC MIN : " + std::to_string(metrics->at(curr_mesh_id).JAC_min) + "</font>";
        message += "<br><font color=\"blue\">JAC MAX : "  + std::to_string(metrics->at(curr_mesh_id).JAC_max) + "</font>";
        message += "<br>JAC AVG : " + std::to_string(metrics->at(curr_mesh_id).JAC_avg);
    }
    else
    if (ui->fro_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).FRO_min_id;
        id_max = metrics->at(curr_mesh_id).FRO_max_id;
        message += "<br><font color=\"red\">FRO MIN : " + std::to_string(metrics->at(curr_mesh_id).FRO_min) + "</font>";
        message += "<br><font color=\"blue\">FRO MAX : "  + std::to_string(metrics->at(curr_mesh_id).FRO_max) + "</font>";
        message += "<br>FRO AVG : " + std::to_string(metrics->at(curr_mesh_id).FRO_avg);
    }
    else
    {
        return;
    }

    set_min_color(id_min);
    set_max_color(id_max);

    ui->mesh_metrics_canvas->clear();
    ui->mesh_metrics_canvas->add_mesh(*mesh_with_metrics.at(0), true);

    ui->info_text->setHtml(message.c_str());
}

void MeshMetricsGraphicWidget::show_tri()
{
    mesh_with_metrics.at(0)->poly_set_color(cinolib::Color::WHITE());

    uint id_min, id_max;
    std::string message = "";

    if (ui->inr_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).INR_min_id;
        id_max = metrics->at(curr_mesh_id).INR_max_id;
        message += "<br><font color=\"red\">INR TRI MIN : " + std::to_string(metrics->at(curr_mesh_id).INR_min) + "</font>";
        message += "<br><font color=\"blue\">INR TRI MAX : "  + std::to_string(metrics->at(curr_mesh_id).INR_max) + "</font>";
        message += "<br>INR TRI AVG : " + std::to_string(metrics->at(curr_mesh_id).INR_avg);
    }
    else
    if (ui->our_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).OUR_min_id;
        id_max = metrics->at(curr_mesh_id).OUR_max_id;
        message += "<br><font color=\"red\">OUR TRI MIN : " + std::to_string(metrics->at(curr_mesh_id).OUR_min) + "</font>";
        message += "<br><font color=\"blue\">OUR TRI MAX : "  + std::to_string(metrics->at(curr_mesh_id).OUR_max) + "</font>";
        message += "<br>OUR TRI AVG : " + std::to_string(metrics->at(curr_mesh_id).OUR_avg);
    }
    else
    if (ui->cir_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).CIR_min_id;
        id_max = metrics->at(curr_mesh_id).CIR_max_id;
        message += "<br><font color=\"red\">CIR TRI MIN : " + std::to_string(metrics->at(curr_mesh_id).CIR_min) + "</font>";
        message += "<br><font color=\"blue\">CIR TRI MAX : "  + std::to_string(metrics->at(curr_mesh_id).CIR_max) + "</font>";
        message += "<br>CIR TRI AVG : " + std::to_string(metrics->at(curr_mesh_id).CIR_avg);
    }
    else
    if (ui->krr_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).KRR_min_id;
        id_max = metrics->at(curr_mesh_id).KRR_max_id;
        message += "<br><font color=\"red\">KRR TRI MIN : " + std::to_string(metrics->at(curr_mesh_id).KRR_min) + "</font>";
        message += "<br><font color=\"blue\">KRR TRI MAX : "  + std::to_string(metrics->at(curr_mesh_id).KRR_max) + "</font>";
        message += "<br>KRR TRI AVG : " + std::to_string(metrics->at(curr_mesh_id).KRR_avg);
    }
    else
    if (ui->kar_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).KAR_min_id;
        id_max = metrics->at(curr_mesh_id).KAR_max_id;
        message += "<br><font color=\"red\">KAR TRI MIN : " + std::to_string(metrics->at(curr_mesh_id).KAR_min) + "</font>";
        message += "<br><font color=\"blue\">KAR TRI MAX : "  + std::to_string(metrics->at(curr_mesh_id).KAR_max) + "</font>";
        message += "<br>KAR TRI AVG : " + std::to_string(metrics->at(curr_mesh_id).KAR_avg);
    }
    else
    if (ui->apr_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).APR_min_id;
        id_max = metrics->at(curr_mesh_id).APR_max_id;
        message += "<br><font color=\"red\">APR TRI MIN: " + std::to_string(metrics->at(curr_mesh_id).APR_min) + "</font>";
        message += "<br><font color=\"blue\">APR TRI MAX : " + std::to_string(metrics->at(curr_mesh_id).APR_max) + "</font>";
        message += "<br>APR TRI AVG : " + std::to_string(metrics->at(curr_mesh_id).APR_avg);
    }
    else
    if (ui->mia_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).MIA_min_id;
        id_max = metrics->at(curr_mesh_id).MIA_max_id;
        message += "<br><font color=\"red\">MIA TRI MIN : " + std::to_string(metrics->at(curr_mesh_id).MIA_min) + "</font>";
        message += "<br><font color=\"blue\">MIA TRI MAX : "  + std::to_string(metrics->at(curr_mesh_id).MIA_max) + "</font>";
        message += "<br>MIA TRI AVG : " + std::to_string(metrics->at(curr_mesh_id).MIA_avg);
    }
    else
    if (ui->maa_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).MAA_min_id;
        id_max = metrics->at(curr_mesh_id).MAA_max_id;
        message += "<br><font color=\"red\">MAA TRI MIN : " + std::to_string(metrics->at(curr_mesh_id).MAA_min) + "</font>";
        message += "<br><font color=\"blue\">MAA TRI MAX : "  + std::to_string(metrics->at(curr_mesh_id).MAA_max) + "</font>";
        message += "<br>MAA TRI AVG : " + std::to_string(metrics->at(curr_mesh_id).MAA_avg);
    }
    else
    if (ui->anr_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).ANR_min_id;
        id_max = metrics->at(curr_mesh_id).ANR_max_id;
        message += "<br><font color=\"red\">ANR TRI MIN : " + std::to_string(metrics->at(curr_mesh_id).ANR_min) + "</font>";
        message += "<br><font color=\"blue\">ANR TRI MAX : "  + std::to_string(metrics->at(curr_mesh_id).ANR_max) + "</font>";
        message += "<br>ANR TRI AVG : " + std::to_string(metrics->at(curr_mesh_id).ANR_avg);
    }
    else
    if (ui->vem_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).VEM_min_id;
        id_max = metrics->at(curr_mesh_id).VEM_max_id;
        message += "<br><font color=\"red\">VEM TRI MIN : " + std::to_string(metrics->at(curr_mesh_id).VEM_min) + "</font>";
        message += "<br><font color=\"blue\">VEM TRI MAX : "  + std::to_string(metrics->at(curr_mesh_id).VEM_max) + "</font>";
        message += "<br>VEM TRI AVG : " + std::to_string(metrics->at(curr_mesh_id).VEM_avg);
    }
    else
    if (ui->jac_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).JAC_min_id;
        id_max = metrics->at(curr_mesh_id).JAC_max_id;
        message += "<br><font color=\"red\">JAC TRI MIN : " + std::to_string(metrics->at(curr_mesh_id).JAC_min) + "</font>";
        message += "<br><font color=\"blue\">JAC TRI MAX : "  + std::to_string(metrics->at(curr_mesh_id).JAC_max) + "</font>";
        message += "<br>JAC TRI AVG : " + std::to_string(metrics->at(curr_mesh_id).JAC_avg);
    }
    else
    if (ui->fro_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).FRO_min_id;
        id_max = metrics->at(curr_mesh_id).FRO_max_id;
        message += "<br><font color=\"red\">FRO TRI MIN : " + std::to_string(metrics->at(curr_mesh_id).FRO_min) + "</font>";
        message += "<br><font color=\"blue\">FRO TRI MAX : "  + std::to_string(metrics->at(curr_mesh_id).FRO_max) + "</font>";
        message += "<br>FRO TRI AVG : " + std::to_string(metrics->at(curr_mesh_id).FRO_avg);
    }
    else
    {
        return;
    }

    //set_min_color(d->get_parametric_mesh(curr_mesh_id)->num_polys()-1);
    set_min_color(id_min);
    set_max_color(id_max);

    ui->mesh_metrics_canvas->clear();
    ui->mesh_metrics_canvas->add_mesh(*mesh_with_metrics.at(0), true);

    ui->info_text->setHtml(message.c_str());
}

void MeshMetricsGraphicWidget::show_poly()
{
    mesh_with_metrics.at(0)->poly_set_color(cinolib::Color::WHITE());

    uint id_min, id_max;
    std::string message = "";

    if (ui->inr_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).INR_poly_min_id;
        id_max = metrics->at(curr_mesh_id).INR_poly_max_id;
        message += "<br><font color=\"red\">INR POLY MIN : " + std::to_string(metrics->at(curr_mesh_id).INR_poly_min) + "</font>";
        message += "<br><font color=\"blue\">INR POLY MAX : "  + std::to_string(metrics->at(curr_mesh_id).INR_poly_max) + "</font>";
        message += "<br>INR POLY AVG : " + std::to_string(metrics->at(curr_mesh_id).INR_poly_avg);
    }
    else
    if (ui->our_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).OUR_poly_min_id;
        id_max = metrics->at(curr_mesh_id).OUR_poly_max_id;
        message += "<br><font color=\"red\">OUR POLY MIN : " + std::to_string(metrics->at(curr_mesh_id).OUR_poly_min) + "</font>";
        message += "<br><font color=\"blue\">OUR POLY MAX : "  + std::to_string(metrics->at(curr_mesh_id).OUR_poly_max) + "</font>";
        message += "<br>OUR POLY AVG : " + std::to_string(metrics->at(curr_mesh_id).OUR_poly_avg);
    }
    else
    if (ui->cir_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).CIR_poly_min_id;
        id_max = metrics->at(curr_mesh_id).CIR_poly_max_id;
        message += "<br><font color=\"red\">CIR POLY MIN : " + std::to_string(metrics->at(curr_mesh_id).CIR_poly_min) + "</font>";
        message += "<br><font color=\"blue\">CIR POLY MAX : "  + std::to_string(metrics->at(curr_mesh_id).CIR_poly_max) + "</font>";
        message += "<br>CIR POLY AVG : " + std::to_string(metrics->at(curr_mesh_id).CIR_poly_avg);
    }
    else
    if (ui->krr_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).KRR_poly_min_id;
        id_max = metrics->at(curr_mesh_id).KRR_poly_max_id;
        message += "<br><font color=\"red\">KRR POLY MIN : " + std::to_string(metrics->at(curr_mesh_id).KRR_poly_min) + "</font>";
        message += "<br><font color=\"blue\">KRR POLY MAX : "  + std::to_string(metrics->at(curr_mesh_id).KRR_poly_max) + "</font>";
        message += "<br>KRR POLY AVG : " + std::to_string(metrics->at(curr_mesh_id).KRR_poly_avg);
    }
    else
    if (ui->kar_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).KAR_poly_min_id;
        id_max = metrics->at(curr_mesh_id).KAR_poly_max_id;
        message += "<br><font color=\"red\">KAR POLY MIN : " + std::to_string(metrics->at(curr_mesh_id).KAR_poly_min) + "</font>";
        message += "<br><font color=\"blue\">KAR POLY MAX : "  + std::to_string(metrics->at(curr_mesh_id).KAR_poly_max) + "</font>";
        message += "<br>KAR POLY AVG : " + std::to_string(metrics->at(curr_mesh_id).KAR_poly_avg);
    }
    else
    if (ui->apr_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).APR_poly_min_id;
        id_max = metrics->at(curr_mesh_id).APR_poly_max_id;
        message += "<br><font color=\"red\">APR POLY MIN: " + std::to_string(metrics->at(curr_mesh_id).APR_poly_min) + "</font>";
        message += "<br><font color=\"blue\">APR POLY MAX : " + std::to_string(metrics->at(curr_mesh_id).APR_poly_max) + "</font>";
        message += "<br>APR POLY AVG : " + std::to_string(metrics->at(curr_mesh_id).APR_poly_avg);
    }
    else
    if (ui->mia_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).MIA_poly_min_id;
        id_max = metrics->at(curr_mesh_id).MIA_poly_max_id;
        message += "<br><font color=\"red\">MIA POLY MIN : " + std::to_string(metrics->at(curr_mesh_id).MIA_poly_min) + "</font>";
        message += "<br><font color=\"blue\">MIA POLY MAX : "  + std::to_string(metrics->at(curr_mesh_id).MIA_poly_max) + "</font>";
        message += "<br>MIA POLY AVG : " + std::to_string(metrics->at(curr_mesh_id).MIA_poly_avg);
    }
    else
    if (ui->maa_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).MAA_poly_min_id;
        id_max = metrics->at(curr_mesh_id).MAA_poly_max_id;
        message += "<br><font color=\"red\">MAA POLY MIN : " + std::to_string(metrics->at(curr_mesh_id).MAA_poly_min) + "</font>";
        message += "<br><font color=\"blue\">MAA POLY MAX : "  + std::to_string(metrics->at(curr_mesh_id).MAA_poly_max) + "</font>";
        message += "<br>MAA POLY AVG : " + std::to_string(metrics->at(curr_mesh_id).MAA_poly_avg);
    }
    else
    if (ui->anr_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).ANR_poly_min_id;
        id_max = metrics->at(curr_mesh_id).ANR_poly_max_id;
        message += "<br><font color=\"red\">ANR POLY MIN : " + std::to_string(metrics->at(curr_mesh_id).ANR_poly_min) + "</font>";
        message += "<br><font color=\"blue\">ANR POLY MAX : "  + std::to_string(metrics->at(curr_mesh_id).ANR_poly_max) + "</font>";
        message += "<br>ANR POLY AVG : " + std::to_string(metrics->at(curr_mesh_id).ANR_poly_avg);
    }
    else
    if (ui->vem_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).VEM_poly_min_id;
        id_max = metrics->at(curr_mesh_id).VEM_poly_max_id;
        message += "<br><font color=\"red\">VEM POLY MIN : " + std::to_string(metrics->at(curr_mesh_id).VEM_poly_min) + "</font>";
        message += "<br><font color=\"blue\">VEM POLY MAX : "  + std::to_string(metrics->at(curr_mesh_id).VEM_poly_max) + "</font>";
        message += "<br>VEM AVG : " + std::to_string(metrics->at(curr_mesh_id).VEM_poly_avg);
    }
    else
    if (ui->jac_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).JAC_poly_min_id;
        id_max = metrics->at(curr_mesh_id).JAC_poly_max_id;
        message += "<br><font color=\"red\">JAC POLY MIN : " + std::to_string(metrics->at(curr_mesh_id).JAC_poly_min) + "</font>";
        message += "<br><font color=\"blue\">JAC POLY MAX : "  + std::to_string(metrics->at(curr_mesh_id).JAC_poly_max) + "</font>";
        message += "<br>JAC AVG : " + std::to_string(metrics->at(curr_mesh_id).JAC_poly_avg);
    }
    else
    if (ui->fro_rb->isChecked())
    {
        id_min = metrics->at(curr_mesh_id).FRO_poly_min_id;
        id_max = metrics->at(curr_mesh_id).FRO_poly_max_id;
        message += "<br><font color=\"red\">FRO POLY MIN : " + std::to_string(metrics->at(curr_mesh_id).FRO_poly_min) + "</font>";
        message += "<br><font color=\"blue\">FRO POLY MAX : "  + std::to_string(metrics->at(curr_mesh_id).FRO_poly_max) + "</font>";
        message += "<br>FRO AVG : " + std::to_string(metrics->at(curr_mesh_id).FRO_poly_avg);
    }
    else
    {
        return;
    }

    //set_min_color(d->get_parametric_mesh(curr_mesh_id)->num_polys()-1);
    set_min_color(id_min);
    set_max_color(id_max);

    ui->mesh_metrics_canvas->clear();
    ui->mesh_metrics_canvas->add_mesh(*mesh_with_metrics.at(0), true);

    ui->info_text->setHtml(message.c_str());
}

void MeshMetricsGraphicWidget::show_all()
{
    mesh_with_metrics.at(0)->poly_set_color(cinolib::Color::WHITE());

    double min, max;
    std::vector<double> all;

    std::string message = "";

    uint min_id, max_id;

    if (ui->inr_rb->isChecked())
    {
        min = std::min(metrics->at(curr_mesh_id).INR_min, metrics->at(curr_mesh_id).INR_poly_min);
        max = std::max(metrics->at(curr_mesh_id).INR_max, metrics->at(curr_mesh_id).INR_poly_max);
        all = metrics->at(curr_mesh_id).INR_all;
        message += "<br><font color=\"red\">INR MIN : " + std::to_string(min) + "</font>";
        message += "<br><font color=\"blue\">INR MAX : "  + std::to_string(max) + "</font>";
        message += "<br>INR AVG : " + std::to_string(metrics->at(curr_mesh_id).INR_global_avg);

        if (metrics->at(curr_mesh_id).INR_min < metrics->at(curr_mesh_id).INR_poly_min)
            min_id = metrics->at(curr_mesh_id).INR_min_id;
        else
            min_id = metrics->at(curr_mesh_id).INR_poly_min_id;

        if (metrics->at(curr_mesh_id).INR_max > metrics->at(curr_mesh_id).INR_poly_max)
            max_id = metrics->at(curr_mesh_id).INR_max_id;
        else
            max_id = metrics->at(curr_mesh_id).INR_poly_max_id;

    }
    else
    if (ui->our_rb->isChecked())
    {
        min = std::min(metrics->at(curr_mesh_id).OUR_min, metrics->at(curr_mesh_id).OUR_poly_min);
        max = std::max(metrics->at(curr_mesh_id).OUR_max, metrics->at(curr_mesh_id).OUR_poly_max);
        all = metrics->at(curr_mesh_id).OUR_all;
        message += "<br><font color=\"red\">OUR MIN : " + std::to_string(min) + "</font>";
        message += "<br><font color=\"blue\">OUR MAX : "  + std::to_string(max) + "</font>";
        message += "<br>OUR AVG : " + std::to_string(metrics->at(curr_mesh_id).OUR_global_avg);

        if (metrics->at(curr_mesh_id).OUR_min < metrics->at(curr_mesh_id).OUR_poly_min)
            min_id = metrics->at(curr_mesh_id).OUR_min_id;
        else
            min_id = metrics->at(curr_mesh_id).OUR_poly_min_id;

        if (metrics->at(curr_mesh_id).OUR_max > metrics->at(curr_mesh_id).OUR_poly_max)
            max_id = metrics->at(curr_mesh_id).OUR_max_id;
        else
            max_id = metrics->at(curr_mesh_id).OUR_poly_max_id;

    }
    else
    if (ui->cir_rb->isChecked())
    {
        min = std::min(metrics->at(curr_mesh_id).CIR_min, metrics->at(curr_mesh_id).CIR_poly_min);
        max = std::max(metrics->at(curr_mesh_id).CIR_max, metrics->at(curr_mesh_id).CIR_poly_max);
        all = metrics->at(curr_mesh_id).CIR_all;
        message += "<br><font color=\"red\">CIR MIN : " + std::to_string(min) + "</font>";
        message += "<br><font color=\"blue\">CIR MAX : "  + std::to_string(max) + "</font>";
        message += "<br>CIR AVG : " + std::to_string(metrics->at(curr_mesh_id).CIR_global_avg);

        if (metrics->at(curr_mesh_id).CIR_min < metrics->at(curr_mesh_id).CIR_poly_min)
            min_id = metrics->at(curr_mesh_id).CIR_min_id;
        else
            min_id = metrics->at(curr_mesh_id).CIR_poly_min_id;

        if (metrics->at(curr_mesh_id).CIR_max > metrics->at(curr_mesh_id).CIR_poly_max)
            max_id = metrics->at(curr_mesh_id).CIR_max_id;
        else
            max_id = metrics->at(curr_mesh_id).CIR_poly_max_id;

    }
    else
    if (ui->krr_rb->isChecked())
    {
        min = std::min(metrics->at(curr_mesh_id).KRR_min, metrics->at(curr_mesh_id).KRR_poly_min);
        max = std::max(metrics->at(curr_mesh_id).KRR_max, metrics->at(curr_mesh_id).KRR_poly_max);
        all = metrics->at(curr_mesh_id).KRR_all;
        message += "<br><font color=\"red\">KRR MIN : " + std::to_string(min) + "</font>";
        message += "<br><font color=\"blue\">KRR MAX : "  + std::to_string(max) + "</font>";
        message += "<br>KRR AVG : " + std::to_string(metrics->at(curr_mesh_id).KRR_global_avg);

        if (metrics->at(curr_mesh_id).KRR_min < metrics->at(curr_mesh_id).KRR_poly_min)
            min_id = metrics->at(curr_mesh_id).KRR_min_id;
        else
            min_id = metrics->at(curr_mesh_id).KRR_poly_min_id;

        if (metrics->at(curr_mesh_id).KRR_max > metrics->at(curr_mesh_id).KRR_poly_max)
            max_id = metrics->at(curr_mesh_id).KRR_max_id;
        else
            max_id = metrics->at(curr_mesh_id).KRR_poly_max_id;

    }
    else
    if (ui->kar_rb->isChecked())
    {
        min = std::min(metrics->at(curr_mesh_id).KAR_min, metrics->at(curr_mesh_id).KAR_poly_min);
        max = std::max(metrics->at(curr_mesh_id).KAR_max, metrics->at(curr_mesh_id).KAR_poly_max);
        all = metrics->at(curr_mesh_id).KAR_all;
        message += "<br><font color=\"red\">KAR MIN : " + std::to_string(min) + "</font>";
        message += "<br><font color=\"blue\">KAR MAX : "  + std::to_string(max) + "</font>";
        message += "<br>KAR AVG : " + std::to_string(metrics->at(curr_mesh_id).KAR_global_avg);

        if (metrics->at(curr_mesh_id).KAR_min < metrics->at(curr_mesh_id).KAR_poly_min)
            min_id = metrics->at(curr_mesh_id).KAR_min_id;
        else
            min_id = metrics->at(curr_mesh_id).KAR_poly_min_id;

        if (metrics->at(curr_mesh_id).KAR_max > metrics->at(curr_mesh_id).KAR_poly_max)
            max_id = metrics->at(curr_mesh_id).KAR_max_id;
        else
            max_id = metrics->at(curr_mesh_id).KAR_poly_max_id;

    }
    else
    if (ui->apr_rb->isChecked())
    {
        min = std::min(metrics->at(curr_mesh_id).APR_min, metrics->at(curr_mesh_id).APR_poly_min);
        max = std::max(metrics->at(curr_mesh_id).APR_max, metrics->at(curr_mesh_id).APR_poly_max);
        all = metrics->at(curr_mesh_id).APR_all;
        message += "<br><font color=\"red\">APR MIN : " + std::to_string(min) + "</font>";
        message += "<br><font color=\"blue\">APR MAX : "  + std::to_string(max) + "</font>";
        message += "<br>APR AVG : " + std::to_string(metrics->at(curr_mesh_id).APR_global_avg);

        if (metrics->at(curr_mesh_id).APR_min < metrics->at(curr_mesh_id).APR_poly_min)
            min_id = metrics->at(curr_mesh_id).APR_min_id;
        else
            min_id = metrics->at(curr_mesh_id).APR_poly_min_id;

        if (metrics->at(curr_mesh_id).APR_max > metrics->at(curr_mesh_id).APR_poly_max)
            max_id = metrics->at(curr_mesh_id).APR_max_id;
        else
            max_id = metrics->at(curr_mesh_id).APR_poly_max_id;

    }
    else
    if (ui->mia_rb->isChecked())
    {
        min = std::min(metrics->at(curr_mesh_id).MIA_min, metrics->at(curr_mesh_id).MIA_poly_min);
        max = std::max(metrics->at(curr_mesh_id).MIA_max, metrics->at(curr_mesh_id).MIA_poly_max);
        all = metrics->at(curr_mesh_id).MIA_all;
        message += "<br><font color=\"red\">MIA MIN : " + std::to_string(min) + "</font>";
        message += "<br><font color=\"blue\">MIA MAX : "  + std::to_string(max) + "</font>";
        message += "<br>MIA AVG : " + std::to_string(metrics->at(curr_mesh_id).MIA_global_avg);

        if (metrics->at(curr_mesh_id).MIA_min < metrics->at(curr_mesh_id).MIA_poly_min)
            min_id = metrics->at(curr_mesh_id).MIA_min_id;
        else
            min_id = metrics->at(curr_mesh_id).MIA_poly_min_id;

        if (metrics->at(curr_mesh_id).MIA_max > metrics->at(curr_mesh_id).MIA_poly_max)
            max_id = metrics->at(curr_mesh_id).MIA_max_id;
        else
            max_id = metrics->at(curr_mesh_id).MIA_poly_max_id;

    }
    else
    if (ui->maa_rb->isChecked())
    {
        min = std::min(metrics->at(curr_mesh_id).MAA_min, metrics->at(curr_mesh_id).MAA_poly_min);
        max = std::max(metrics->at(curr_mesh_id).MAA_max, metrics->at(curr_mesh_id).MAA_poly_max);
        all = metrics->at(curr_mesh_id).MAA_all;
        message += "<br><font color=\"red\">MAA MIN : " + std::to_string(min) + "</font>";
        message += "<br><font color=\"blue\">MAA MAX : "  + std::to_string(max) + "</font>";
        message += "<br>MAA AVG : " + std::to_string(metrics->at(curr_mesh_id).MAA_global_avg);

        if (metrics->at(curr_mesh_id).MAA_min < metrics->at(curr_mesh_id).MAA_poly_min)
            min_id = metrics->at(curr_mesh_id).MAA_min_id;
        else
            min_id = metrics->at(curr_mesh_id).MAA_poly_min_id;

        if (metrics->at(curr_mesh_id).MAA_max > metrics->at(curr_mesh_id).MAA_poly_max)
            max_id = metrics->at(curr_mesh_id).MAA_max_id;
        else
            max_id = metrics->at(curr_mesh_id).MAA_poly_max_id;

    }
    else
    if (ui->anr_rb->isChecked())
    {
        min = std::min(metrics->at(curr_mesh_id).ANR_min, metrics->at(curr_mesh_id).ANR_poly_min);
        max = std::max(metrics->at(curr_mesh_id).ANR_max, metrics->at(curr_mesh_id).ANR_poly_max);
        all = metrics->at(curr_mesh_id).ANR_all;
        message += "<br><font color=\"red\">ANR MIN : " + std::to_string(min) + "</font>";
        message += "<br><font color=\"blue\">ANR MAX : "  + std::to_string(max) + "</font>";
        message += "<br>ANR AVG : " + std::to_string(metrics->at(curr_mesh_id).ANR_global_avg);

        if (metrics->at(curr_mesh_id).ANR_min < metrics->at(curr_mesh_id).ANR_poly_min)
            min_id = metrics->at(curr_mesh_id).ANR_min_id;
        else
            min_id = metrics->at(curr_mesh_id).ANR_poly_min_id;

        if (metrics->at(curr_mesh_id).ANR_max > metrics->at(curr_mesh_id).ANR_poly_max)
            max_id = metrics->at(curr_mesh_id).ANR_max_id;
        else
            max_id = metrics->at(curr_mesh_id).ANR_poly_max_id;

    }
    else
    if (ui->vem_rb->isChecked())
    {
        min = std::min(metrics->at(curr_mesh_id).VEM_min, metrics->at(curr_mesh_id).VEM_poly_min);
        max = std::max(metrics->at(curr_mesh_id).VEM_max, metrics->at(curr_mesh_id).VEM_poly_max);
        all = metrics->at(curr_mesh_id).VEM_all;
        message += "<br><font color=\"red\">VEM MIN : " + std::to_string(min) + "</font>";
        message += "<br><font color=\"blue\">VEM MAX : "  + std::to_string(max) + "</font>";
        message += "<br>VEM AVG : " + std::to_string(metrics->at(curr_mesh_id).VEM_global_avg);

        if (metrics->at(curr_mesh_id).VEM_min < metrics->at(curr_mesh_id).VEM_poly_min)
            min_id = metrics->at(curr_mesh_id).VEM_min_id;
        else
            min_id = metrics->at(curr_mesh_id).VEM_poly_min_id;

        if (metrics->at(curr_mesh_id).VEM_max > metrics->at(curr_mesh_id).VEM_poly_max)
            max_id = metrics->at(curr_mesh_id).VEM_max_id;
        else
            max_id = metrics->at(curr_mesh_id).VEM_poly_max_id;

    }
    else
    if (ui->jac_rb->isChecked())
    {
        min = std::min(metrics->at(curr_mesh_id).JAC_min, metrics->at(curr_mesh_id).JAC_poly_min);
        max = std::max(metrics->at(curr_mesh_id).JAC_max, metrics->at(curr_mesh_id).JAC_poly_max);
        all = metrics->at(curr_mesh_id).JAC_all;
        message += "<br><font color=\"red\">JAC MIN : " + std::to_string(min) + "</font>";
        message += "<br><font color=\"blue\">JAC MAX : "  + std::to_string(max) + "</font>";
        message += "<br>JAC AVG : " + std::to_string(metrics->at(curr_mesh_id).JAC_global_avg);

        if (metrics->at(curr_mesh_id).JAC_min < metrics->at(curr_mesh_id).JAC_poly_min)
            min_id = metrics->at(curr_mesh_id).JAC_min_id;
        else
            min_id = metrics->at(curr_mesh_id).JAC_poly_min_id;

        if (metrics->at(curr_mesh_id).JAC_max > metrics->at(curr_mesh_id).JAC_poly_max)
            max_id = metrics->at(curr_mesh_id).JAC_max_id;
        else
            max_id = metrics->at(curr_mesh_id).JAC_poly_max_id;

    }
    else
    if (ui->fro_rb->isChecked())
    {
        min = std::min(metrics->at(curr_mesh_id).FRO_min, metrics->at(curr_mesh_id).FRO_poly_min);
        max = std::max(metrics->at(curr_mesh_id).FRO_max, metrics->at(curr_mesh_id).FRO_poly_max);
        all = metrics->at(curr_mesh_id).FRO_all;
        message += "<br><font color=\"red\">FRO MIN : " + std::to_string(min) + "</font>";
        message += "<br><font color=\"blue\">FRO MAX : "  + std::to_string(max) + "</font>";
        message += "<br>FRO AVG : " + std::to_string(metrics->at(curr_mesh_id).FRO_global_avg);

        if (metrics->at(curr_mesh_id).FRO_min < metrics->at(curr_mesh_id).FRO_poly_min)
            min_id = metrics->at(curr_mesh_id).FRO_min_id;
        else
            min_id = metrics->at(curr_mesh_id).FRO_poly_min_id;

        if (metrics->at(curr_mesh_id).FRO_max > metrics->at(curr_mesh_id).FRO_poly_max)
            max_id = metrics->at(curr_mesh_id).FRO_max_id;
        else
            max_id = metrics->at(curr_mesh_id).FRO_poly_max_id;

    }
    else
    {
        return;
    }

    set_all_color(min, max, all);


    if (ui->highlight_min_max_cb->isChecked())
    {
        highlight_min_max_elements(min_id, max_id);
    }

    ui->min_label->setText(QString(std::to_string(min).c_str()));
    ui->max_label->setText(QString(std::to_string(max).c_str()));

    ui->mesh_metrics_canvas->clear();
    ui->mesh_metrics_canvas->add_mesh(*mesh_with_metrics.at(0), true);

    ui->info_text->setHtml(message.c_str());
}

void MeshMetricsGraphicWidget::set_slider_max(const uint max)
{
    ui->mesh_metrics_slider->setMaximum(static_cast<int>(max));
}

void MeshMetricsGraphicWidget::set_slider_value(const uint val)
{
    ui->mesh_metrics_slider->setValue(static_cast<int>(val));
}


void MeshMetricsGraphicWidget::on_mesh_metrics_slider_valueChanged(int value)
{
    clean_canvas();

    show_mesh(value);

    ui->mesh_number_label->setText(std::to_string(value).c_str());
}

void MeshMetricsGraphicWidget::highlight_min_max_elements (const uint min, const uint max)
{
    std::vector<uint> ids {min, max};
    highlight_element(ids);
}

void MeshMetricsGraphicWidget::highlight_element (const std::vector<uint> ids)
{
    std::vector<cinolib::Color> colors;

    for (uint id : ids)
        colors.push_back(mesh_with_metrics.at(0)->poly_data(id).color);

    #pragma omp parallel for
    for (int pid=0; pid < mesh_with_metrics.at(0)->num_polys(); pid++)
    {
        mesh_with_metrics.at(0)->poly_data(pid).color = cinolib::Color::WHITE();
    }

    for (int pid=0; pid < ids.size(); pid++)
        mesh_with_metrics.at(0)->poly_data(ids.at(pid)).color = colors.at(pid);

}

void MeshMetricsGraphicWidget::on_highlight_min_max_cb_checkStateChanged(const Qt::CheckState &checked)
{
    mesh_with_metrics.at(0)->poly_set_color(cinolib::Color::WHITE());

    double min, max;
    uint min_id, max_id;
    std::vector<double> all;

    std::string message = "";

    if (ui->inr_rb->isChecked())
    {
        min = std::min(metrics->at(curr_mesh_id).INR_min, metrics->at(curr_mesh_id).INR_poly_min);
        max = std::max(metrics->at(curr_mesh_id).INR_max, metrics->at(curr_mesh_id).INR_poly_max);

        if (metrics->at(curr_mesh_id).INR_min < metrics->at(curr_mesh_id).INR_poly_min)
            min_id = metrics->at(curr_mesh_id).INR_min_id;
        else
            min_id = metrics->at(curr_mesh_id).INR_poly_min_id;

        if (metrics->at(curr_mesh_id).INR_max > metrics->at(curr_mesh_id).INR_poly_max)
            max_id = metrics->at(curr_mesh_id).INR_max_id;
        else
            max_id = metrics->at(curr_mesh_id).INR_poly_max_id;

        all = metrics->at(curr_mesh_id).INR_all;
        message += "<br><font color=\"red\">INR MIN : " + std::to_string(min) + "</font>";
        message += "<br><font color=\"blue\">INR MAX : "  + std::to_string(max) + "</font>";
        message += "<br>INR AVG : " + std::to_string(metrics->at(curr_mesh_id).INR_global_avg);
    }
    else
    if (ui->our_rb->isChecked())
    {
        min = std::min(metrics->at(curr_mesh_id).OUR_min, metrics->at(curr_mesh_id).OUR_poly_min);
        max = std::max(metrics->at(curr_mesh_id).OUR_max, metrics->at(curr_mesh_id).OUR_poly_max);

        if (metrics->at(curr_mesh_id).OUR_min < metrics->at(curr_mesh_id).OUR_poly_min)
            min_id = metrics->at(curr_mesh_id).OUR_min_id;
        else
            min_id = metrics->at(curr_mesh_id).OUR_poly_min_id;

        if (metrics->at(curr_mesh_id).OUR_max > metrics->at(curr_mesh_id).OUR_poly_max)
            max_id = metrics->at(curr_mesh_id).OUR_max_id;
        else
            max_id = metrics->at(curr_mesh_id).OUR_poly_max_id;

        all = metrics->at(curr_mesh_id).OUR_all;
        message += "<br><font color=\"red\">OUR MIN : " + std::to_string(min) + "</font>";
        message += "<br><font color=\"blue\">OUR MAX : "  + std::to_string(max) + "</font>";
        message += "<br>OUR AVG : " + std::to_string(metrics->at(curr_mesh_id).OUR_global_avg);
    }
    else
    if (ui->cir_rb->isChecked())
    {
        min = std::min(metrics->at(curr_mesh_id).CIR_min, metrics->at(curr_mesh_id).CIR_poly_min);
        max = std::max(metrics->at(curr_mesh_id).CIR_max, metrics->at(curr_mesh_id).CIR_poly_max);

        if (metrics->at(curr_mesh_id).INR_min < metrics->at(curr_mesh_id).INR_poly_min)
            min_id = metrics->at(curr_mesh_id).INR_min_id;
        else
            min_id = metrics->at(curr_mesh_id).INR_poly_min_id;

        if (metrics->at(curr_mesh_id).INR_max > metrics->at(curr_mesh_id).INR_poly_max)
            max_id = metrics->at(curr_mesh_id).INR_max_id;
        else
            max_id = metrics->at(curr_mesh_id).INR_poly_max_id;

        all = metrics->at(curr_mesh_id).CIR_all;
        message += "<br><font color=\"red\">CIR MIN : " + std::to_string(min) + "</font>";
        message += "<br><font color=\"blue\">CIR MAX : "  + std::to_string(max) + "</font>";
        message += "<br>CIR AVG : " + std::to_string(metrics->at(curr_mesh_id).CIR_global_avg);
    }
    else
    if (ui->krr_rb->isChecked())
    {
        min = std::min(metrics->at(curr_mesh_id).KRR_min, metrics->at(curr_mesh_id).KRR_poly_min);
        max = std::max(metrics->at(curr_mesh_id).KRR_max, metrics->at(curr_mesh_id).KRR_poly_max);

        if (metrics->at(curr_mesh_id).KRR_min < metrics->at(curr_mesh_id).KRR_poly_min)
            min_id = metrics->at(curr_mesh_id).KRR_min_id;
        else
            min_id = metrics->at(curr_mesh_id).KRR_poly_min_id;

        if (metrics->at(curr_mesh_id).KRR_max > metrics->at(curr_mesh_id).KRR_poly_max)
            max_id = metrics->at(curr_mesh_id).KRR_max_id;
        else
            max_id = metrics->at(curr_mesh_id).KRR_poly_max_id;

        all = metrics->at(curr_mesh_id).KRR_all;
        message += "<br><font color=\"red\">KRR MIN : " + std::to_string(min) + "</font>";
        message += "<br><font color=\"blue\">KRR MAX : "  + std::to_string(max) + "</font>";
        message += "<br>KRR AVG : " + std::to_string(metrics->at(curr_mesh_id).KRR_global_avg);
    }
    else
    if (ui->kar_rb->isChecked())
    {
        min = std::min(metrics->at(curr_mesh_id).KAR_min, metrics->at(curr_mesh_id).KAR_poly_min);
        max = std::max(metrics->at(curr_mesh_id).KAR_max, metrics->at(curr_mesh_id).KAR_poly_max);

        if (metrics->at(curr_mesh_id).KAR_min < metrics->at(curr_mesh_id).KAR_poly_min)
            min_id = metrics->at(curr_mesh_id).KAR_min_id;
        else
            min_id = metrics->at(curr_mesh_id).KAR_poly_min_id;

        if (metrics->at(curr_mesh_id).KAR_max > metrics->at(curr_mesh_id).KAR_poly_max)
            max_id = metrics->at(curr_mesh_id).KAR_max_id;
        else
            max_id = metrics->at(curr_mesh_id).KAR_poly_max_id;

        all = metrics->at(curr_mesh_id).KAR_all;
        message += "<br><font color=\"red\">KAR MIN : " + std::to_string(min) + "</font>";
        message += "<br><font color=\"blue\">KAR MAX : "  + std::to_string(max) + "</font>";
        message += "<br>KAR AVG : " + std::to_string(metrics->at(curr_mesh_id).KAR_global_avg);
    }
    else
    if (ui->apr_rb->isChecked())
    {
        min = std::min(metrics->at(curr_mesh_id).APR_min, metrics->at(curr_mesh_id).APR_poly_min);
        max = std::max(metrics->at(curr_mesh_id).APR_max, metrics->at(curr_mesh_id).APR_poly_max);

        if (metrics->at(curr_mesh_id).APR_min < metrics->at(curr_mesh_id).APR_poly_min)
            min_id = metrics->at(curr_mesh_id).APR_min_id;
        else
            min_id = metrics->at(curr_mesh_id).APR_poly_min_id;

        if (metrics->at(curr_mesh_id).APR_max > metrics->at(curr_mesh_id).APR_poly_max)
            max_id = metrics->at(curr_mesh_id).APR_max_id;
        else
            max_id = metrics->at(curr_mesh_id).APR_poly_max_id;

        all = metrics->at(curr_mesh_id).APR_all;
        message += "<br><font color=\"red\">APR MIN : " + std::to_string(min) + "</font>";
        message += "<br><font color=\"blue\">APR MAX : "  + std::to_string(max) + "</font>";
        message += "<br>APR AVG : " + std::to_string(metrics->at(curr_mesh_id).APR_global_avg);
    }
    else
    if (ui->mia_rb->isChecked())
    {
        min = std::min(metrics->at(curr_mesh_id).MIA_min, metrics->at(curr_mesh_id).MIA_poly_min);
        max = std::max(metrics->at(curr_mesh_id).MIA_max, metrics->at(curr_mesh_id).MIA_poly_max);

        if (metrics->at(curr_mesh_id).MIA_min < metrics->at(curr_mesh_id).MIA_poly_min)
            min_id = metrics->at(curr_mesh_id).MIA_min_id;
        else
            min_id = metrics->at(curr_mesh_id).MIA_poly_min_id;

        if (metrics->at(curr_mesh_id).MIA_max > metrics->at(curr_mesh_id).MIA_poly_max)
            max_id = metrics->at(curr_mesh_id).MIA_max_id;
        else
            max_id = metrics->at(curr_mesh_id).MIA_poly_max_id;

        all = metrics->at(curr_mesh_id).MIA_all;
        message += "<br><font color=\"red\">MIA MIN : " + std::to_string(min) + "</font>";
        message += "<br><font color=\"blue\">MIA MAX : "  + std::to_string(max) + "</font>";
        message += "<br>MIA AVG : " + std::to_string(metrics->at(curr_mesh_id).MIA_global_avg);
    }
    else
    if (ui->maa_rb->isChecked())
    {
        min = std::min(metrics->at(curr_mesh_id).MAA_min, metrics->at(curr_mesh_id).MAA_poly_min);
        max = std::max(metrics->at(curr_mesh_id).MAA_max, metrics->at(curr_mesh_id).MAA_poly_max);

        if (metrics->at(curr_mesh_id).MAA_min < metrics->at(curr_mesh_id).MAA_poly_min)
            min_id = metrics->at(curr_mesh_id).MAA_min_id;
        else
            min_id = metrics->at(curr_mesh_id).MAA_poly_min_id;

        if (metrics->at(curr_mesh_id).MAA_max > metrics->at(curr_mesh_id).MAA_poly_max)
            max_id = metrics->at(curr_mesh_id).MAA_max_id;
        else
            max_id = metrics->at(curr_mesh_id).MAA_poly_max_id;

        all = metrics->at(curr_mesh_id).MAA_all;
        message += "<br><font color=\"red\">MAA MIN : " + std::to_string(min) + "</font>";
        message += "<br><font color=\"blue\">MAA MAX : "  + std::to_string(max) + "</font>";
        message += "<br>MAA AVG : " + std::to_string(metrics->at(curr_mesh_id).MAA_global_avg);
    }
    else
    if (ui->anr_rb->isChecked())
    {
        min = std::min(metrics->at(curr_mesh_id).ANR_min, metrics->at(curr_mesh_id).ANR_poly_min);
        max = std::max(metrics->at(curr_mesh_id).ANR_max, metrics->at(curr_mesh_id).ANR_poly_max);

        if (metrics->at(curr_mesh_id).ANR_min < metrics->at(curr_mesh_id).ANR_poly_min)
            min_id = metrics->at(curr_mesh_id).ANR_min_id;
        else
            min_id = metrics->at(curr_mesh_id).ANR_poly_min_id;

        if (metrics->at(curr_mesh_id).ANR_max > metrics->at(curr_mesh_id).ANR_poly_max)
            max_id = metrics->at(curr_mesh_id).ANR_max_id;
        else
            max_id = metrics->at(curr_mesh_id).ANR_poly_max_id;

        all = metrics->at(curr_mesh_id).ANR_all;
        message += "<br><font color=\"red\">ANR MIN : " + std::to_string(min) + "</font>";
        message += "<br><font color=\"blue\">ANR MAX : "  + std::to_string(max) + "</font>";
        message += "<br>ANR AVG : " + std::to_string(metrics->at(curr_mesh_id).ANR_global_avg);
    }
    else
    if (ui->vem_rb->isChecked())
    {
        min = std::min(metrics->at(curr_mesh_id).VEM_min, metrics->at(curr_mesh_id).VEM_poly_min);
        max = std::max(metrics->at(curr_mesh_id).VEM_max, metrics->at(curr_mesh_id).VEM_poly_max);

        if (metrics->at(curr_mesh_id).VEM_min < metrics->at(curr_mesh_id).VEM_poly_min)
            min_id = metrics->at(curr_mesh_id).VEM_min_id;
        else
            min_id = metrics->at(curr_mesh_id).VEM_poly_min_id;

        if (metrics->at(curr_mesh_id).VEM_max > metrics->at(curr_mesh_id).VEM_poly_max)
            max_id = metrics->at(curr_mesh_id).VEM_max_id;
        else
            max_id = metrics->at(curr_mesh_id).VEM_poly_max_id;

        all = metrics->at(curr_mesh_id).VEM_all;
        message += "<br><font color=\"red\">VEM MIN : " + std::to_string(min) + "</font>";
        message += "<br><font color=\"blue\">VEM MAX : "  + std::to_string(max) + "</font>";
        message += "<br>VEM AVG : " + std::to_string(metrics->at(curr_mesh_id).VEM_global_avg);
    }
    else
    if (ui->jac_rb->isChecked())
    {
        min = std::min(metrics->at(curr_mesh_id).JAC_min, metrics->at(curr_mesh_id).JAC_poly_min);
        max = std::max(metrics->at(curr_mesh_id).JAC_max, metrics->at(curr_mesh_id).JAC_poly_max);

        if (metrics->at(curr_mesh_id).JAC_min < metrics->at(curr_mesh_id).JAC_poly_min)
            min_id = metrics->at(curr_mesh_id).JAC_min_id;
        else
            min_id = metrics->at(curr_mesh_id).JAC_poly_min_id;

        if (metrics->at(curr_mesh_id).JAC_max > metrics->at(curr_mesh_id).JAC_poly_max)
            max_id = metrics->at(curr_mesh_id).JAC_max_id;
        else
            max_id = metrics->at(curr_mesh_id).JAC_poly_max_id;

        all = metrics->at(curr_mesh_id).JAC_all;
        message += "<br><font color=\"red\">JAC MIN : " + std::to_string(min) + "</font>";
        message += "<br><font color=\"blue\">JAC MAX : "  + std::to_string(max) + "</font>";
        message += "<br>JAC AVG : " + std::to_string(metrics->at(curr_mesh_id).JAC_global_avg);
    }
    else
    if (ui->fro_rb->isChecked())
    {
        min = std::min(metrics->at(curr_mesh_id).FRO_min, metrics->at(curr_mesh_id).FRO_poly_min);
        max = std::max(metrics->at(curr_mesh_id).FRO_max, metrics->at(curr_mesh_id).FRO_poly_max);

        if (metrics->at(curr_mesh_id).FRO_min < metrics->at(curr_mesh_id).FRO_poly_min)
            min_id = metrics->at(curr_mesh_id).FRO_min_id;
        else
            min_id = metrics->at(curr_mesh_id).FRO_poly_min_id;

        if (metrics->at(curr_mesh_id).FRO_max > metrics->at(curr_mesh_id).FRO_poly_max)
            max_id = metrics->at(curr_mesh_id).FRO_max_id;
        else
            max_id = metrics->at(curr_mesh_id).FRO_poly_max_id;

        all = metrics->at(curr_mesh_id).FRO_all;
        message += "<br><font color=\"red\">FRO MIN : " + std::to_string(min) + "</font>";
        message += "<br><font color=\"blue\">FRO MAX : "  + std::to_string(max) + "</font>";
        message += "<br>FRO AVG : " + std::to_string(metrics->at(curr_mesh_id).FRO_global_avg);
    }
    else
    {
        return;
    }

    set_all_color(min, max, all);

    if (checked == Qt::Checked)
    {
        highlight_min_max_elements(min_id, max_id);
    }

    ui->mesh_metrics_canvas->clear();
    ui->mesh_metrics_canvas->add_mesh(*mesh_with_metrics.at(0), true);

    ui->info_text->setHtml(message.c_str());
}


void MeshMetricsGraphicWidget::on_indicator_info_btn_clicked()
{
    std::string tmp_file = QApplication::applicationDirPath().append("/indicators.pdf").toStdString();

    QFile HelpFile(":/docs/docs/quality_indicators.pdf");
    HelpFile.copy(tmp_file.c_str());

    QDesktopServices::openUrl(QUrl::fromLocalFile(tmp_file.c_str()));

}

