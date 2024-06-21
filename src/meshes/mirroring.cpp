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

#include "mirroring.h"
#include "non_uniform_scaling_01.h"
#include <cinolib/vertex_clustering.h>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void translate(cinolib::Polygonmesh<> &tin, const cinolib::vec3d &t)
{
    //Node *n;
    //Vertex *v;
    //FOREACHVVVERTEX((&(tin->V)), v, n) (*v) += t;

    for (uint vid=0; vid < tin.num_verts(); vid++)
    {
        cinolib::vec3d pos = tin.vert(vid) + t;
        tin.vert(vid).x() = pos.x();
        tin.vert(vid).y() = pos.y();
        tin.vert(vid).z() = 0.0;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void mirror_x(cinolib::Polygonmesh<> &tin)
{
    //Node *n;
    //Vertex *v;
    //FOREACHVVVERTEX((&(tin->V)), v, n) v->x = -v->x;

    for (uint vid=0; vid < tin.num_verts(); vid++)
    {
        cinolib::vec3d pos = -tin.vert(vid);
        tin.vert(vid).x() = pos.x();
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void mirror_y(cinolib::Polygonmesh<> &tin)
{
    //Node *n;
    //Vertex *v;
    //FOREACHVVVERTEX((&(tin->V)), v, n) v->y = -v->y;

    for (uint vid=0; vid < tin.num_verts(); vid++)
    {
        cinolib::vec3d pos = -tin.vert(vid);
        tin.vert(vid).y() = pos.y();
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void apply_mirroring (cinolib::Polygonmesh<> &mesh)
{
    cinolib::Polygonmesh<> meshx, meshy, meshxy;

    cinolib::vec3d bbm, bbM;
    cinolib::AABB bb = mesh.bbox();
    bbm = bb.min;
    bbM = bb.max;

    mesh.translate(cinolib::vec3d(-bbm.x(), -bbm.y(), 0.0));

    meshx = mesh;
    mirror_x(meshx);
    //tinx.flipNormals();

    for (uint pid=0; pid < meshx.num_polys(); pid++)
        meshx.poly_flip_winding_order(pid);

    meshy = mesh;
    mirror_y(meshy);
    //tiny.flipNormals();

    for (uint pid=0; pid < meshy.num_polys(); pid++)
        meshy.poly_flip_winding_order(pid);

    meshxy = mesh;
    mirror_x(meshxy);
    mirror_y(meshxy);

    mesh += meshx;
    mesh += meshy;
    mesh += meshxy;

    std::vector<uint> poly_tb_rem;
    std::vector<std::vector<uint>> poly_tb_added;
    std::vector<uint> boundary_verts_id = mesh.get_boundary_vertices();
    std::vector<cinolib::vec3d> boundary_verts;

    for (uint vid : boundary_verts_id)
        boundary_verts.push_back(mesh.vert(vid));

    std::vector<std::unordered_set<uint>> tb_merged;
    cinolib::vertex_clustering (boundary_verts, 1e-12, tb_merged);

    bool stop = false;

    while (!stop)
    {
        bool is_merged = false;

        for (uint i=0; i < tb_merged.size(); i++)
        {
            if (tb_merged.at(i).size() == 1) continue;

            std::vector<uint> set_vect;
            for (uint ii : tb_merged.at(i)) set_vect.push_back(boundary_verts_id.at(ii));

            std::sort (set_vect.begin(), set_vect.end());

            uint keep_vid = set_vect.at(0);

            std::reverse (set_vect.begin(), set_vect.end());

            for (uint v : set_vect)
            {
                if (v == keep_vid) continue;
                mesh.vert_merge(keep_vid, v);
            }

            is_merged = true;

            tb_merged.clear();

            boundary_verts_id = mesh.get_boundary_vertices();
            boundary_verts.clear();

            for (uint vid : boundary_verts_id)
                boundary_verts.push_back(mesh.vert(vid));

            cinolib::vertex_clustering (boundary_verts, 1e-10, tb_merged);

            break;
        }

        if (! is_merged)
            stop = true;
    }

    non_uniform_scaling(mesh);
}
