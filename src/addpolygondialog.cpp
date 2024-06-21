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

#include "addpolygondialog.h"
#include "ui_addpolygondialog.h"

#include <cinolib/meshes/polygonmesh.h>

#include <QFileDialog>

AddPolygonDialog::AddPolygonDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AddPolygonDialog) {
  ui->setupUi(this);

  setWindowTitle(tr("Select Polygon Class"));

  update_img(getClassIndex());

  connect(ui->class_list, SIGNAL(currentIndexChanged(int)), this,
          SLOT(load_mesh_and_update_image(int)));

  ui->preview_canvas->hide();
  ui->preview_filename->hide();

  ui->change_poly_btn->hide();

  ui->deformation_label->hide();
  ui->deformation_param->hide();
}

AddPolygonDialog::~AddPolygonDialog() { delete ui; }

int AddPolygonDialog::getClassIndex() const {
  return ui->class_list->currentIndex();
}

std::string AddPolygonDialog::getClassName() const {
  return ui->class_list->currentText().toStdString();
}

double AddPolygonDialog::getDeformationParam() const {
  return ui->deformation_param->value();
}

void AddPolygonDialog::update_img(int index)
{
    if (index == 8)
    {
        ui->preview_img->hide();
        ui->preview_canvas->show();
        ui->preview_filename->show();
        ui->change_poly_btn->show();

        // ui->preview_canvas->pop_all_occurrences_of(cinolib::DRAWABLE_POLYGONMESH);

        // ui->preview_canvas->push_obj(new cinolib::DrawablePolygonmesh<> (mesh->vector_verts(), mesh->vector_polys()));
        ui->preview_canvas->clear();

        cinolib::Color c = cinolib::Color(1,0,0,1);
        ui->preview_canvas->add_mesh(cinolib::Polygonmesh<> (mesh->vector_verts(), mesh->vector_polys()), c);

        ui->preview_filename->setText(mesh->mesh_data().filename.c_str());
        return;
    }

    QPixmap pm;

    switch (index) {
    case 0:
    pm.load(":/class_img/img/comb.png"); // <- path to image file
    break;
    case 1:
    pm.load(":/class_img/img/convexity.png"); // <- path to image file
    break;
    case 2:
    pm.load(":/class_img/img/isotropy.png"); // <- path to image file
    break;
    case 3:
    pm.load(":/class_img/img/maze.png"); // <- path to image file
    break;
    case 4:
    pm.load(":/class_img/img/nsided.png"); // <- path to image file
    break;
    case 5:
    pm.load(":/class_img/img/star.png"); // <- path to image file
    break;
    case 6:
    pm.load(":/class_img/img/ulike.png"); // <- path to iAddPolygonDialogmage file
    break;
    case 7:
    pm.load(":/class_img/img/zeta.png"); // <- path to image file
    break;
    default:
    break;
    }

    ui->preview_img->show();
    // ui->preview_canvas->hide();
    ui->preview_filename->hide();
    ui->change_poly_btn->hide();

    // QPixmap pms = pm.scaledToWidth(ui->preview_img->width());
    ui->preview_img->setPixmap(pm);
}

void AddPolygonDialog::load_mesh_and_update_image (int index)
{
    if (index == 8) // Parametric poly
    {
        QString filename = QFileDialog::getOpenFileName(this,
                tr("Open Polygon"), "",
                tr("OBJ (*.obj);;All Files (*)"));

        if (filename.isEmpty())
        {
            ui->class_list->setCurrentIndex(last_selected_index);
            load_mesh_and_update_image(last_selected_index);
            return;
        }

        mesh = new cinolib::Polygonmesh<>(filename.toStdString().c_str());
    }

    last_selected_index = index;

    update_img(index);
}

void AddPolygonDialog::on_change_poly_btn_clicked()
{
    load_mesh_and_update_image(8);
}
