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

#include "dataset.h"

#include <iomanip>
#include <sstream>

#include <QDir>

Dataset::Dataset() {}

void Dataset::save_on_disk(const std::string directory)
{
    QString dir = QString(directory.c_str());

    if (directory.size() > 0)
        dir += QDir::separator();

    if (parametric_meshes.size() > 0)
    {
        unsigned int index = 0;

        for (const cinolib::Polygonmesh<> *polymesh : parametric_meshes)
        {
            std::string filename;

            if (polymesh->mesh_data().filename.length() == 0)
            {
                std::stringstream ss;
                ss << std::setw((parametric_meshes.size() / 10) + 1)
                   << std::setfill('0') << index;
                std::string s = ss.str();

                filename = ss.str() + "_";
                filename = filename + std::to_string(parametric_meshes_t.at(index));
            }
            else
            {
                filename = polymesh->mesh_data().filename.substr(polymesh->mesh_data().filename.find_last_of("/")+1);
            }


            std::string filepath = dir.toStdString() + filename;

            std::cout << "Saving " << filepath << std::endl;

            // write_NODE_ELE_2D(filepath.c_str(), polymesh->vector_verts(), polymesh->vector_polys());

            filepath += ".off";
            polymesh->save((filepath).c_str());

            parametric_meshes.at(index)->mesh_data().filename = filepath;

            index++;
        }
    }
}

void Dataset::clean()
{
    for (cinolib::Polygonmesh<> *p : parametric_meshes)
        delete p;

    parametric_meshes.clear();
    parametric_meshes_metrics.clear();
    parametric_meshes_t.clear();
}

void Dataset::add_parametric_mesh (cinolib::Polygonmesh<> *m, const double t, const uint class_id)
{
    parametric_meshes.push_back(m);
    parametric_meshes_t.push_back(t);
    parametric_meshes_class_ids.push_back(class_id);
}

void Dataset::overwrite_parametric_mesh (const uint index, cinolib::Polygonmesh<> *m, const double t, const uint class_id)
{
    parametric_meshes.at(index) = m;
    parametric_meshes_t.at(index) = t;
    parametric_meshes_class_ids.at(index) = class_id;
}

bool Dataset::is_on_disk () const
{
    if (parametric_meshes.empty())
        return false;

    if (parametric_meshes.at(0)->mesh_data().filename.length() == 0)
        return false;

    return true;
}
