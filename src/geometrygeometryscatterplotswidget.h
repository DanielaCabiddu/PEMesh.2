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

#ifndef GEOMETRYGEOMETRYSCATTERPLOTSWIDGET_H
#define GEOMETRYGEOMETRYSCATTERPLOTSWIDGET_H

#include <QWidget>

#include "customizedchartview.h"
#include "dataset.h"
#include "meshes/mesh_metrics.h"
#include "scatterplotmarkersettingwidget.h"

namespace Ui {
class GeometryGeometryScatterPlotsWidget;
}

class GeometryGeometryScatterPlotsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GeometryGeometryScatterPlotsWidget(QWidget *parent = nullptr);
    ~GeometryGeometryScatterPlotsWidget();

    void create_scatterPlots (const Dataset &d, const std::vector<MeshMetrics> &metrics);

private slots:

    void show_scatterplot_x (int xID);
    void show_scatterplot_y (int yID);

    void update_marker_size (double size);
    void update_marker_size (uint id, double size);

    void update_show_series (uint id, int b);
    void update_color_series (uint id, QColor c);

    void show_tooltip (double posX, double posY, double x, double y);

    void on_save_btn_clicked();

private:
    Ui::GeometryGeometryScatterPlotsWidget *ui;

    bool update_settings_bar = true;

    std::vector<uint> cbID2metricsID;

    std::vector<CustomizedChartView * > chart_views;
    std::vector<ScatterPlotMarkerSettingWidget *> marker_settings_widgets;

    std::vector<QColor> extra_colors;
};

#endif // GEOMETRYGEOMETRYSCATTERPLOTSWIDGET_H
