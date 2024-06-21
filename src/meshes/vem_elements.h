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

#ifndef VEM_ELEMENTS_H
#define VEM_ELEMENTS_H

#include "abstract_vem_element.h"
#include <cinolib/meshes/polygonmesh.h>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

class IsotropyTester : public AbstractVEMelement
{
    public:
        IsotropyTester();
        cinolib::Polygonmesh<> deform(float t) const;
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

class ConvexityTester : public AbstractVEMelement
{
    public:
        ConvexityTester();
        cinolib::Polygonmesh<> deform(float t) const;
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

class NumSidesTester : public AbstractVEMelement
{
    public:
        NumSidesTester(const uint max_sides = 40);
        cinolib::Polygonmesh<> deform(float t) const;
    private:
        uint max_sides;
        cinolib::Polygonmesh<> n_sided_polygon(const uint n_sides) const;
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

class MazeTester : public AbstractVEMelement
{
    public:
        MazeTester();
        cinolib::Polygonmesh<> deform(float t) const;
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

class StarTester : public AbstractVEMelement
{
    public:
        StarTester(const uint max_spikes = 40);
        cinolib::Polygonmesh<> deform(float t) const;
    private:
        uint max_spikes;
        cinolib::Polygonmesh<> n_sided_polygon(const uint n_sides) const;
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

class UTester : public AbstractVEMelement
{
    public:
        UTester();
        cinolib::Polygonmesh<> deform(float t) const;
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

class ZetaTester : public AbstractVEMelement
{
    public:
        ZetaTester();
        cinolib::Polygonmesh<> deform(float t) const;
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

class CombTester : public AbstractVEMelement
{
    public:
        CombTester(const uint max_dents = 10);
        cinolib::Polygonmesh<> deform(float t) const;
    private:
        uint max_dents;
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

class RandomTester : public AbstractVEMelement
{
    public:
        RandomTester(const std::string filename);
        cinolib::Polygonmesh<> deform(float t) const;
    private:
        std::string filename;
};


#endif // VEM_ELEMENTS_H
