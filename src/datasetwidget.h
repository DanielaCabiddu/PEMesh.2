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

#ifndef DATASETWIDGET_H
#define DATASETWIDGET_H

#include "dataset.h"

#include "meshes/abstract_vem_element.h"
#include "meshes/mesh_metrics.h"

#include <cinolib/meshes/meshes.h>

#include <QDoubleSpinBox>
#include <QPushButton>
#include <QWidget>

struct SelectedPolyData
{
    uint class_type = UINT_MAX;
    std::string class_name = "";
    std::string filename = "";
    cinolib::Polygonmesh<> *mesh = nullptr;

};

namespace Ui {
class DatasetWidget;
}

class DatasetWidget : public QWidget {
  Q_OBJECT

public:
  explicit DatasetWidget(QWidget *parent = nullptr);
  ~DatasetWidget();

  std::vector<MeshMetrics> get_parametric_meshes_metrics () const { return  dataset->get_parametric_meshes_metrics(); }

  Dataset *get_dataset() { return dataset; }

  void set_dataset(Dataset *d) { dataset = d; }

  void compute_geometric_metrics();

public slots:

  void polygon_zoom_in();
  void polygon_zoom_out();
  void rotate_polygon(double angle);
  void polygon_zoom_in_out(double s);

  void move_polygon_on_x(double new_pos);
  void move_polygon_on_y(double new_pos);

private slots:
  void on_add_btn_clicked();

  void on_geom_qualities_btn_clicked();

  void on_generate_dataset_btn_clicked();

  void show_parametric_mesh(int id);

  void on_save_btn_clicked();

//  void on_sort_geom_qualities_btn_clicked();

  void on_save_polys_btn_clicked();

  void on_load_polys_btn_clicked();

  void on_load_meshes_btn_clicked();

  void on_aggregate_btn_clicked();

  void on_mirroring_btn_clicked();

  void on_show_coords_cb_stateChanged(int checked);

  void on_highlight_polys_cb_stateChanged(int checked);

  void on_optimize_btn_clicked();

  void on_load_mesh_btn_clicked();

  Q_SIGNALS:

  void computed_mesh_metrics();
  void computed_parametric_dataset();
  void mirrored();

  void sort_geometric_qualities(const uint id);

  void saved_in(const std::string folder);

private:
    Ui::DatasetWidget *ui;

    std::vector<AbstractVEMelement *> elems;

    std::vector<uint> elems_class_types;
    std::vector<std::string> elems_class_name;
    std::vector<std::string> elems_filename;
    std::vector<cinolib::vec3d> elems_centers;
    std::vector<double> elems_scale_factors;
    std::vector<double> elems_rotation_angles;

    std::vector<cinolib::Polygonmesh<> *> drawable_polys;
    // std::vector<cinolib::Marker *> markers;

    std::vector<QPushButton *> plus_buttons;
    std::vector<QPushButton *> minus_buttons;
    std::vector<QDoubleSpinBox *> rotation_angles;
    std::vector<QDoubleSpinBox *> scale_factors;

    std::vector<QDoubleSpinBox *> x_poses;
    std::vector<QDoubleSpinBox *> y_poses;

    //double last_angle = 0.0;
    const double minus_scale_factor = 0.8;

    Dataset *dataset = nullptr;

    std::string dataset_folder;

    bool enable_add_polygon = true;
    bool reset_canvas = true;

    AbstractVEMelement * create_element (const uint elem_type, const std::string fname = "") const;
    void deform_elem (const double value, const bool with_canvas = false);

    void clean_canvas ();

    void add_polygon (/*cinolib::GLcanvas *canvas, */QMouseEvent *event) ;
    void add_polygon (const SelectedPolyData selected_poly, const cinolib::vec2d &pos = cinolib::vec2d(0.5, 0.5), const bool scale_to_fit = true);

    const SelectedPolyData ask_polygon_class() const ;

    void add_polygon (const SelectedPolyData selected_poly,
                      const cinolib::vec2d &pos,
                      const double rotation_angle,
                      const double scale_factor) ;

    cinolib::Polygonmesh<> deform_with_canvas(const std::vector<cinolib::Polygonmesh<> *> &elems_polys, const std::string triangle_flags);

    void polygon_zoom_in (cinolib::Polygonmesh<> *m);
    void polygon_zoom_out (cinolib::Polygonmesh<> *m);

    void aggregate_triangles (cinolib::Polygonmesh<> &m, const double value_bound, const int flag);
};

#endif // DATASETWIDGET_H
