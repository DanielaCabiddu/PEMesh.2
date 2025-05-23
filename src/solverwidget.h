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

#ifndef SOLVERWIDGET_H
#define SOLVERWIDGET_H

#include "dataset.h"

#include <QProcess>
#include <QTextCharFormat>

#include <QWidget>

namespace Ui {
class SolverWidget;
}

class SolverWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SolverWidget(QWidget *parent = nullptr);
    ~SolverWidget();

    void set_dataset (Dataset *d);
    void set_input_folder (const std::string folder);

    bool get_last_condition_number_state () const { return condition_number_selected; }

    void update ();
    void clean_canvas ();

Q_SIGNALS:

    void solver_completed (const uint solution_id, const std::string folder, const std::string filename);

public slots:

    void show_parametric_mesh (int);
    void update_log (const QString text);
    void update_log ();

private slots:
    void on_t_slider_valueChanged(int value);

    void on_run_btn_clicked();

    void on_solver_cb_currentIndexChanged(int index);

private:
    Ui::SolverWidget *ui;

    Dataset *dataset = nullptr;
    uint curr_mesh_id = cinolib::max_uint;

    std::string input_folder ;
    std::string output_folder;

    bool update_scene = true;
    bool condition_number_selected = false;

    std::vector<cinolib::Polygonmesh<> *> drawable_polys;

    QProcess *process = nullptr;

    void update_equation_preview(int id);

    void parseEscapeSequence(int attribute, QListIterator< QString > & i, QTextCharFormat & textCharFormat, QTextCharFormat const & defaultTextCharFormat);

};

#endif // SOLVERWIDGET_H
