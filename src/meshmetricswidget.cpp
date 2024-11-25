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

#include "meshmetricswidget.h"
#include "ui_meshmetricswidget.h"

#include "meshes/mesh_metrics.h"

#include "quality_metrics.h"
#include "sortgeometricqualitiesdialog.h"

#include <QColorDialog>
#include <QFileDialog>

#include <QtCharts/QLineSeries>

#include <iomanip>
#include <iostream>
#include <sstream>

MeshMetricsWidget::MeshMetricsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MeshMetricsWidget)
{
    ui->setupUi(this);

    QSizePolicy spRight(QSizePolicy::Preferred, QSizePolicy::Preferred);
    spRight.setHorizontalStretch(3);
    ui->charts->setSizePolicy(spRight);

    ui->scale_dependent_cb->hide();
    ui->min_cb->hide();
    ui->min_color_btn->hide();
    ui->max_cb->hide();
    ui->max_color_btn->hide();
    ui->poly_min_cb->hide();
    ui->poly_min_color_btn->hide();
    ui->poly_max_cb->hide();
    ui->poly_max_color_btn->hide();
}

MeshMetricsWidget::~MeshMetricsWidget()
{
    delete ui;
}

void MeshMetricsWidget::reset()
{
    last_row = 0;
    last_col = 0;

    for (CustomizedChartView *c : chart_views)
        delete c;

    chart_views.clear();
}

void MeshMetricsWidget::add_chart(CustomizedChartView *chart)
{
    chart->set_double_click_enabled(true);

    chart_views.push_back(chart);

    QGridLayout *l = ui->chart_grid_layout;

    std::cout << "Adding chart " << last_row << " -- " << last_col << std::endl;

    l->addWidget(chart, static_cast<int>(last_row), static_cast<int>(last_col));

    l->setRowStretch(static_cast<int>(last_row), 1);
    l->setColumnStretch(static_cast<int>(last_col),1);

    if (last_col == n_cols-1)
    {
        last_col = 0;
        last_row++;
    }
    else
    {
        last_col++;
    }
    update_cb_stateChanged();
}


void MeshMetricsWidget::on_sort_btn_clicked()
{
    SortGeometricQualitiesDialog *dialog = new SortGeometricQualitiesDialog();

    if (dialog->exec() == 1)
    {
        uint selected_id = static_cast<uint>(dialog->get_selected());

        emit (sort_geometric_qualities(selected_id));

        ui->original_btn->setEnabled(true);
    }

    ui->show_legend_cb->setChecked(false);
    ui->show_ranges_cb->setChecked(false);
    ui->scale_dependent_cb->setChecked(false);

    delete dialog;
}

void MeshMetricsWidget::on_original_btn_clicked()
{
    ui->original_btn->setEnabled(false);

    ui->show_legend_cb->setChecked(false);
    ui->show_ranges_cb->setChecked(true);
    ui->scale_dependent_cb->setChecked(false);

    emit (show_unsorted_metrics());
}

void MeshMetricsWidget::on_show_legend_cb_stateChanged(int checked)
{
    if (checked)
    {
        for (CustomizedChartView *chart : chart_views)
            chart->chart()->legend()->show();
    }
    else
    {
        for (CustomizedChartView *chart : chart_views)
            chart->chart()->legend()->hide();
    }
}

void MeshMetricsWidget::on_show_ranges_cb_stateChanged(int checked)
{
    uint id = 0;

    if (checked)
    {
        for (CustomizedChartView *chart : chart_views)
        {
            double min = get_metrics_min(id);
            double max = get_metrics_max(id);

            std::string max_str = std::to_string(max);
            if (max == cinolib::max_double) max_str = "+inf";
            else if (max == 0)  max_str = "0";
            else if (max == 1)  max_str = "1";

            std::string min_str = std::to_string(min);
            if (min == 0)       min_str = "0";
            else if (min == 1)  min_str = "1";

            std::string title = metrics_names.at(id) +
                                " ["+min_str+", "+max_str+"]";

            chart->chart()->setTitle(title.c_str());

            id++;
        }
    }
    else
    {
        for (CustomizedChartView *chart : chart_views)
            chart->chart()->setTitle(metrics_names.at(id++).c_str());
    }
}

void MeshMetricsWidget::change_series_color(const int series_id)
{
    QLineSeries *series = static_cast<QLineSeries *> (chart_views.at(0)->chart()->series().at(series_id));
    QPen pen = series->pen();
    QColor prev_color = pen.brush().color();

    QColorDialog *colorDialog = new QColorDialog(this);
    QColor color = colorDialog->getColor(prev_color, this, "Effect Color", QColorDialog::DontUseNativeDialog);

    if (color == nullptr) return;

    for (CustomizedChartView *chart : chart_views)
    {
        QLineSeries *series = static_cast<QLineSeries *> (chart->chart()->series().at(series_id));

        QPen pen = series->pen();
        pen.setWidth(1);
        pen.setBrush(QBrush(color)); // or just pen.setColor("red");
        series->setPen(pen);
    }
}

void MeshMetricsWidget::show_series(const uint series_id, const bool checked)
{
    for (CustomizedChartView *chart : chart_views)
    {
        QLineSeries *series = static_cast<QLineSeries *> (chart->chart()->series().at(static_cast<int>(series_id)));

        if (checked) series->show();
        else series->hide();
    }
}


void MeshMetricsWidget::on_min_color_btn_clicked()
{
   change_series_color(0);
}

