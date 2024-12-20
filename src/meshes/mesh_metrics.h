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

#ifndef MESH_METRICS_H
#define MESH_METRICS_H

#include <cinolib/meshes/polygonmesh.h>
#include <cinolib/min_max_inf.h>

#include <limits.h>
#include <vector>
#include <cfloat>


struct MeshMetrics
{
    // INR - Inradius/Diameter Ratio - Range: [0,1] - high is good - scale INdependent
    double INR_mesh         = 0.0;
    double INR_min          = DBL_MAX;
    double INR_max          = DBL_MIN;
    double INR_avg          = 0.0;
    double INR_poly_min     = DBL_MAX;
    double INR_poly_max     = DBL_MIN;
    double INR_poly_avg     = 0.0;
    double INR_global_avg   = 0.0;
    double INR_global_norm  = 0.0;
    uint   INR_min_id       = UINT_MAX;
    uint   INR_max_id       = UINT_MAX;
    uint   INR_poly_min_id  = UINT_MAX;
    uint   INR_poly_max_id  = UINT_MAX;
    std::vector<double> INR_all;

    // OUR - Outradius/Diameter Ratio - Range: [0,1] - high is good - scale INdependent
    double OUR_mesh         = 0.0;
    double OUR_min          = DBL_MAX;
    double OUR_max          = DBL_MIN;
    double OUR_avg          = 0.0;
    double OUR_poly_min     = DBL_MAX;
    double OUR_poly_max     = DBL_MIN;
    double OUR_poly_avg     = 0.0;
    double OUR_global_avg   = 0.0;
    double OUR_global_norm  = 0.0;
    uint   OUR_min_id       = UINT_MAX;
    uint   OUR_max_id       = UINT_MAX;
    uint   OUR_poly_min_id  = UINT_MAX;
    uint   OUR_poly_max_id  = UINT_MAX;
    std::vector<double> OUR_all;

    // CIR - Circle Ratio - Range: [0,1] - high is good - scale INdependent
    double CIR_mesh         = 0.0;
    double CIR_min          = DBL_MAX;
    double CIR_max          = DBL_MIN;
    double CIR_avg          = 0.0;
    double CIR_poly_min     = DBL_MAX;
    double CIR_poly_max     = DBL_MIN;
    double CIR_poly_avg     = 0.0;
    double CIR_global_avg   = 0.0;
    double CIR_global_norm  = 0.0;
    uint   CIR_min_id       = UINT_MAX;
    uint   CIR_max_id       = UINT_MAX;
    uint   CIR_poly_min_id  = UINT_MAX;
    uint   CIR_poly_max_id  = UINT_MAX;
    std::vector<double> CIR_all;

    // KRR - Kernel Radius Ratio - Range: [0,1] - high is good - scale INdependent
    double KRR_mesh         = 0.0;
    double KRR_min          = DBL_MAX;
    double KRR_max          = DBL_MIN;
    double KRR_avg          = 0.0;
    double KRR_poly_min     = DBL_MAX;
    double KRR_poly_max     = DBL_MIN;
    double KRR_poly_avg     = 0.0;
    double KRR_global_avg   = 0.0;
    double KRR_global_norm  = 0.0;
    uint   KRR_min_id       = UINT_MAX;
    uint   KRR_max_id       = UINT_MAX;
    uint   KRR_poly_min_id  = UINT_MAX;
    uint   KRR_poly_max_id  = UINT_MAX;
    std::vector<double> KRR_all;

    // KAR - Kernel Area Ratio - Range: [0,1] - high is good - scale INdependent
    double KAR_mesh         = 0.0;
    double KAR_min          = DBL_MAX;
    double KAR_max          = DBL_MIN;
    double KAR_avg          = 0.0;
    double KAR_poly_min     = DBL_MAX;
    double KAR_poly_max     = DBL_MIN;
    double KAR_poly_avg     = 0.0;
    double KAR_global_avg   = 0.0;
    double KAR_global_norm  = 0.0;
    uint   KAR_min_id       = UINT_MAX;
    uint   KAR_max_id       = UINT_MAX;
    uint   KAR_poly_min_id  = UINT_MAX;
    uint   KAR_poly_max_id  = UINT_MAX;
    std::vector<double> KAR_all;

    // APR - Area/Perimeter Ratio - Range: [0,1] - high is good - scale INdependent
    double APR_mesh         = 0.0;
    double APR_min          = DBL_MAX;
    double APR_max          = DBL_MIN;
    double APR_avg          = 0.0;
    double APR_poly_min     = DBL_MAX;
    double APR_poly_max     = DBL_MIN;
    double APR_poly_avg     = 0.0;
    double APR_global_avg   = 0.0;
    double APR_global_norm  = 0.0;
    uint   APR_min_id       = UINT_MAX;
    uint   APR_max_id       = UINT_MAX;
    uint   APR_poly_min_id  = UINT_MAX;
    uint   APR_poly_max_id  = UINT_MAX;
    std::vector<double> APR_all;

    // MIA - Minimum Angle - Range: [0,1] - high is good - scale INdependent
    double MIA_mesh         = 0.0;
    double MIA_min          = DBL_MAX;
    double MIA_max          = DBL_MIN;
    double MIA_avg          = 0.0;
    double MIA_poly_min     = DBL_MAX;
    double MIA_poly_max     = DBL_MIN;
    double MIA_poly_avg     = 0.0;
    double MIA_global_avg   = 0.0;
    double MIA_global_norm  = 0.0;
    uint   MIA_min_id       = UINT_MAX;
    uint   MIA_max_id       = UINT_MAX;
    uint   MIA_poly_min_id  = UINT_MAX;
    uint   MIA_poly_max_id  = UINT_MAX;
    std::vector<double> MIA_all;

