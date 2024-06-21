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

#include <float.h>
#include <limits.h>
#include <vector>


using namespace cinolib;

typedef struct
{
    // INR - Inradius/Diameter Ratio - Range: [0,1] - high is good - scale INdependent
    double INR_min    = 0.0;
    double INR_max    = 0.0;
    double INR_avg    = 0.0;
    double INR_poly_min   = 0.0;
    double INR_poly_max   = 0.0;
    double INR_poly_avg   = 0.0;
    double INR_global_avg   = 0.0;
    double INR_global_norm   = 0.0;
    uint   INR_min_id = UINT_MAX;
    uint   INR_max_id = UINT_MAX;
    uint   INR_poly_min_id = UINT_MAX;
    uint   INR_poly_max_id = UINT_MAX;
    std::vector<double> INR_all;

    // OUR - Outradius/Diameter Ratio - Range: [0,1] - high is good - scale INdependent
    double OUR_min    = 0.0;
    double OUR_max    = 0.0;
    double OUR_avg    = 0.0;
    double OUR_poly_min   = 0.0;
    double OUR_poly_max   = 0.0;
    double OUR_poly_avg   = 0.0;
    double OUR_global_avg   = 0.0;
    double OUR_global_norm   = 0.0;
    uint   OUR_min_id = UINT_MAX;
    uint   OUR_max_id = UINT_MAX;
    uint   OUR_poly_min_id = UINT_MAX;
    uint   OUR_poly_max_id = UINT_MAX;
    std::vector<double> OUR_all;

    // CIR - Circle Ratio - Range: [0,1] - high is good - scale INdependent
    double CIR_min    = 0.0;
    double CIR_max    = 0.0;
    double CIR_avg    = 0.0;
    double CIR_poly_min   = 0.0;
    double CIR_poly_max   = 0.0;
    double CIR_poly_avg   = 0.0;
    double CIR_global_avg   = 0.0;
    double CIR_global_norm   = 0.0;
    uint   CIR_min_id = UINT_MAX;
    uint   CIR_max_id = UINT_MAX;
    uint   CIR_poly_min_id = UINT_MAX;
    uint   CIR_poly_max_id = UINT_MAX;
    std::vector<double> CIR_all;

    // KRR - Kernel Radius Ratio - Range: [0,1] - high is good - scale INdependent
    double KRR_min    = 0.0;
    double KRR_max    = 0.0;
    double KRR_avg    = 0.0;
    double KRR_poly_min   = 0.0;
    double KRR_poly_max   = 0.0;
    double KRR_poly_avg   = 0.0;
    double KRR_global_avg   = 0.0;
    double KRR_global_norm   = 0.0;
    uint   KRR_min_id = UINT_MAX;
    uint   KRR_max_id = UINT_MAX;
    uint   KRR_poly_min_id = UINT_MAX;
    uint   KRR_poly_max_id = UINT_MAX;
    std::vector<double> KRR_all;

    // KAR - Kernel Area Ratio - Range: [0,1] - high is good - scale INdependent
    double KAR_min    = 0.0;
    double KAR_max    = 0.0;
    double KAR_avg    = 0.0;
    double KAR_poly_min   = 0.0;
    double KAR_poly_max   = 0.0;
    double KAR_poly_avg   = 0.0;
    double KAR_global_avg   = 0.0;
    double KAR_global_norm   = 0.0;
    uint   KAR_min_id = UINT_MAX;
    uint   KAR_max_id = UINT_MAX;
    uint   KAR_poly_min_id = UINT_MAX;
    uint   KAR_poly_max_id = UINT_MAX;
    std::vector<double> KAR_all;

    // APR - Area/Perimeter Ratio - Range: [0,1] - high is good - scale INdependent
    double APR_min    = 0.0;
    double APR_max    = 0.0;
    double APR_avg    = 0.0;
    double APR_poly_min   = 0.0;
    double APR_poly_max   = 0.0;
    double APR_poly_avg   = 0.0;
    double APR_global_avg   = 0.0;
    double APR_global_norm   = 0.0;
    uint   APR_min_id = UINT_MAX;
    uint   APR_max_id = UINT_MAX;
    uint   APR_poly_min_id = UINT_MAX;
    uint   APR_poly_max_id = UINT_MAX;
    std::vector<double> APR_all;

    // MIA - Minimum Angle - Range: [0,1] - high is good - scale INdependent
    double MIA_min    = 0.0;
    double MIA_max    = 0.0;
    double MIA_avg    = 0.0;
    double MIA_poly_min   = 0.0;
    double MIA_poly_max   = 0.0;
    double MIA_poly_avg   = 0.0;
    double MIA_global_avg   = 0.0;
    double MIA_global_norm   = 0.0;
    uint   MIA_min_id = UINT_MAX;
    uint   MIA_max_id = UINT_MAX;
    uint   MIA_poly_min_id = UINT_MAX;
    uint   MIA_poly_max_id = UINT_MAX;
    std::vector<double> MIA_all;

    // MAA - Maximum Angle - Range: [0,1] - high is good - scale INdependent
    double MAA_min    = 0.0;
    double MAA_max    = 0.0;
    double MAA_avg    = 0.0;
    double MAA_poly_min   = 0.0;
    double MAA_poly_max   = 0.0;
    double MAA_poly_avg   = 0.0;
    double MAA_global_avg   = 0.0;
    double MAA_global_norm   = 0.0;
    uint   MAA_min_id = UINT_MAX;
    uint   MAA_max_id = UINT_MAX;
    uint   MAA_poly_min_id = UINT_MAX;
    uint   MAA_poly_max_id = UINT_MAX;
    std::vector<double> MAA_all;

    // ANR - Angle Ratio - Range: [0,1] - high is good - scale INdependent
    double ANR_min    = 0.0;
    double ANR_max    = 0.0;
    double ANR_avg    = 0.0;
    double ANR_poly_min   = 0.0;
    double ANR_poly_max   = 0.0;
    double ANR_poly_avg   = 0.0;
    double ANR_global_avg   = 0.0;
    double ANR_global_norm   = 0.0;
    uint   ANR_min_id = UINT_MAX;
    uint   ANR_max_id = UINT_MAX;
    uint   ANR_poly_min_id = UINT_MAX;
    uint   ANR_poly_max_id = UINT_MAX;
    std::vector<double> ANR_all;

    // VEM - VEM Indicator - Range: [0,1] - high is good - scale INdependent
    double VEM_min       = 0.0;
    double VEM_max       = 0.0;
    double VEM_avg       = 0.0;
    double VEM_poly_min  = 0.0;
    double VEM_poly_max  = 0.0;
    double VEM_poly_avg  = 0.0;
    double VEM_global_avg   = 0.0;
    double VEM_global_norm   = 0.0;
    uint   VEM_min_id    = UINT_MAX;
    uint   VEM_max_id    = UINT_MAX;
    uint   VEM_poly_min_id = UINT_MAX;
    uint   VEM_poly_max_id = UINT_MAX;
    std::vector<double> VEM_all;

    // JAC - JAC Indicator - Range: [0,1] - high is good - scale INdependent
    double JAC_min       = 0.0;
    double JAC_max       = 0.0;
    double JAC_avg       = 0.0;
    double JAC_poly_min  = 0.0;
    double JAC_poly_max  = 0.0;
    double JAC_poly_avg  = 0.0;
    double JAC_global_avg   = 0.0;
    double JAC_global_norm   = 0.0;
    uint   JAC_min_id    = UINT_MAX;
    uint   JAC_max_id    = UINT_MAX;
    uint   JAC_poly_min_id = UINT_MAX;
    uint   JAC_poly_max_id = UINT_MAX;
    std::vector<double> JAC_all;

    // FRO - Frobenius Ratio - Range: [0,1] - high is good - scale INdependent
    double FRO_min       = 0.0;
    double FRO_max       = 0.0;
    double FRO_avg       = 0.0;
    double FRO_poly_min  = 0.0;
    double FRO_poly_max  = 0.0;
    double FRO_poly_avg  = 0.0;
    double FRO_global_avg   = 0.0;
    double FRO_global_norm   = 0.0;
    uint   FRO_min_id    = UINT_MAX;
    uint   FRO_max_id    = UINT_MAX;
    uint   FRO_poly_min_id = UINT_MAX;
    uint   FRO_poly_max_id = UINT_MAX;
    std::vector<double> FRO_all;
}
MeshMetrics;

