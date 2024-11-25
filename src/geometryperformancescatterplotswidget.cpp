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

#include "geometryperformancescatterplotswidget.h"
#include "ui_geometryperformancescatterplotswidget.h"

#include "quality_metrics.h"

#include <QFileDialog>

#include <QtCharts/QLogValueAxis>

#include <iostream>

GeometryPerformanceScatterPlotsWidget::GeometryPerformanceScatterPlotsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GeometryPerformanceScatterplotsWidget)
{
    ui->setupUi(this);
    ui->compute_btn->hide();

    for (uint i=0; i < metrics_names.size(); i++)
        if (!metrics_scale_dependent.at(i))
        {
            ui->x_axis_cb->addItem(metrics_names.at(i).c_str());

            cbID2metricsID.push_back(i);
        }

    connect(ui->x_axis_cb, SIGNAL (currentIndexChanged(int)), this, SLOT (show_scatterplot_x(int)));

    ui->y_axis_cb->addItem("H1 Error");
    ui->y_axis_cb->addItem("Inf Error");
    ui->y_axis_cb->addItem("L2 Error");
    ui->y_axis_cb->addItem("Condition Number");

    connect(ui->y_axis_cb, SIGNAL (currentIndexChanged(int)), this, SLOT (show_scatterplot_y(int)));


    for(int i = ui->stackedWidget->count(); i >= 0; i--)
        ui->stackedWidget->removeWidget(ui->stackedWidget->widget(i));

    extra_colors.push_back(QColor(255, 192, 203));
    extra_colors.push_back(QColor(211, 211, 211));
    extra_colors.push_back(QColor(0, 0, 139));
    extra_colors.push_back(QColor(34,139,34));

    ui->marker_size_label->hide();
    ui->marker_size_dsb->hide();
    ui->line->hide();
}

GeometryPerformanceScatterPlotsWidget::~GeometryPerformanceScatterPlotsWidget()
{
    delete ui;
}

