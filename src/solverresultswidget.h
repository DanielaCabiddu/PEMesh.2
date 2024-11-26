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

#ifndef SOLVERRESULTSWIDGET_H
#define SOLVERRESULTSWIDGET_H

#include "dataset.h"

#include <cinolib/cino_inline.h>
#include <cinolib/meshes/drawable_polygonmesh.h>
#include <cinolib/scalar_field.h>

#include <QtCharts/QCategoryAxis>
#include <QtCharts/QChartView>

#include "customizedchartview.h"

#include <QWidget>

namespace Ui {
class SolverResultsWidget;
}

class SolverResultsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SolverResultsWidget(QWidget *parent = nullptr);
    ~SolverResultsWidget();

    void add_chart(CustomizedChartView *chart, const std::string name);
    void clean_charts();

    // void set_dataset (Dataset *d);
    void set_solution_id (const uint id) { solution_id = id; }

    void show_mesh (const uint index);

    // void show_mesh_solution_and_groundtruth () const;

    void clean_canvas ();

    void set_residuals_on();
    void set_condition_number_on();

    // cinolib::Polygonmesh<> * get_gt_mesh (const uint i);

public slots:

    // void show_parametric_mesh (int);

private slots:

    // void on_t_slider_valueChanged(int value);

    void on_save_plots_btn_clicked();

    void on_errH1_color_btn_clicked();

    void on_errInf_color_btn_clicked();

    void on_errL2_color_btn_clicked();

    void on_dofs_color_btn_clicked();

    void on_hEmax_color_btn_clicked();

    void on_condVect_color_btn_clicked();

    void on_all_color_btn_clicked();

    void on_deformation_cb_stateChanged(int arg1);

    void on_meshsize_cb_stateChanged(int arg1);

    void on_loglog_cb_checkStateChanged(const Qt::CheckState &checked);

private:
    Ui::SolverResultsWidget *ui;

    uint solution_id = UINT_MAX;

    unsigned int last_row = 0;
    unsigned int last_col = 0;
    const unsigned int n_cols = 3;

    std::vector<CustomizedChartView *> chart_views;
    std::vector<std::string> chart_views_names;

    bool update_scene = true;
    uint curr_mesh_id = cinolib::max_uint;

    void change_series_color (const uint series_id);

};

#endif // SOLVERRESULTSWIDGET_H