template<typename T>
void get_min_max_avg(std::vector<std::pair<T,uint>> & list,
                     T                              & min,
                     T                              & max,
                     double                         & avg,
                     uint                           & min_id,
                     uint                           & max_id);

void get_all(std::vector<std::pair<double,uint>> & list_triangles, std::vector<std::pair<double,uint>> & list_polys, std::vector<double> & values);

void save_to_file(const char *filename, const MeshMetrics & metrics);

void compute_mesh_metrics(const Polygonmesh<> &m, MeshMetrics &metrics);

inline double compute_metric_INR(const std::vector<vec3d> &points);
inline double compute_metric_OUR(const std::vector<vec3d> &points);
inline double compute_metric_CIR(const std::vector<vec3d> &points);
inline double compute_metric_KRR(const std::vector<vec3d> &points);
inline double compute_metric_KAR(const std::vector<vec3d> &points);
inline double compute_metric_APR(const std::vector<vec3d> &points);
inline double compute_metric_MIA(const std::vector<vec3d> &points);
inline double compute_metric_MAA(const std::vector<vec3d> &points);
inline double compute_metric_ANR(const std::vector<vec3d> &points);
inline double compute_metric_VEM(const std::vector<vec3d> &points);
inline double compute_metric_JAC(const std::vector<vec3d> &points);
inline double compute_metric_FRO(const std::vector<vec3d> &points);

#endif
