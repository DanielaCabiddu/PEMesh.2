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

#ifndef DATASET_H
#define DATASET_H

#include "dataset_classes.h"

#include "meshes/mesh_metrics.h"

#include <cinolib/meshes/meshes.h>

class Dataset {
public:
    Dataset();

    void clean ();

    void save_on_disk (const std::string directory);
    bool is_on_disk () const;

    uint get_num_parametric_meshes () const { return parametric_meshes.size(); }

    void add_parametric_mesh (cinolib::Polygonmesh<> *m, const double t, const uint class_id);
    void add_parametric_mesh_metrics (const MeshMetrics &m) { parametric_meshes_metrics.push_back(m); }

    void overwrite_parametric_mesh (const uint index, cinolib::Polygonmesh<> *m, const double t, const uint class_id);

    const std::vector<cinolib::Polygonmesh<> *> & get_parametric_meshes  ()             const { return parametric_meshes; }
                      cinolib::Polygonmesh<> *    get_parametric_mesh    (const uint i) const { return parametric_meshes.at(i); }

    const std::vector<MeshMetrics>  & get_parametric_meshes_metrics     () const { return parametric_meshes_metrics; }
    const std::vector<double>       & get_parametric_meshes_t           () const { return parametric_meshes_t; }

    const std::vector<uint>         & get_parametric_meshes_class_id    () const { return parametric_meshes_class_ids; }
    const std::vector<std::string>  & get_parametric_meshes_class_names () const { return classNames; }

                 uint get_parametric_mesh_class_id   (const uint i) const { return parametric_meshes_class_ids.at(i); }
    const std::string get_parametric_mesh_class_name (const uint i) const { if (i >= classNames.size()) return "Unknown Class"; return classNames.at(i); }

    const std::string get_parametric_mesh_filename (const uint i) const { return parametric_meshes.at(i)->mesh_data().filename; }

private:

    std::vector<cinolib::Polygonmesh<> *> parametric_meshes;

    std::vector<MeshMetrics> parametric_meshes_metrics;

    std::vector<double>      parametric_meshes_t;

    std::vector<uint>        parametric_meshes_class_ids;
};

#endif // DATASET_H
