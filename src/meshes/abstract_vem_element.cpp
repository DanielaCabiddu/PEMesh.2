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

#include "abstract_vem_element.h"
#include <cinolib/triangle_wrap.h>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

AbstractVEMelement::AbstractVEMelement()
{}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

cinolib::Polygonmesh<> AbstractVEMelement::deform_with_canvas(float t, const std::string flags) const
{
    cinolib::Polygonmesh<> m = deform(t);

    // dump the geometry of the element
    //
    std::vector<cinolib::vec2d> verts_in;
    std::vector<uint>  segs;
    for(uint vid=0; vid<m.num_verts(); ++vid)
    {
        verts_in.push_back(cinolib::vec2d(m.vert(vid).x(), m.vert(vid).y()));
    }
    for(uint eid=0; eid<m.num_edges(); ++eid)
    {
        segs.push_back(m.edge_vert_id(eid,0));
        segs.push_back(m.edge_vert_id(eid,1));
    }

    // create a quad around the base element bounding box
    //
    cinolib::vec2d  bot_l = cinolib::vec2d(m.bbox().min.x(), m.bbox().min.y());
    cinolib::vec2d  top_r = cinolib::vec2d(m.bbox().max.x(), m.bbox().max.y());
    double delta = m.bbox().diag();
    verts_in.push_back(cinolib::vec2d(bot_l.x() - delta, bot_l.y() - delta));
    verts_in.push_back(cinolib::vec2d(top_r.x() + delta, bot_l.y() - delta));
    verts_in.push_back(cinolib::vec2d(top_r.x() + delta, top_r.y() + delta));
    verts_in.push_back(cinolib::vec2d(bot_l.x() - delta, top_r.y() + delta));

    // create a hole in correspondence of the deformed element
    // (I'm taking as hole seed the centroid of one of the
    // triangles in the triangulation of the poligon)
    //
    std::vector<cinolib::vec2d> holes;
    std::vector<uint>  tmp = m.poly_tessellation(0);
    cinolib::vec3d sum =(m.vert(tmp.at(0)) + m.vert(tmp.at(1)) + m.vert(tmp.at(2)));
    cinolib::vec2d hole = cinolib::vec2d(sum.x(), sum.y())/3.0;
    holes.push_back(hole);

    std::vector<cinolib::vec3d> verts_out;
    std::vector<uint> tris;
    //std::string flags("cq20.0a");
    std::string t_flags = flags + std::to_string(delta*delta*0.25); // bound max area with PEM element bbox area
    triangle_wrap(verts_in, segs, holes, 0, t_flags.c_str(), verts_out, tris);

    cinolib::Polygonmesh<> m_with_canvas(verts_out, cinolib::polys_from_serialized_vids(tris,3));

    // walk along boundary to fill holes with PEM elements
    uint start_v = 0;
    uint curr_v  = 0;
    std::vector<uint> poly;
    std::set<uint> visited;
    do
    {
        poly.push_back(curr_v);
        bool found = false;
        uint next_v;
        for(uint eid : m_with_canvas.adj_v2e(curr_v))
        {
            if (found || !m_with_canvas.edge_is_boundary(eid)) continue;
            if(cinolib::DOES_NOT_CONTAIN(visited,eid))
            {
                visited.insert(eid);
                assert(found==false);
                found=true;
                next_v = m_with_canvas.vert_opposite_to(eid, curr_v);
            }
        }
        assert(found==true);
        curr_v = next_v;
    }
    while(curr_v!=start_v);
    uint pid = m_with_canvas.poly_add(poly);
    if(m_with_canvas.poly_data(pid).normal.dot(cinolib::vec3d(0,0,1))<0)
    {
        m_with_canvas.poly_remove(pid);
        std::reverse(poly.begin(),poly.end());
        m_with_canvas.poly_add(poly);
    }

    m_with_canvas.normalize_area();
    return m_with_canvas;
}