void GeometryPerformanceScatterPlotsWidget::create_scatterPlots(const Dataset d, const std::vector<MeshMetrics> &metrics,
                                                                const std::vector<std::vector<double> > &performances)
{
    ui->compute_btn->hide();

    ui->groupBox->show();
    ui->groupBox_2->show();
    ui->groupBox_3->show();

    if (ui->stackedWidget->count() > 0) {
        for(int i = ui->stackedWidget->count(); i >= 0; i--)
        {
            QWidget* widget = ui->stackedWidget->widget(i);
            ui->stackedWidget->removeWidget(widget);
            widget->deleteLater();
        }
    }

    for (QChartView *chv : chart_views)
        chv->deleteLater();

    chart_views.clear();

    //std::string first_class = d.class_names.at(0);

    std::vector<uint> class_chages;
    class_chages.push_back(0);

    for (uint i=1; i < d.get_num_parametric_meshes(); i++)
        if (d.get_parametric_mesh_class_id(i) != d.get_parametric_mesh_class_id(i-1))
            class_chages.push_back(i);

    class_chages.push_back(d.get_num_parametric_meshes());

    for (int i=0; i < ui->x_axis_cb->count(); i++)
    {
        for (int j=0; j < ui->y_axis_cb->count(); j++)
        {
            QChart *ch = new QChart();

            double maxX = -DBL_MAX, maxY = -DBL_MAX;
            double minX =  DBL_MAX, minY =  DBL_MAX;

            for (uint cc=0; cc < class_chages.size()-1; cc++)
            {
                std::string class_name = "CLASS";

                if (d.get_num_parametric_meshes() > 0)
                    class_name = d.get_parametric_mesh_class_name(d.get_parametric_mesh_class_id(class_chages.at(cc))).c_str();

                QScatterSeries *s = new QScatterSeries();
                // s->setName(class_name.c_str());

                if (cc > 4)
                    s->setColor(extra_colors.at(cc-5));

                for (uint m=class_chages.at(cc); m < class_chages.at(cc+1); m++)
                {
                    double x;
                    switch (cbID2metricsID.at(i)) {
                        case 0: x =  metrics.at(m).INR_mesh; break;
                        case 1: x =  metrics.at(m).OUR_mesh; break;
                        case 2: x =  metrics.at(m).CIR_mesh; break;
                        case 3: x =  metrics.at(m).KRR_mesh; break;
                        case 4: x =  metrics.at(m).KAR_mesh; break;
                        case 5: x =  metrics.at(m).APR_mesh; break;
                        case 6: x =  metrics.at(m).MIA_mesh; break;
                        case 7: x =  metrics.at(m).MAA_mesh; break;
                        case 8: x =  metrics.at(m).ANR_mesh; break;
                        case 9: x =  metrics.at(m).VEM_mesh; break;
                        case 10: x = metrics.at(m).JAC_mesh; break;
                        case 11: x = metrics.at(m).FRO_mesh; break;
                    default: x = DBL_MAX;
                    }

                    double y = performances.at(j).at(m);
                    // y = log(y);

                    s->append(x,y);

                    minX = std::min(minX, x);
                    maxX = std::max(maxX, x);
                    minY = std::min(minY, y);
                    maxY = std::max(maxY, y);
                }
                ch->addSeries(s);
            }

            ch->legend()->hide();
            ch->createDefaultAxes();

            // ch->axes()[0]->setMax(maxX*1.001);
            // ch->axes()[0]->setMin(minX*0.999);
            // ch->axes()[1]->setMax(maxY*1.001);
            // ch->axes()[1]->setMin(minY*0.999);
            // ch->axes()[1]->setMax(*std::max_element(performances.at(j).begin(), performances.at(j).end()));

            ch->axes()[0]->setTitleText(metrics_names.at(cbID2metricsID.at(i)).c_str());
            ch->axes()[1]->setTitleText(ui->y_axis_cb->itemText(j).toStdString().c_str());
            ch->setTitle((metrics_names.at(cbID2metricsID.at(i)) + " vs " + ui->y_axis_cb->itemText(j).toStdString()).c_str());

            std::cout << chart_views.size() << " : " << metrics_names.at(cbID2metricsID.at(i)) <<
                                               " - " << ui->y_axis_cb->itemText(j).toStdString() << std::endl;

//             if (ui->y_axis_cb->itemText(j).toStdString().compare("condVect") == 0)
//             {
//                 QLogValueAxis *axisYlog = new QLogValueAxis();
// //                axisYlog->setBase(8.0);
// //                axisYlog->setMinorTickCount(-1);
//                 axisYlog->setLabelFormat("%e");
//                 axisYlog->setTitleText(ui->y_axis_cb->itemText(j).toStdString().c_str());
//                 axisYlog->setMax(*std::max_element(performances.at(j).begin(), performances.at(j).end()) * 10);

//                 std::cout << "MAX CONDVECT : " << axisYlog->max() << std::endl;

//                 ch->removeAxis(ch->axes().at(1));
//                 ch->addAxis(axisYlog, Qt::AlignLeft);

//                 for (QAbstractSeries *s : ch->series())
//                     s->attachAxis(axisYlog);
//             }


            CustomizedChartView *ch_view = new CustomizedChartView();
            ch_view->setChart(ch);

//            connect(ch_view, SIGNAL(clicked(double, double, double, double)),
//                    this, SLOT (show_tooltip (double, double, double, double)));

            chart_views.push_back(ch_view);
            ui->stackedWidget->addWidget(ch_view);

        }
    }

    ui->stackedWidget->setCurrentIndex(0);

    QScatterSeries * s = static_cast<QScatterSeries*>(chart_views.at(0)->chart()->series()[0]);
    ui->marker_size_dsb->setValue(s->markerSize());

    for (uint cc=0; cc < class_chages.size()-1; cc++)
    {
        std::string class_name = "Mesh";

        if (d.get_num_parametric_meshes() > 0)
            class_name = d.get_parametric_mesh_class_name(d.get_parametric_mesh_class_id(class_chages.at(cc))).c_str();

        ScatterPlotMarkerSettingWidget * msw;
        if (update_settings_bar)
            msw = new ScatterPlotMarkerSettingWidget();
        else
            msw = marker_settings_widgets.at(cc);

        msw->update_series_name(class_name.c_str());
        msw->set_checked(true);

        QScatterSeries * s = static_cast<QScatterSeries*>(chart_views.at(0)->chart()->series()[cc]);
        ui->marker_size_dsb->setValue(s->markerSize());
        msw->set_marker_size(s->markerSize());
        msw->set_id(cc);
        msw->set_color(s->color());

        connect(msw, SIGNAL(size_changed(uint, double)), this, SLOT (update_marker_size (uint, double)));
        connect(msw, SIGNAL(checked_changed(uint, int)), this, SLOT (update_show_series (uint, int)));
        connect(msw, SIGNAL(color_changed(uint, QColor)), this, SLOT (update_color_series (uint, QColor)));

        if (ui->marker_settings_widget->layout() == nullptr)
            ui->marker_settings_widget->setLayout( new QVBoxLayout() );

        if (update_settings_bar) {
            ui->marker_settings_widget->layout()->addWidget(msw);
            marker_settings_widgets.push_back(msw);
        }
    }

    connect(ui->marker_size_dsb, SIGNAL(valueChanged(double)), this, SLOT (update_marker_size (double)));

    update_settings_bar = false;
}

