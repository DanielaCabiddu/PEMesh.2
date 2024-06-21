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

#ifndef ABSTRACT_VEM_ELEMENT_H
#define ABSTRACT_VEM_ELEMENT_H

#include <cinolib/meshes/polygonmesh.h>

// This is an abstract class that defines a parametric VEM element.
// Istances of it should implement the deform() method, defining a
// controlled deformation of the element aimed to emphasize (or reduce)
// the extent of a certain quality metric, such as isotropy, angles, number
// of edges, edge lengths, etc.

class AbstractVEMelement
{
    public:

        AbstractVEMelement();

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        // produce a controlled deformation of the element aimed to emphasize
        // (or reduce) the extent of a certain quality metric, such as isotropy,
        // angles, number of edges, edge lengths, etc.
        //
        virtual cinolib::Polygonmesh<> deform(float t) const = 0; // t \in [0,1]

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        // create a square canvas around the abstract element
        // default triangle parameter: min angle 20 deg
        cinolib::Polygonmesh<> deform_with_canvas(float t, const std::string flags="cq20.0a") const;

        //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        cinolib::Polygonmesh<> base_elem;
};

#endif // ABSTRACT_VEM_ELEMENT_H
