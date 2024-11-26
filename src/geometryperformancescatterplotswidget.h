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

#ifndef GEOMETRYPERFORMANCESCATTERPLOTSWIDGET_H
#define GEOMETRYPERFORMANCESCATTERPLOTSWIDGET_H

#include "customizedchartview.h"
#include "dataset.h"
#include "meshes/mesh_metrics.h"
#include "scatterplotmarkersettingwidget.h"

#include <QWidget>

namespace Ui {
class GeometryPerformanceScatterplotsWidget;
}

class GeometryPerformanceScatterPlotsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GeometryPerformanceScatterPlotsWidget(QWidget *parent = nullptr);
    ~GeometryPerformanceScatterPlotsWidget();

    void create_scatterPlots (const Dataset d,
                              const std::vector<MeshMetrics> &metrics,
                              const std::vector<std::vector<double>> &performances);

    void set_empty ();
    void set_solution_id (const uint id) { solution_id = id; }

    void set_residuals_on();
    void set_condition_number_on();

Q_SIGNALS:

    void compute_GP_scatterplots ();

private slots:

    void show_scatterplot_x (int xID);
    void show_scatterplot_y (int yID);

    void update_marker_size (double size);
    void update_marker_size (uint id, double val);

    void update_show_series (uint id, int b);

    void update_color_series (uint id, QColor c);

    void on_pushButton_clicked();

    void on_compute_btn_clicked();

    void on_y_axis_cb_currentIndexChanged(int index);

private:
    Ui::GeometryPerformanceScatterplotsWidget *ui;

    uint solution_id = UINT_MAX;
    bool update_settings_bar = true;

    std::vector<uint> cbID2metricsID;

    std::vector<CustomizedChartView * > chart_views;
    std::vector<ScatterPlotMarkerSettingWidget *> marker_settings_widgets;

    std::vector<QColor> extra_colors;


};

#endif // GEOMETRYPERFORMANCESCATTERPLOTSWIDGET_H