    // MAA - Maximum Angle - Range: [0,1] - high is good - scale INdependent
    double MAA_mesh         = 0.0;
    double MAA_min          = DBL_MAX;
    double MAA_max          = DBL_MIN;
    double MAA_avg          = 0.0;
    double MAA_poly_min     = DBL_MAX;
    double MAA_poly_max     = DBL_MIN;
    double MAA_poly_avg     = 0.0;
    double MAA_global_avg   = 0.0;
    double MAA_global_norm  = 0.0;
    uint   MAA_min_id       = UINT_MAX;
    uint   MAA_max_id       = UINT_MAX;
    uint   MAA_poly_min_id  = UINT_MAX;
    uint   MAA_poly_max_id  = UINT_MAX;
    std::vector<double> MAA_all;

    // ANR - Angle Ratio - Range: [0,1] - high is good - scale INdependent
    double ANR_mesh         = 0.0;
    double ANR_min          = DBL_MAX;
    double ANR_max          = DBL_MIN;
    double ANR_avg          = 0.0;
    double ANR_poly_min     = DBL_MAX;
    double ANR_poly_max     = DBL_MIN;
    double ANR_poly_avg     = 0.0;
    double ANR_global_avg   = 0.0;
    double ANR_global_norm  = 0.0;
    uint   ANR_min_id       = UINT_MAX;
    uint   ANR_max_id       = UINT_MAX;
    uint   ANR_poly_min_id  = UINT_MAX;
    uint   ANR_poly_max_id  = UINT_MAX;
    std::vector<double> ANR_all;

    // VEM - VEM Indicator - Range: [0,1] - high is good - scale INdependent
    double VEM_mesh         = 0.0;
    double VEM_min          = DBL_MAX;
    double VEM_max          = DBL_MIN;
    double VEM_avg          = 0.0;
    double VEM_poly_min     = DBL_MAX;
    double VEM_poly_max     = DBL_MIN;
    double VEM_poly_avg     = 0.0;
    double VEM_global_avg   = 0.0;
    double VEM_global_norm  = 0.0;
    uint   VEM_min_id       = UINT_MAX;
    uint   VEM_max_id       = UINT_MAX;
    uint   VEM_poly_min_id  = UINT_MAX;
    uint   VEM_poly_max_id  = UINT_MAX;
    std::vector<double> VEM_all;

    // JAC - JAC Indicator - Range: [0,1] - high is good - scale INdependent
    double JAC_mesh         = 0.0;
    double JAC_min          = DBL_MAX;
    double JAC_max          = DBL_MIN;
    double JAC_avg          = 0.0;
    double JAC_poly_min     = DBL_MAX;
    double JAC_poly_max     = DBL_MIN;
    double JAC_poly_avg     = 0.0;
    double JAC_global_avg   = 0.0;
    double JAC_global_norm  = 0.0;
    uint   JAC_min_id       = UINT_MAX;
    uint   JAC_max_id       = UINT_MAX;
    uint   JAC_poly_min_id  = UINT_MAX;
    uint   JAC_poly_max_id  = UINT_MAX;
    std::vector<double> JAC_all;

    // FRO - Frobenius Norm - Range: [0,1] - high is good - scale INdependent
    double FRO_mesh         = 0.0;
    double FRO_min          = DBL_MAX;
    double FRO_max          = DBL_MIN;
    double FRO_avg          = 0.0;
    double FRO_poly_min     = DBL_MAX;
    double FRO_poly_max     = DBL_MIN;
    double FRO_poly_avg     = 0.0;
    double FRO_global_avg   = 0.0;
    double FRO_global_norm  = 0.0;
    uint   FRO_min_id       = UINT_MAX;
    uint   FRO_max_id       = UINT_MAX;
    uint   FRO_poly_min_id  = UINT_MAX;
    uint   FRO_poly_max_id  = UINT_MAX;
    std::vector<double> FRO_all;
};

template<typename T>
void get_min_max_avg(std::vector<std::pair<T,uint>> & list,
                     T                              & min,
                     T                              & max,
                     double                         & avg,
                     uint                           & min_id,
                     uint                           & max_id);

void get_all(std::vector<std::pair<double,uint>> & list_triangles, std::vector<std::pair<double,uint>> & list_polys, std::vector<double> & values);

void save_to_file(const char *filename, const MeshMetrics & metrics);

void compute_mesh_metrics(const cinolib::Polygonmesh<> &m, MeshMetrics &metrics);

double compute_metric_INR(const std::vector<cinolib::vec3d> &points);
double compute_metric_OUR(const std::vector<cinolib::vec3d> &points);
double compute_metric_CIR(const std::vector<cinolib::vec3d> &points);
double compute_metric_KRR(const std::vector<cinolib::vec3d> &points);
double compute_metric_KAR(const std::vector<cinolib::vec3d> &points);
double compute_metric_APR(const std::vector<cinolib::vec3d> &points);
double compute_metric_MIA(const std::vector<cinolib::vec3d> &points);
double compute_metric_MAA(const std::vector<cinolib::vec3d> &points);
double compute_metric_ANR(const std::vector<cinolib::vec3d> &points);
double compute_metric_VEM(const std::vector<cinolib::vec3d> &points);
double compute_metric_JAC(const std::vector<cinolib::vec3d> &points);
double compute_metric_FRO(const std::vector<cinolib::vec3d> &points);

#endif
