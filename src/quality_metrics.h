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

#ifndef QUALITY_METRICS_H
#define QUALITY_METRICS_H

#include <math.h>

#include <string>
#include <vector>

#include <cinolib/min_max_inf.h>

const std::vector<std::string> metrics_names =
{
    "Inradius Ratio",
    "Outradius Ratio",
    "Circle Ratio",
    "Kernel Radius Ratio",
    "Kernel Area Ratio",
    "Area Perimeter Ratio",
    "Minimum Angle",
    "Maximum Angle",
    "Angle Ratio",
    "VEM Indicator",
    "Scaled Jacobian",
    "Frobenius Ratio"
};

const std::vector<std::string> metrics_acronym =
{
    "INR",
    "OUR",
    "CIR",
    "KRR",
    "KAR",
    "APR",
    "MIA",
    "MAA",
    "ANR",
    "VEM",
    "JAC",
    "FRO"
};

const std::vector<bool> metrics_scale_dependent =
{
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false
};

const std::vector<std::pair<double, double>> metrics_ranges
{
    std::pair<double, double> (0, 1),
    std::pair<double, double> (0, 1),
    std::pair<double, double> (0, 1),
    std::pair<double, double> (0, 1),
    std::pair<double, double> (0, 1),
    std::pair<double, double> (0, 1),
    std::pair<double, double> (0, 1),
    std::pair<double, double> (0, 1),
    std::pair<double, double> (0, 1),
    std::pair<double, double> (0, 1),
    std::pair<double, double> (0, 1),
    std::pair<double, double> (0, 1)
};

inline
double get_metrics_min (const uint i)
{
    return metrics_ranges.at(i).first;
}

inline
double get_metrics_max (const uint i)
{
    return metrics_ranges.at(i).second;
}

#endif // QUALITY_METRICS_H