void GeometryPerformanceScatterPlotsWidget::show_scatterplot_x(int xID)
{
    int yID = ui->y_axis_cb->currentIndex();

    std::cout << xID << " - " << yID << std::endl;

    ui->stackedWidget->setCurrentIndex(xID * ui->y_axis_cb->count()+ yID);
}

void GeometryPerformanceScatterPlotsWidget::show_scatterplot_y(int yID)
{
    int xID = ui->x_axis_cb->currentIndex();

    std::cout << xID << " - " << yID << std::endl;

    ui->stackedWidget->setCurrentIndex(xID * ui->y_axis_cb->count()+ yID);
}

void GeometryPerformanceScatterPlotsWidget::update_marker_size(double size)
{
    for (CustomizedChartView *ch_view : chart_views)
    {
        for (QAbstractSeries *as : ch_view->chart()->series())
        {
            QScatterSeries * s = static_cast<QScatterSeries*>(as);
            s->setMarkerSize(size);
        }
    }
}

void GeometryPerformanceScatterPlotsWidget::update_marker_size(uint id, double size)
{
    for (CustomizedChartView *ch_view : chart_views)
    {
        QScatterSeries * s = static_cast<QScatterSeries*>(ch_view->chart()->series()[id]);
        s->setMarkerSize(size);
    }
}

void GeometryPerformanceScatterPlotsWidget::update_show_series(uint id, int b)
{
    for (CustomizedChartView *ch_view : chart_views)
    {
        QScatterSeries * s = static_cast<QScatterSeries*>(ch_view->chart()->series()[id]);
        s->setVisible(b);
    }
}

void GeometryPerformanceScatterPlotsWidget::update_color_series(uint id, QColor c)
{
    for (CustomizedChartView *ch_view : chart_views)
    {
        QScatterSeries * s = static_cast<QScatterSeries*>(ch_view->chart()->series()[id]);
        s->setColor(c);
    }
}

void GeometryPerformanceScatterPlotsWidget::on_pushButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
                                                        "/tmp",
                                                        QFileDialog::ShowDirsOnly
                                                        | QFileDialog::DontResolveSymlinks);
    if (dir.isNull())
        return;

    for (QChartView *chview : chart_views)
    {
        std::string x_axis = chview->chart()->axisX()->titleText().toStdString();
        std::string y_axis = chview->chart()->axisY()->titleText().toStdString();

        std::replace( x_axis.begin(), x_axis.end(), ' ', '_'); // replace all 'x' to 'y'
        std::replace( y_axis.begin(), y_axis.end(), ' ', '_'); // replace all 'x' to 'y'

        std::string filename = dir.toStdString() + "/" + x_axis + "__..__" +
                                                         y_axis + "__" +
                                                         std::to_string(solution_id) + ".png";

        std::cout << "Saving "  << filename << std::endl;

        QPixmap p = chview->grab();
        p.save(filename.c_str());
    }

}

void GeometryPerformanceScatterPlotsWidget::set_empty()
{
    ui->compute_btn->show();

    ui->groupBox->hide();
    ui->groupBox_2->hide();
    ui->groupBox_3->hide();
}

void GeometryPerformanceScatterPlotsWidget::on_compute_btn_clicked()
{
    emit compute_GP_scatterplots();
}

void GeometryPerformanceScatterPlotsWidget::on_y_axis_cb_currentIndexChanged(int index)
{

}

