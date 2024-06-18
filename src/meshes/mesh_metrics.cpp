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

#include "mesh_metrics.h"

#include <cinolib/polygon_kernel.h>
#include <cinolib/polygon_maximum_inscribed_circle.h>
#include <cinolib/smallest_enclosing_disk.h>


template<typename T>
void get_min_max_avg(std::vector<std::pair<T,uint>> & list,
                     T                              & min,
                     T                              & max,
                     double                         & avg,
                     uint                           & min_id,
                     uint                           & max_id)
{
    if (list.empty()) return;

    sort(list.begin(), list.end());
    min    = list.front().first;
    min_id = list.front().second;
    max    = list.back().first;
    max_id = list.back().second;
    avg    = 0.0;
    for(auto item : list) avg += item.first;
    avg/=static_cast<double>(list.size());
}

template<typename T>
void get_global_avg_norm (const std::vector<std::pair<T,uint>> & list1,
                          const std::vector<std::pair<T,uint>> & list2,
                                double &avg,
                                double &norm)
{
    avg  = 0.0;
    norm = 0.0;

    for(auto item : list1) avg += item.first;
    for(auto item : list2) avg += item.first;

    for(auto item : list1) norm += item.first * item.first;
    for(auto item : list2) norm += item.first * item.first;

    avg/=static_cast<double>(list1.size() + list2.size());

    norm/=static_cast<double>(list1.size() + list2.size());

    norm = sqrt (norm);

}

void get_all(std::vector<std::pair<double,uint>> & list_triangles, std::vector<std::pair<double,uint>> & list_polys, std::vector<double> & values) {
    values.resize(list_triangles.size() + list_polys.size());
    for(auto i : list_triangles) {
        values.at(i.second) = i.first;
    }
    for(auto i : list_polys) {
        values.at(i.second) = i.first;
    }
}

template<typename T>
void get_sum(std::vector<std::pair<T,uint>> & list, double & sum)
{
    if (list.empty()) return;
    sum = 0.0;
    for(auto item : list) sum += item.first;
}

