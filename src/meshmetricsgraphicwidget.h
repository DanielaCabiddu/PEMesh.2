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

#ifndef MESHMETRICSGRAPHICWIDGET_H
#define MESHMETRICSGRAPHICWIDGET_H

#include "dataset.h"

#include <cinolib/meshes/drawable_polygonmesh.h>

#include <QWidget>

namespace Ui {
class MeshMetricsGraphicWidget;
}

class MeshMetricsGraphicWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MeshMetricsGraphicWidget(QWidget *parent = nullptr);
    ~MeshMetricsGraphicWidget();

    void clean_canvas ();

    void set_dataset (Dataset *ds);
    void set_metrics (std::vector<MeshMetrics> *m);

    void set_slider_max (const uint max);

public slots:

    void show_mesh (int i);

    void show_inr ();
    void show_our ();
    void show_cir ();
    void show_krr ();
    void show_kar ();
    void show_apr ();
    void show_mia ();
    void show_maa ();
    void show_anr ();
    void show_vem ();
    void show_jac ();
    void show_fro ();


    void show_min ();
    void show_max ();
    void show_avg ();
    void show_poly ();

private slots:
    void on_mesh_metrics_slider_valueChanged(int value);

private:
    Ui::MeshMetricsGraphicWidget *ui;

    std::vector<cinolib::DrawablePolygonmesh<> *> mesh_with_metrics;

    Dataset *d = nullptr;

    std::vector<MeshMetrics> * metrics = nullptr;

    uint curr_mesh_id = 0;
    bool update_scene = true;

    void set_min_color (const uint i);
    void set_max_color (const uint i);

};

#endif // MESHMETRICSGRAPHICWIDGET_H
