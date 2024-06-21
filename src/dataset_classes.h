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

#ifndef DATASET_CLASSES_H
#define DATASET_CLASSES_H

#include <map>

#ifndef _WIN32
#include <sys/types.h>
#else
typedef unsigned int uint;
#endif
#include <string>
#include <vector>

const std::vector<std::string> classNames
{
    "Comb",
    "Convexity",
    "Isotropy",
    "Maze",
    "N-Sided",
    "Star",
    "U-Like",
    "Zeta"
};

const std::map<std::string, uint> className2classId
{
    std::pair<std::string, uint> (classNames.at(0), 0),
    std::pair<std::string, uint> (classNames.at(1), 1),
    std::pair<std::string, uint> (classNames.at(2), 2),
    std::pair<std::string, uint> (classNames.at(3), 3),
    std::pair<std::string, uint> (classNames.at(4), 4),
    std::pair<std::string, uint> (classNames.at(5), 5),
    std::pair<std::string, uint> (classNames.at(6), 6),
    std::pair<std::string, uint> (classNames.at(7), 7)
};

const std::vector<std::string> classPrefix
{
    "comb_",
    "convexity_",
    "isotropy_",
    "maze_",
    "num_sides_",
    "star_",
    "u_like_",
    "zeta_",
    "random",
};

#endif // DATASET_CLASSES_H