void save_to_file(const char *filename, const MeshMetrics & metrics)
{
    FILE *f = fopen(filename, "w");
    if(f)
    {
        fprintf(f, "INR_min %f %d\n" , metrics.INR_min, metrics.INR_min_id);
        fprintf(f, "INR_max %f %d\n" , metrics.INR_max, metrics.INR_max_id);
        fprintf(f, "INR_avg %f\n"    , metrics.INR_avg);
        fprintf(f, "INR_poly_min %f %d\n" , metrics.INR_poly_min, metrics.INR_poly_min_id);
        fprintf(f, "INR_poly_max %f %d\n" , metrics.INR_poly_max, metrics.INR_poly_max_id);
        fprintf(f, "INR_poly_avg %f\n"    , metrics.INR_poly_avg);
        fprintf(f, "INR_global_avg %f\n"    , metrics.INR_global_avg);
        fprintf(f, "INR_global_norm %f\n"   , metrics.INR_global_norm);

        fprintf(f, "OUR_min %f %d\n" , metrics.OUR_min, metrics.OUR_min_id);
        fprintf(f, "OUR_max %f %d\n" , metrics.OUR_max, metrics.OUR_max_id);
        fprintf(f, "OUR_avg %f\n"    , metrics.OUR_avg);
        fprintf(f, "OUR_poly_min %f %d\n" , metrics.OUR_poly_min, metrics.OUR_poly_min_id);
        fprintf(f, "OUR_poly_max %f %d\n" , metrics.OUR_poly_max, metrics.OUR_poly_max_id);
        fprintf(f, "OUR_poly_avg %f\n"    , metrics.OUR_poly_avg);
        fprintf(f, "OUR_global_avg %f\n"    , metrics.OUR_global_avg);
        fprintf(f, "OUR_global_norm %f\n"   , metrics.OUR_global_norm);

        fprintf(f, "CIR_min %f %d\n" , metrics.CIR_min, metrics.CIR_min_id);
        fprintf(f, "CIR_max %f %d\n" , metrics.CIR_max, metrics.CIR_max_id);
        fprintf(f, "CIR_avg %f\n"    , metrics.CIR_avg);
        fprintf(f, "CIR_poly_min %f %d\n" , metrics.CIR_poly_min, metrics.CIR_poly_min_id);
        fprintf(f, "CIR_poly_max %f %d\n" , metrics.CIR_poly_max, metrics.CIR_poly_max_id);
        fprintf(f, "CIR_poly_avg %f\n"    , metrics.CIR_poly_avg);
        fprintf(f, "CIR_global_avg %f\n"    , metrics.CIR_global_avg);
        fprintf(f, "CIR_global_norm %f\n"   , metrics.CIR_global_norm);

        fprintf(f, "KRR_min %f %d\n" , metrics.KRR_min, metrics.KRR_min_id);
        fprintf(f, "KRR_max %f %d\n" , metrics.KRR_max, metrics.KRR_max_id);
        fprintf(f, "KRR_avg %f\n"    , metrics.KRR_avg);
        fprintf(f, "KRR_poly_min %f %d\n" , metrics.KRR_poly_min, metrics.KRR_poly_min_id);
        fprintf(f, "KRR_poly_max %f %d\n" , metrics.KRR_poly_max, metrics.KRR_poly_max_id);
        fprintf(f, "KRR_poly_avg %f\n"    , metrics.KRR_poly_avg);
        fprintf(f, "KRR_global_avg %f\n"    , metrics.KRR_global_avg);
        fprintf(f, "KRR_global_norm %f\n"   , metrics.KRR_global_norm);

        fprintf(f, "KAR_min %f %d\n", metrics.KAR_min, metrics.KAR_min_id);
        fprintf(f, "KAR_max %f %d\n", metrics.KAR_max, metrics.KAR_max_id);
        fprintf(f, "KAR_avg %f\n"   , metrics.KAR_avg);
        fprintf(f, "KAR_poly_min %f %d\n", metrics.KAR_poly_min, metrics.KAR_poly_min_id);
        fprintf(f, "KAR_poly_max %f %d\n", metrics.KAR_poly_max, metrics.KAR_poly_max_id);
        fprintf(f, "KAR_poly_avg %f\n"   , metrics.KAR_poly_avg);
        fprintf(f, "KAR_global_avg %f\n"    , metrics.KAR_global_avg);
        fprintf(f, "KAR_global_norm %f\n"   , metrics.KAR_global_norm);

        fprintf(f, "APR_min %f %d\n", metrics.APR_min, metrics.APR_min_id);
        fprintf(f, "APR_max %f %d\n", metrics.APR_max, metrics.APR_max_id);
        fprintf(f, "APR_avg %f\n"   , metrics.APR_avg);
        fprintf(f, "APR_poly_min %f %d\n", metrics.APR_poly_min, metrics.APR_poly_min_id);
        fprintf(f, "APR_poly_max %f %d\n", metrics.APR_poly_max, metrics.APR_poly_max_id);
        fprintf(f, "APR_poly_avg %f\n"   , metrics.APR_poly_avg);
        fprintf(f, "APR_global_avg %f\n"    , metrics.APR_global_avg);
        fprintf(f, "APR_global_norm %f\n"   , metrics.APR_global_norm);

        fprintf(f, "MIA_min %f %d\n" , metrics.MIA_min, metrics.MIA_min_id);
        fprintf(f, "MIA_max %f %d\n" , metrics.MIA_max, metrics.MIA_max_id);
        fprintf(f, "MIA_avg %f\n"    , metrics.MIA_avg);
        fprintf(f, "MIA_poly_min %f %d\n" , metrics.MIA_poly_min, metrics.MIA_poly_min_id);
        fprintf(f, "MIA_poly_max %f %d\n" , metrics.MIA_poly_max, metrics.MIA_poly_max_id);
        fprintf(f, "MIA_poly_avg %f\n"    , metrics.MIA_poly_avg);
        fprintf(f, "MIA_global_avg %f\n"    , metrics.MIA_global_avg);
        fprintf(f, "MIA_global_norm %f\n"   , metrics.MIA_global_norm);

        fprintf(f, "MAA_min %f %d\n" , metrics.MAA_min, metrics.MAA_min_id);
        fprintf(f, "MAA_max %f %d\n" , metrics.MAA_max, metrics.MAA_max_id);
        fprintf(f, "MAA_avg %f\n"    , metrics.MAA_avg);
        fprintf(f, "MAA_poly_min %f %d\n" , metrics.MAA_poly_min, metrics.MAA_poly_min_id);
        fprintf(f, "MAA_poly_max %f %d\n" , metrics.MAA_poly_max, metrics.MAA_poly_max_id);
        fprintf(f, "MAA_poly_avg %f\n"    , metrics.MAA_poly_avg);
        fprintf(f, "MAA_global_avg %f\n"    , metrics.MAA_global_avg);
        fprintf(f, "MAA_global_norm %f\n"   , metrics.MAA_global_norm);

        fprintf(f, "ANR_min %f %d\n", metrics.ANR_min, metrics.ANR_min_id);
        fprintf(f, "ANR_max %f %d\n", metrics.ANR_max, metrics.ANR_max_id);
        fprintf(f, "ANR_avg %f\n"   , metrics.ANR_avg);
        fprintf(f, "ANR_poly_min %f %d\n", metrics.ANR_poly_min, metrics.ANR_poly_min_id);
        fprintf(f, "ANR_poly_max %f %d\n", metrics.ANR_poly_max, metrics.ANR_poly_max_id);
        fprintf(f, "ANR_poly_avg %f\n"   , metrics.ANR_poly_avg);
        fprintf(f, "ANR_global_avg %f\n"    , metrics.ANR_global_avg);
        fprintf(f, "ANR_global_norm %f\n"   , metrics.ANR_global_norm);

        fprintf(f, "VEM_min %f %d\n", metrics.VEM_min, metrics.VEM_min_id);
        fprintf(f, "VEM_max %f %d\n", metrics.VEM_max, metrics.VEM_max_id);
        fprintf(f, "VEM_avg %f\n"   , metrics.VEM_avg);
        fprintf(f, "VEM_poly_min %f %d\n", metrics.VEM_poly_min, metrics.VEM_poly_min_id);
        fprintf(f, "VEM_poly_max %f %d\n", metrics.VEM_poly_max, metrics.VEM_poly_max_id);
        fprintf(f, "VEM_poly_avg %f\n"   , metrics.VEM_poly_avg);
        fprintf(f, "VEM_global_avg %f\n"    , metrics.VEM_global_avg);
        fprintf(f, "VEM_global_norm %f\n"   , metrics.VEM_global_norm);

        fprintf(f, "JAC_min %f %d\n", metrics.JAC_min, metrics.JAC_min_id);
        fprintf(f, "JAC_max %f %d\n", metrics.JAC_max, metrics.JAC_max_id);
        fprintf(f, "JAC_avg %f\n"   , metrics.JAC_avg);
        fprintf(f, "JAC_poly_min %f %d\n", metrics.JAC_poly_min, metrics.JAC_poly_min_id);
        fprintf(f, "JAC_poly_max %f %d\n", metrics.JAC_poly_max, metrics.JAC_poly_max_id);
        fprintf(f, "JAC_poly_avg %f\n"   , metrics.JAC_poly_avg);
        fprintf(f, "JAC_global_avg %f\n"    , metrics.JAC_global_avg);
        fprintf(f, "JAC_global_norm %f\n"   , metrics.JAC_global_norm);

        fprintf(f, "FRO_min %f %d\n", metrics.FRO_min, metrics.FRO_min_id);
        fprintf(f, "FRO_max %f %d\n", metrics.FRO_max, metrics.FRO_max_id);
        fprintf(f, "FRO_avg %f\n"   , metrics.FRO_avg);
        fprintf(f, "FRO_poly_min %f %d\n", metrics.FRO_poly_min, metrics.FRO_poly_min_id);
        fprintf(f, "FRO_poly_max %f %d\n", metrics.FRO_poly_max, metrics.FRO_poly_max_id);
        fprintf(f, "FRO_poly_avg %f\n"   , metrics.FRO_poly_avg);
        fprintf(f, "FRO_global_avg %f\n"    , metrics.FRO_global_avg);
        fprintf(f, "FRO_global_norm %f\n"   , metrics.FRO_global_norm);

        fclose(f);
    }
}