void MeshMetricsWidget::on_max_color_btn_clicked()
{
    change_series_color(1);
}

void MeshMetricsWidget::on_avg_color_btn_clicked()
{
    change_series_color(2);
}

void MeshMetricsWidget::on_poly_min_color_btn_clicked()
{
    change_series_color(3);
}

void MeshMetricsWidget::on_poly_max_color_btn_clicked()
{
    change_series_color(4);
}

void MeshMetricsWidget::on_poly_avg_color_btn_clicked()
{
    change_series_color(5);
}

void MeshMetricsWidget::on_mesh_color_btn_clicked()
{
    change_series_color(6);
}

void MeshMetricsWidget::on_save_txt_btn_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
                                                 "/tmp",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);

    if (dir.isNull()) return;

    for (uint i=0; i < dataset->get_parametric_meshes().size(); i++)
    {
        std::string filename ;

        std::cout << dataset->get_parametric_mesh(i)->mesh_data().filename << std::endl;

        if (dataset->is_on_disk())
        {
            std::string fn = dataset->get_parametric_mesh(i)->mesh_data().filename;
            std::string fname = fn.substr(fn.find_last_of(QDir::separator().toLatin1()) +1);

            std::cout << fname << std::endl;

            std::string filename_ne = fname.substr(0, fname.find_last_of("."));

            std::cout << filename_ne << std::endl;

            filename = dir.toStdString() + QString(QDir::separator()).toStdString() +
                               filename_ne + ".txt";
        }
        else
        {           
            std::stringstream ss;
            ss << std::setw((dataset->get_parametric_meshes().size() / 10) + 1)
               << std::setfill('0') << i;
            std::string s = ss.str();

            std::string filename_prefix = ss.str() + "_";

            filename = dir.toStdString() + QString(QDir::separator()).toStdString() +
                               filename_prefix + ".txt";
        }

        std::cout << "Saving " << filename << std::endl;

        save_to_file(filename.c_str(), metrics->at(i));
    }
}

void MeshMetricsWidget::on_save_plots_btn_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
                                                 "/tmp",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);

    if (dir.isNull()) return;

    for (uint i=0; i < chart_views.size(); i++)
    {
        std::string filename = dir.toStdString() + QString(QDir::separator()).toStdString() +
                               metrics_names.at(i) + ".png";

        QPixmap p = chart_views.at(i)->grab();
        p.save(filename.c_str());
    }
}

void MeshMetricsWidget::on_scale_dependent_cb_stateChanged(int checked)
{
    if (checked)
    {
        for (uint i=0; i < chart_views.size(); i++)
        {
            if (metrics_scale_dependent.at(i))
                chart_views.at(i)->setBackgroundBrush(QColor (190, 190, 190));
        }
    }
    else
    {
        for (uint i=0; i < chart_views.size(); i++)
        {
            if (metrics_scale_dependent.at(i))
                chart_views.at(i)->setBackgroundBrush(QColor (230, 230, 230));
        }
    }
}

void MeshMetricsWidget::on_min_cb_stateChanged(int checked)
{
    show_series(0, checked);
}

void MeshMetricsWidget::on_max_cb_stateChanged(int checked)
{
    show_series(1, checked);
}

void MeshMetricsWidget::on_avg_cb_stateChanged(int checked)
{
    show_series(2, checked);
}

void MeshMetricsWidget::on_poly_min_cb_stateChanged(int checked)
{
    show_series(3, checked);
}

void MeshMetricsWidget::on_poly_max_cb_stateChanged(int checked)
{
    show_series(4, checked);
}

void MeshMetricsWidget::on_poly_avg_cb_stateChanged(int checked)
{
    show_series(5, checked);
}

void MeshMetricsWidget::on_mesh_cb_stateChanged(int checked)
{
    show_series(6, checked);
}

void MeshMetricsWidget::update_cb_stateChanged()
{
    on_min_cb_stateChanged(ui->min_cb->checkState());
    on_max_cb_stateChanged(ui->max_cb->checkState());
    on_avg_cb_stateChanged(ui->avg_cb->checkState());
    on_poly_min_cb_stateChanged(ui->poly_min_cb->checkState());
    on_poly_max_cb_stateChanged(ui->poly_max_cb->checkState());
    on_poly_avg_cb_stateChanged(ui->poly_avg_cb->checkState());
    on_mesh_cb_stateChanged(ui->mesh_cb->checkState());
}

void MeshMetricsWidget::on_all_btn_clicked()
{
    ui->min_cb->setCheckState(Qt::Checked);
    ui->max_cb->setCheckState(Qt::Checked);
    ui->avg_cb->setCheckState(Qt::Checked);
    ui->poly_min_cb->setCheckState(Qt::Checked);
    ui->poly_max_cb->setCheckState(Qt::Checked);
    ui->poly_avg_cb->setCheckState(Qt::Checked);
    ui->mesh_cb->setCheckState(Qt::Checked);
}

void MeshMetricsWidget::on_none_btn_clicked()
{
    ui->min_cb->setCheckState(Qt::Unchecked);
    ui->max_cb->setCheckState(Qt::Unchecked);
    ui->avg_cb->setCheckState(Qt::Unchecked);
    ui->poly_min_cb->setCheckState(Qt::Unchecked);
    ui->poly_max_cb->setCheckState(Qt::Unchecked);
    ui->poly_avg_cb->setCheckState(Qt::Unchecked);
    ui->mesh_cb->setCheckState(Qt::Unchecked);
}

