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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "dataset.h"
#include "meshes/mesh_metrics.h"

#include "quality_metrics.h"

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void compute_GP_scatterplots ();

    void show_mesh_metrics();
    void show_sorted_mesh_metrics(const uint to_be_sort_id);
    void show_full_mesh_metrics();

    void show_solver_results (const uint solution_id, const std::string folder, const std::string filename);

    void update_solver_input_folder (const std::string folder);

    void on_reset_btn_clicked();

    void on_tab_widgets_currentChanged(int index);

    void on_save_all_btn_clicked();

private:
    Ui::MainWindow *ui;

    const unsigned int n_metrics = static_cast<const uint>( metrics_names.size());

    std::vector<MeshMetrics> metrics;
    std::vector<std::vector<double>> errsToScatterPlots;

    Dataset dataset;

    std::string folder = "";

    const std::string project_ext = ".changeproj";

    std::vector<QColor> extra_colors;

    const double default_missing_geometric_val = -1.0;

};

#endif // MAINWINDOW_H