void compute_mesh_metrics(const Polygonmesh<> &m, MeshMetrics &metrics)
{
    // convenient compact representations for each item
    std::vector<std::pair<double,uint>> INR;
    std::vector<std::pair<double,uint>> OUR;
    std::vector<std::pair<double,uint>> CIR;
    std::vector<std::pair<double,uint>> KRR;
    std::vector<std::pair<double,uint>> KAR;
    std::vector<std::pair<double,uint>> APR;
    std::vector<std::pair<double,uint>> MIA;
    std::vector<std::pair<double,uint>> MAA;
    std::vector<std::pair<double,uint>> ANR;
    std::vector<std::pair<double,uint>> VEM;
    std::vector<std::pair<double,uint>> JAC;
    std::vector<std::pair<double,uint>> FRO;

    std::vector<std::pair<double,uint>> INR_poly;
    std::vector<std::pair<double,uint>> OUR_poly;
    std::vector<std::pair<double,uint>> CIR_poly;
    std::vector<std::pair<double,uint>> KRR_poly;
    std::vector<std::pair<double,uint>> KAR_poly;
    std::vector<std::pair<double,uint>> APR_poly;
    std::vector<std::pair<double,uint>> MIA_poly;
    std::vector<std::pair<double,uint>> MAA_poly;
    std::vector<std::pair<double,uint>> ANR_poly;
    std::vector<std::pair<double,uint>> VEM_poly;
    std::vector<std::pair<double,uint>> JAC_poly;
    std::vector<std::pair<double,uint>> FRO_poly;

    for(uint pid=0; pid<m.num_polys(); ++pid)
    {
        //std::cout << "[pid " << pid << "] " << std::endl;

        std::vector<vec3d> points = m.poly_verts(pid);
        bool is_triangle = (points.size() == 3);
        double TOLL = 1e-6;

        std::vector<double> e;
        for(uint eid : m.adj_p2e(pid)) { e.push_back(m.edge_length(eid)); }
        double min_edge = *std::min_element(e.begin(), e.end());

        vec3d  dummy;
        double inradius, outradius;
        polygon_maximum_inscribed_circle(points, dummy, inradius);
        smallest_enclosing_disk(points, dummy, outradius);
        assert(inradius < outradius && outradius > 0.);
        double diameter = DBL_MIN;
        for(uint i=0; i<points.size()-1; ++i) {
            for(uint j=i+1; j<points.size(); ++j) {
                diameter = std::max(diameter, points.at(i).dist(points.at(j)));
            }
        }
        assert(diameter > 0.);
        double inr = (2. * inradius) / diameter;
        double our = diameter / (2. * outradius);
        double cir = inradius / outradius;

        std::vector<vec3d> kernel_verts;
        double kernel_area = polygon_kernel(points, kernel_verts);
        double kernel_inradius = 0.;
        if (!kernel_verts.empty()) { polygon_maximum_inscribed_circle(kernel_verts, dummy, kernel_inradius); }
        double area = m.poly_area(pid);
        assert(area > 0.);
        double perimeter  = std::accumulate(e.begin(), e.end(), 0.);
        assert(perimeter > 0.);
        double krr = kernel_inradius / outradius;
        double kar = kernel_area / area;
        if (0 < kernel_area - area && kernel_area - area < TOLL) { kar = 1.; }
        double apr = sqrt(area) / perimeter;

        std::vector<double> angles;
        for(uint vid : m.adj_p2v(pid)) { angles.push_back(m.poly_angle_at_vert(pid, vid, RAD)); }
        double mia = *std::min_element(angles.begin(), angles.end()) / (2. * M_PI);
        double maa = *std::max_element(angles.begin(), angles.end()) / (2. * M_PI);
        assert(mia <= maa && maa != 0.);
        double anr = mia / maa;

        double rho1 = kernel_area / area;
        double rho2 = std::min(sqrt(area), min_edge) / diameter;
        double rho3 = 3. / points.size();
        double rho4 = DBL_MAX;
        std::vector<std::vector<vec3d>> tau;
        std::vector<vec3d> tau_i = {points.front()};
        for (uint i = 0; i < points.size(); ++i) {
            vec3d p0 = points.at(i);
            vec3d p1 = points.at((i + 1) % points.size());
            vec3d p2 = points.at((i + 2) % points.size());
            while (points_are_colinear_3d(p0, p1, p2)) {
                tau_i.push_back(p1);
                i++;
                p1 = points.at((i + 1) % points.size());
                p2 = points.at((i + 2) % points.size());
            }
            tau_i.push_back(p1);
            tau.push_back(tau_i);
            tau_i = {p1};
        }
        for (const std::vector<vec3d> &tau_i : tau) {
            double min_ei = DBL_MAX;
            double max_ei = DBL_MIN;
            for (uint vid = 0; vid < tau_i.size() - 1; vid++) {
                double t = tau_i.at(vid).dist(tau_i.at(vid + 1));
                min_ei = std::min(min_ei, t);
                max_ei = std::max(max_ei, t);
            }
            assert(max_ei > 0.);
            rho4 = std::min(rho4, min_ei / max_ei);
        }
        double vem = sqrt((rho1 * rho2 + rho1 * rho3 + rho1 * rho4) / 3.);

        double jac = DBL_MAX;
        double fro = DBL_MAX;
        std::vector<vec2d> points_2d = vec2d_from_vec3d(points);
        for (uint i = 0; i < points_2d.size(); ++i) {
            vec2d p0 = points_2d.at(i);
            vec2d p1 = points_2d.at((i + 1) % points_2d.size());
            vec2d p2 = points_2d.at((i + 2) % points_2d.size());

            vec2d L0 = p1 - p0;
            vec2d L1 = p2 - p0;

            mat<2,2,double> A;
            A.set_col(0, L0);
            A.set_col(1, L1);
            double J = A.det();
            double lambda = L0.norm() * L1.norm();
            jac = std::min(jac, J / lambda);

            mat<2,2,double> inv_A = A.inverse();
            mat<2,2,double> tsp_A = A.transpose();
            mat<2,2,double> tsp_inv_A = inv_A.transpose();

            double k = sqrt((tsp_A * A).trace() * (tsp_inv_A * inv_A).trace());
            fro = std::min(fro, 2. / k);
        }
        if (jac < 0.) jac = 0.;

        assert(0. <= inr && inr <= 1. + TOLL);
        assert(0. <= our && our <= 1. + TOLL);
        assert(0. <= cir && cir <= 1. + TOLL);
        assert(0. <= krr && krr <= 1. + TOLL);
        assert(0. <= kar && kar <= 1. + TOLL);
        assert(0. <= apr && apr <= 1. + TOLL);
        assert(0. <= mia && mia <= 1. + TOLL);
        assert(0. <= maa && maa <= 1. + TOLL);
        assert(0. <= anr && anr <= 1. + TOLL);
        assert(0. <= vem && vem <= 1. + TOLL);
        assert(0. <= jac && jac <= 1. + TOLL);
        assert(0. <= fro && fro <= 1. + TOLL);

        if (is_triangle)
        {
            INR.push_back(std::make_pair(inr,pid));
            OUR.push_back(std::make_pair(our,pid));
            CIR.push_back(std::make_pair(cir,pid));
            KRR.push_back(std::make_pair(krr,pid));
            KAR.push_back(std::make_pair(kar,pid));
            APR.push_back(std::make_pair(apr,pid));
            MIA.push_back(std::make_pair(mia,pid));
            MAA.push_back(std::make_pair(maa,pid));
            ANR.push_back(std::make_pair(anr,pid));
            VEM.push_back(std::make_pair(vem,pid));
            JAC.push_back(std::make_pair(jac,pid));
            FRO.push_back(std::make_pair(fro,pid));
        }
        else
        {
            INR_poly.push_back(std::make_pair(inr,pid));
            OUR_poly.push_back(std::make_pair(our,pid));
            CIR_poly.push_back(std::make_pair(cir,pid));
            KRR_poly.push_back(std::make_pair(krr,pid));
            KAR_poly.push_back(std::make_pair(kar,pid));
            APR_poly.push_back(std::make_pair(apr,pid));
            MIA_poly.push_back(std::make_pair(mia,pid));
            MAA_poly.push_back(std::make_pair(maa,pid));
            ANR_poly.push_back(std::make_pair(anr,pid));
            VEM_poly.push_back(std::make_pair(vem,pid));
            JAC_poly.push_back(std::make_pair(jac,pid));
            FRO_poly.push_back(std::make_pair(fro,pid));
        }
    }

    //std::cout << "[getting min max avg] " << std::endl;

    get_min_max_avg(INR, metrics.INR_min, metrics.INR_max, metrics.INR_avg, metrics.INR_min_id, metrics.INR_max_id);
    get_min_max_avg(OUR, metrics.OUR_min, metrics.OUR_max, metrics.OUR_avg, metrics.OUR_min_id, metrics.OUR_max_id);
    get_min_max_avg(CIR, metrics.CIR_min, metrics.CIR_max, metrics.CIR_avg, metrics.CIR_min_id, metrics.CIR_max_id);
    get_min_max_avg(KRR, metrics.KRR_min, metrics.KRR_max, metrics.KRR_avg, metrics.KRR_min_id, metrics.KRR_max_id);
    get_min_max_avg(KAR, metrics.KAR_min, metrics.KAR_max, metrics.KAR_avg, metrics.KAR_min_id, metrics.KAR_max_id);
    get_min_max_avg(APR, metrics.APR_min, metrics.APR_max, metrics.APR_avg, metrics.APR_min_id, metrics.APR_max_id);
    get_min_max_avg(MIA, metrics.MIA_min, metrics.MIA_max, metrics.MIA_avg, metrics.MIA_min_id, metrics.MIA_max_id);
    get_min_max_avg(MAA, metrics.MAA_min, metrics.MAA_max, metrics.MAA_avg, metrics.MAA_min_id, metrics.MAA_max_id);
    get_min_max_avg(ANR, metrics.ANR_min, metrics.ANR_max, metrics.ANR_avg, metrics.ANR_min_id, metrics.ANR_max_id);
    get_min_max_avg(VEM, metrics.VEM_min, metrics.VEM_max, metrics.VEM_avg, metrics.VEM_min_id, metrics.VEM_max_id);
    get_min_max_avg(JAC, metrics.JAC_min, metrics.JAC_max, metrics.JAC_avg, metrics.JAC_min_id, metrics.JAC_max_id);
    get_min_max_avg(FRO, metrics.FRO_min, metrics.FRO_max, metrics.FRO_avg, metrics.FRO_min_id, metrics.FRO_max_id);

    get_min_max_avg(INR_poly, metrics.INR_poly_min, metrics.INR_poly_max, metrics.INR_poly_avg, metrics.INR_poly_min_id, metrics.INR_poly_max_id);
    get_min_max_avg(OUR_poly, metrics.OUR_poly_min, metrics.OUR_poly_max, metrics.OUR_poly_avg, metrics.OUR_poly_min_id, metrics.OUR_poly_max_id);
    get_min_max_avg(CIR_poly, metrics.CIR_poly_min, metrics.CIR_poly_max, metrics.CIR_poly_avg, metrics.CIR_poly_min_id, metrics.CIR_poly_max_id);
    get_min_max_avg(KRR_poly, metrics.KRR_poly_min, metrics.KRR_poly_max, metrics.KRR_poly_avg, metrics.KRR_poly_min_id, metrics.KRR_poly_max_id);
    get_min_max_avg(KAR_poly, metrics.KAR_poly_min, metrics.KAR_poly_max, metrics.KAR_poly_avg, metrics.KAR_poly_min_id, metrics.KAR_poly_max_id);
    get_min_max_avg(APR_poly, metrics.APR_poly_min, metrics.APR_poly_max, metrics.APR_poly_avg, metrics.APR_poly_min_id, metrics.APR_poly_max_id);
    get_min_max_avg(MIA_poly, metrics.MIA_poly_min, metrics.MIA_poly_max, metrics.MIA_poly_avg, metrics.MIA_poly_min_id, metrics.MIA_poly_max_id);
    get_min_max_avg(MAA_poly, metrics.MAA_poly_min, metrics.MAA_poly_max, metrics.MAA_poly_avg, metrics.MAA_poly_min_id, metrics.MAA_poly_max_id);
    get_min_max_avg(ANR_poly, metrics.ANR_poly_min, metrics.ANR_poly_max, metrics.ANR_poly_avg, metrics.ANR_poly_min_id, metrics.ANR_poly_max_id);
    get_min_max_avg(VEM_poly, metrics.VEM_poly_min, metrics.VEM_poly_max, metrics.VEM_poly_avg, metrics.VEM_poly_min_id, metrics.VEM_poly_max_id);
    get_min_max_avg(JAC_poly, metrics.JAC_poly_min, metrics.JAC_poly_max, metrics.JAC_poly_avg, metrics.JAC_poly_min_id, metrics.JAC_poly_max_id);
    get_min_max_avg(FRO_poly, metrics.FRO_poly_min, metrics.FRO_poly_max, metrics.FRO_poly_avg, metrics.FRO_poly_min_id, metrics.FRO_poly_max_id);

    get_global_avg_norm(INR, INR_poly, metrics.INR_global_avg, metrics.INR_global_norm);
    get_global_avg_norm(OUR, OUR_poly, metrics.OUR_global_avg, metrics.OUR_global_norm);
    get_global_avg_norm(CIR, CIR_poly, metrics.CIR_global_avg, metrics.CIR_global_norm);
    get_global_avg_norm(KRR, KRR_poly, metrics.KRR_global_avg, metrics.KRR_global_norm);
    get_global_avg_norm(KAR, KAR_poly, metrics.KAR_global_avg, metrics.KAR_global_norm);
    get_global_avg_norm(APR, APR_poly, metrics.APR_global_avg, metrics.APR_global_norm);
    get_global_avg_norm(MIA, MIA_poly, metrics.MIA_global_avg, metrics.MIA_global_norm);
    get_global_avg_norm(MAA, MAA_poly, metrics.MAA_global_avg, metrics.MAA_global_norm);
    get_global_avg_norm(ANR, ANR_poly, metrics.ANR_global_avg, metrics.ANR_global_norm);
    get_global_avg_norm(VEM, VEM_poly, metrics.VEM_global_avg, metrics.VEM_global_norm);
    get_global_avg_norm(JAC, JAC_poly, metrics.JAC_global_avg, metrics.JAC_global_norm);
    get_global_avg_norm(FRO, FRO_poly, metrics.FRO_global_avg, metrics.FRO_global_norm);

    get_all(INR, INR_poly, metrics.INR_all);
    get_all(OUR, OUR_poly, metrics.OUR_all);
    get_all(CIR, CIR_poly, metrics.CIR_all);
    get_all(KRR, KRR_poly, metrics.KRR_all);
    get_all(KAR, KAR_poly, metrics.KAR_all);
    get_all(APR, APR_poly, metrics.APR_all);
    get_all(MIA, MIA_poly, metrics.MIA_all);
    get_all(MAA, MAA_poly, metrics.MAA_all);
    get_all(ANR, ANR_poly, metrics.ANR_all);
    get_all(VEM, VEM_poly, metrics.VEM_all);
    get_all(JAC, JAC_poly, metrics.JAC_all);
    get_all(FRO, FRO_poly, metrics.FRO_all);
}
