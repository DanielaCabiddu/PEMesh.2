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

#include "geometrygeometryscatterplotswidget.h"
#include "ui_geometrygeometryscatterplotswidget.h"

#include "quality_metrics.h"

#include <QFileDialog>
#include <QPixmap>
#include <QtCharts/QScatterSeries>
#include <QToolTip>
#include <QtCharts/QLogValueAxis>

GeometryGeometryScatterPlotsWidget::GeometryGeometryScatterPlotsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GeometryGeometryScatterPlotsWidget)
{
    ui->setupUi(this);

    for (uint i=0; i < metrics_names.size(); i++)
//        if (!metrics_scale_dependent.at(i))
        {
            ui->x_axis_cb->addItem(metrics_names.at(i).c_str());
            ui->y_axis_cb->addItem(metrics_names.at(i).c_str());

            cbID2metricsID.push_back(i);
        }

    connect(ui->x_axis_cb, SIGNAL (currentIndexChanged(int)), this, SLOT (show_scatterplot_x(int)));
    connect(ui->y_axis_cb, SIGNAL (currentIndexChanged(int)), this, SLOT (show_scatterplot_y(int)));

    for(int i = ui->stackedWidget->count(); i >= 0; i--)
        ui->stackedWidget->removeWidget(ui->stackedWidget->widget(i));

    ui->marker_settings_widget->setLayout(new QVBoxLayout());

    extra_colors.push_back(QColor(255, 192, 203));
    extra_colors.push_back(QColor(211, 211, 211));
    extra_colors.push_back(QColor(0, 0, 139));
    extra_colors.push_back(QColor(34,139,34));

}

GeometryGeometryScatterPlotsWidget::~GeometryGeometryScatterPlotsWidget()
{
    delete ui;
}

