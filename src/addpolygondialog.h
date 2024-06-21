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

#ifndef ADDPOLYGONDIALOG_H
#define ADDPOLYGONDIALOG_H

#include <cinolib/meshes/polygonmesh.h>

#include <QDialog>

namespace Ui {
class AddPolygonDialog;
}

class AddPolygonDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddPolygonDialog(QWidget *parent = nullptr);
    ~AddPolygonDialog();

    int getClassIndex() const;
    std::string getClassName () const;
    double getDeformationParam() const;

    cinolib::Polygonmesh<> * getMesh () const { return mesh; }

private slots:

    void update_img (int index);
    void load_mesh_and_update_image (int index);

    void on_change_poly_btn_clicked();

private:
    Ui::AddPolygonDialog *ui;

    cinolib::Polygonmesh<> *mesh = nullptr;

    uint last_selected_index = 0;
};

#endif // ADDPOLYGONDIALOG_H
