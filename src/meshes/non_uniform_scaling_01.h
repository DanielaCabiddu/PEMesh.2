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

#ifndef NON_UNIFORM_SCALE_H
#define NON_UNIFORM_SCALE_H

#include <cinolib/meshes/polygonmesh.h>
#include <cinolib/min_max_inf.h>

void non_uniform_scaling (cinolib::Polygonmesh<> &mesh)
{
    double minX = mesh.bbox().min.x(), maxX = mesh.bbox().max.x();
    double minY = mesh.bbox().min.y(), maxY = mesh.bbox().max.y();

    for (uint vid=0; vid < mesh.num_verts(); vid++)
    {
        mesh.vert(vid).x() = (mesh.vert(vid).x() - minX) / (maxX - minX);
        mesh.vert(vid).y() = (mesh.vert(vid).y() - minY) / (maxY - minY);
    }

    mesh.update_bbox();
}

#endif