void GeometryGeometryScatterPlotsWidget::create_scatterPlots(const Dataset &d, const std::vector<MeshMetrics> &metrics)
{
    std::vector<uint> class_chages;
    class_chages.push_back(0);

    for (uint i=1; i < d.get_num_parametric_meshes(); i++)
        if (d.get_parametric_mesh_class_id(i) != d.get_parametric_mesh_class_id(i-1))
            class_chages.push_back(i);

    class_chages.push_back(d.get_num_parametric_meshes());

    for (int i=0; i < ui->x_axis_cb->count(); i++)
    {
        for (int j=0; j < ui->x_axis_cb->count(); j++)
        {
            QChart *ch = new QChart();

            double maxX = -DBL_MAX, maxY = -DBL_MAX;
            double minX =  DBL_MAX, minY = DBL_MAX;

            for (uint cc=0; cc < class_chages.size()-1; cc++)
            {
                std::string class_name = "Unknown";
                if (d.get_num_parametric_meshes() > 0)
                    class_name = d.get_parametric_mesh_class_name(d.get_parametric_mesh_class_id(class_chages.at(cc))).c_str();

                QScatterSeries *s = new QScatterSeries();
                s->setName(class_name.c_str());

                if (cc > 4)
                    s->setColor(extra_colors.at(cc-5));

                for (uint m=class_chages.at(cc); m < class_chages.at(cc+1); m++)
                {
                    double x, y;

                    double INR = metrics.at(m).INR_mesh;
                    double OUR = metrics.at(m).OUR_mesh;
                    double CIR = metrics.at(m).CIR_mesh;
                    double KRR = metrics.at(m).KRR_mesh;
                    double KAR = metrics.at(m).KAR_mesh;
                    double APR = metrics.at(m).APR_mesh;
                    double MIA = metrics.at(m).MIA_mesh;
                    double MAA = metrics.at(m).MAA_mesh;
                    double ANR = metrics.at(m).ANR_mesh;
                    double VEM = metrics.at(m).VEM_mesh;
                    double JAC = metrics.at(m).JAC_mesh;
                    double FRO = metrics.at(m).FRO_mesh;

                    // double min_INR = std::min(metrics.at(m).INR_min,
                    //                          metrics.at(m).INR_poly_min);

                    // if (metrics.at(m).INR_min_id == UINT_MAX) min_INR = metrics.at(m).INR_poly_min;
                    // else if (metrics.at(m).INR_poly_min_id == UINT_MAX) min_INR = metrics.at(m).INR_min;

                    // double min_OUR = std::min(metrics.at(m).OUR_min,
                    //                          metrics.at(m).OUR_poly_min);

                    // if (metrics.at(m).OUR_min_id == UINT_MAX) min_OUR = metrics.at(m).OUR_poly_min;
                    // else if (metrics.at(m).OUR_poly_min_id == UINT_MAX) min_OUR = metrics.at(m).OUR_min;

                    // double min_CIR = std::min(metrics.at(m).CIR_min,
                    //                          metrics.at(m).CIR_poly_min);

                    // if (metrics.at(m).CIR_min_id == UINT_MAX) min_CIR = metrics.at(m).CIR_poly_min;
                    // else if (metrics.at(m).CIR_poly_min_id == UINT_MAX) min_CIR = metrics.at(m).CIR_min;

                    // double min_KRR = std::min(metrics.at(m).KRR_min,
                    //                          metrics.at(m).KRR_poly_min);

                    // if (metrics.at(m).KRR_min_id == UINT_MAX) min_KRR = metrics.at(m).KRR_poly_min;
                    // else if (metrics.at(m).KRR_poly_min_id == UINT_MAX) min_KRR = metrics.at(m).KRR_min;

                    // double min_KAR = std::min(metrics.at(m).KAR_min,
                    //                           metrics.at(m).KAR_poly_min);

                    // if (metrics.at(m).KAR_min_id == UINT_MAX) min_KAR = metrics.at(m).KAR_poly_min;
                    // else if (metrics.at(m).KAR_poly_min_id == UINT_MAX) min_KAR = metrics.at(m).KAR_min;

                    // double min_APR = std::min(metrics.at(m).APR_min,
                    //                           metrics.at(m).APR_poly_min);

                    // if (metrics.at(m).APR_min_id == UINT_MAX) min_APR = metrics.at(m).APR_poly_min;
                    // else if (metrics.at(m).APR_poly_min_id == UINT_MAX) min_APR = metrics.at(m).APR_min;

                    // double min_MIA = std::min(metrics.at(m).MIA_min,
                    //                          metrics.at(m).MIA_poly_min);

                    // if (metrics.at(m).MIA_min_id == UINT_MAX) min_MIA = metrics.at(m).MIA_poly_min;
                    // else if (metrics.at(m).MIA_poly_min_id == UINT_MAX) min_MIA = metrics.at(m).MIA_min;

                    // double min_MAA = std::min(metrics.at(m).MAA_min,
                    //                          metrics.at(m).MAA_poly_min);
                    // min_MAA = log(min_MAA);

                    // if (metrics.at(m).MAA_min_id == UINT_MAX) min_MAA = metrics.at(m).MAA_poly_min;
                    // else if (metrics.at(m).MAA_poly_min_id == UINT_MAX) min_MAA = metrics.at(m).MAA_min;

                    // double min_ANR = std::min(metrics.at(m).ANR_min,
                    //                          metrics.at(m).ANR_poly_min);

                    // if (metrics.at(m).ANR_min_id == UINT_MAX) min_ANR = metrics.at(m).ANR_poly_min;
                    // else if (metrics.at(m).ANR_poly_min_id == UINT_MAX) min_ANR = metrics.at(m).ANR_min;

                    // double min_VEM = std::min(metrics.at(m).VEM_min,
                    //                           metrics.at(m).VEM_poly_min);

                    // if (metrics.at(m).VEM_min_id == UINT_MAX) min_VEM = metrics.at(m).VEM_poly_min;
                    // else if (metrics.at(m).VEM_poly_min_id == UINT_MAX) min_VEM = metrics.at(m).VEM_min;

                    // double min_JAC = std::min(metrics.at(m).JAC_min,
                    //                           metrics.at(m).JAC_poly_min);

                    // if (metrics.at(m).JAC_min_id == UINT_MAX) min_JAC = metrics.at(m).JAC_poly_min;
                    // else if (metrics.at(m).JAC_poly_min_id == UINT_MAX) min_JAC = metrics.at(m).JAC_min;

                    // double min_FRO = std::min(metrics.at(m).FRO_min,
                    //                           metrics.at(m).FRO_poly_min);

                    // if (metrics.at(m).FRO_min_id == UINT_MAX) min_FRO = metrics.at(m).FRO_poly_min;
                    // else if (metrics.at(m).FRO_poly_min_id == UINT_MAX) min_FRO = metrics.at(m).FRO_min;

                    switch (cbID2metricsID.at(i)) {
                        case 0: x =  INR; break;
                        case 1: x =  OUR; break;
                        case 2: x =  CIR; break;
                        case 3: x =  KRR; break;
                        case 4: x =  KAR; break;
                        case 5: x =  APR; break;
                        case 6: x =  MIA; break;
                        case 7: x =  MAA; break;
                        case 8: x =  ANR; break;
                        case 9: x =  VEM; break;
                        case 10: x = JAC; break;
                        case 11: x = FRO; break;
                    default: x = DBL_MAX;
                    }
                    switch (cbID2metricsID.at(j)) {
                        case 0: y =  INR; break;
                        case 1: y =  OUR; break;
                        case 2: y =  CIR; break;
                        case 3: y =  KRR; break;
                        case 4: y =  KAR; break;
                        case 5: y =  APR; break;
                        case 6: y =  MIA; break;
                        case 7: y =  MAA; break;
                        case 8: y =  ANR; break;
                        case 9: y =  VEM; break;
                        case 10: y = JAC; break;
                        case 11: y = FRO; break;
                    default: y = DBL_MAX;
                    }
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

//            QLogValueAxis *axisXlog = new QLogValueAxis();
//            axisXlog->setLabelFormat("%e");
//            axisXlog->setBase(10.0);
//            axisXlog->setMax(maxX);
//            ch->removeAxis(ch->axes().at(0));
//            ch->addAxis(axisXlog, Qt::AlignBottom);

//            QLogValueAxis *axisYlog = new QLogValueAxis();
//            axisYlog->setLabelFormat("%e");
//            axisYlog->setBase(10.0);
//            axisYlog->setMax(maxY);
//            ch->removeAxis(ch->axes().at(1));
//            ch->addAxis(axisYlog, Qt::AlignLeft);

            ch->axes()[0]->setMax(maxX*1.001);
            ch->axes()[0]->setMin(minX*0.999);
            ch->axes()[1]->setMax(maxY*1.001);
            ch->axes()[1]->setMin(minY*0.999);

            ch->axes()[0]->setTitleText(metrics_names.at(cbID2metricsID.at(i)).c_str());
            ch->axes()[1]->setTitleText(metrics_names.at(cbID2metricsID.at(j)).c_str());
            ch->setTitle((metrics_names.at(cbID2metricsID.at(i)) + " vs " + metrics_names.at(cbID2metricsID.at(j))).c_str());

            std::cout << chart_views.size() << " : " << metrics_names.at(cbID2metricsID.at(i)) <<
                                               " - " << metrics_names.at(cbID2metricsID.at(j)) << std::endl;
            CustomizedChartView *ch_view = new CustomizedChartView();
            ch_view->setChart(ch);

//            connect(ch_view, SIGNAL(clicked(double, double, double, double)),
//                    this, SLOT (show_tooltip (double, double, double, double)));

            chart_views.push_back(ch_view);
            ui->stackedWidget->addWidget(ch_view);
        }
    }
    ui->stackedWidget->setCurrentIndex(0);

    ui->x_axis_cb->setCurrentIndex(0);
    ui->y_axis_cb->setCurrentIndex(0);

    QScatterSeries * s = static_cast<QScatterSeries*>(chart_views.at(0)->chart()->series()[0]);
    ui->marker_size_dsb->setValue(s->markerSize());

    for (uint cc=0; cc < class_chages.size()-1; cc++)
    {
        std::string class_name = "Unknown";

        if (d.get_num_parametric_meshes() > 0)
            class_name = d.get_parametric_mesh_class_name(d.get_parametric_mesh_class_id(class_chages.at(cc))).c_str();

        ScatterPlotMarkerSettingWidget * msw = new ScatterPlotMarkerSettingWidget();
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

        ui->marker_settings_widget->layout()->addWidget(msw);
        marker_settings_widgets.push_back(msw);
    }
    connect(ui->marker_size_dsb, SIGNAL(valueChanged(double)), this, SLOT (update_marker_size (double)));
}

void GeometryGeometryScatterPlotsWidget::show_scatterplot_x(int xID)
{
    int yID = ui->y_axis_cb->currentIndex();

    std::cout << xID << " - " << yID << std::endl;

    ui->stackedWidget->setCurrentIndex(xID * ui->y_axis_cb->count()+ yID);
}

void GeometryGeometryScatterPlotsWidget::show_scatterplot_y(int yID)
{
    int xID = ui->x_axis_cb->currentIndex();

    std::cout << xID << " - " << yID << std::endl;

    ui->stackedWidget->setCurrentIndex(xID * ui->y_axis_cb->count()+ yID);
}

void GeometryGeometryScatterPlotsWidget::update_marker_size(double size)
{
    for (CustomizedChartView *ch_view : chart_views)
    {
        for (int i=0; i < ch_view->chart()->series().count(); i++)
        {
            QScatterSeries * s = static_cast<QScatterSeries*>(ch_view->chart()->series()[i]);
            s->setMarkerSize(size);
        }
    }
}

void GeometryGeometryScatterPlotsWidget::update_marker_size(uint id, double size)
{
    for (CustomizedChartView *ch_view : chart_views)
    {
        QScatterSeries * s = static_cast<QScatterSeries*>(ch_view->chart()->series()[id]);
        s->setMarkerSize(size);
    }
}

void GeometryGeometryScatterPlotsWidget::update_show_series(uint id, int b)
{
    for (CustomizedChartView *ch_view : chart_views)
    {
        QScatterSeries * s = static_cast<QScatterSeries*>(ch_view->chart()->series()[id]);
        s->setVisible(b);
    }
}

void GeometryGeometryScatterPlotsWidget::update_color_series(uint id, QColor c)
{
    for (CustomizedChartView *ch_view : chart_views)
    {
        QScatterSeries * s = static_cast<QScatterSeries*>(ch_view->chart()->series()[id]);
        s->setColor(c);
    }
}

void GeometryGeometryScatterPlotsWidget::show_tooltip(double posX, double posY, double x, double y)
{
    QToolTip::showText(QPoint(posX,posY),
                       QString::number(x) + ", " + QString::number(y));
}

void GeometryGeometryScatterPlotsWidget::on_save_btn_clicked()
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

        std::string filename = dir.toStdString() + "/" + x_axis + "__..__" + y_axis + ".png";

        std::cout << "Saving "  << filename << std::endl;

//        const auto dpr = chview->devicePixelRatioF();
//        QPixmap buffer(chview->width() * dpr, chview->height() * dpr);
//        buffer.setDevicePixelRatio(dpr);
//        buffer.fill(Qt::transparent);

//        QPainter *paint = new QPainter(&buffer);
//        paint->setPen(*(new QColor(255,34,255,255)));
//        chview->render(paint);

//        QFile file(filename.c_str());
//        file.open(QIODevice::WriteOnly);
//        buffer.save(&file, "PNG");

//        delete  paint;

        QPixmap p = chview->grab();
        p.save(filename.c_str());
    }

}
