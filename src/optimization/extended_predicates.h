/********************************************************************************
*  This file is part of CinoLib                                                 *
*  Copyright(C) 2016: Marco Livesu                                              *
*                                                                               *
*  The MIT License                                                              *
*                                                                               *
*  Permission is hereby granted, free of charge, to any person obtaining a      *
*  copy of this software and associated documentation files (the "Software"),   *
*  to deal in the Software without restriction, including without limitation    *
*  the rights to use, copy, modify, merge, publish, distribute, sublicense,     *
*  and/or sell copies of the Software, and to permit persons to whom the        *
*  Software is furnished to do so, subject to the following conditions:         *
*                                                                               *
*  The above copyright notice and this permission notice shall be included in   *
*  all copies or substantial portions of the Software.                          *
*                                                                               *
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR   *
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,     *
*  FITNESS FOR A PARTICULAR PURPOSE AND NON INFRINGEMENT. IN NO EVENT SHALL THE *
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER       *
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING      *
*  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS *
*  IN THE SOFTWARE.                                                             *
*                                                                               *
*  Author(s):                                                                   *
*                                                                               *
*     Marco Livesu (marco.livesu@gmail.com)                                     *
*     http://pers.ge.imati.cnr.it/livesu/                                       *
*                                                                               *
*     Italian National Research Council (CNR)                                   *
*     Institute for Applied Mathematics and Information Technologies (IMATI)    *
*     Via de Marini, 6                                                          *
*     16149 Genoa,                                                              *
*     Italy                                                                     *
*********************************************************************************/
#ifndef EXTENDED_PREDICATES
#define EXTENDED_PREDICATES

#include <cinolib/geometry/vec_mat.h>
#include <cinolib/predicates.h>

namespace cinolib {

// true if the area of all the orthogonal 2d projections of the triangle p0-p1-p2 is smaller than toll
CINO_INLINE
bool points_are_colinear_3d_toll(const vec3d & p0,
                                 const vec3d & p1,
                                 const vec3d & p2,
                                 const double toll)
{
    vec2d p0_dropX(p0[1], p0[2]);
    vec2d p0_dropY(p0[0], p0[2]);
    vec2d p0_dropZ(p0[0], p0[1]);
    vec2d p1_dropX(p1[1], p1[2]);
    vec2d p1_dropY(p1[0], p1[2]);
    vec2d p1_dropZ(p1[0], p1[1]);
    vec2d p2_dropX(p2[1], p2[2]);
    vec2d p2_dropY(p2[0], p2[2]);
    vec2d p2_dropZ(p2[0], p2[1]);

    // check if all the 2d orthogonal projections of p are colinear
    bool Dx = fabs(orient2d(p0_dropX, p1_dropX, p2_dropX)) <= p2_dropX.dist(p0_dropX) * p2_dropX.dist(p1_dropX) * toll;
    bool Dy = fabs(orient2d(p0_dropY, p1_dropY, p2_dropY)) <= p2_dropY.dist(p0_dropY) * p2_dropY.dist(p1_dropY) * toll;
    bool Dz = fabs(orient2d(p0_dropZ, p1_dropZ, p2_dropZ)) <= p2_dropZ.dist(p0_dropZ) * p2_dropZ.dist(p1_dropZ) * toll;
    return Dx && Dy && Dz;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// true if the normalized volume of the tetrahedron p0-p1-p2-p3 is smaller than toll
CINO_INLINE
bool points_are_coplanar_3d_toll(const vec3d & p0,
                                 const vec3d & p1,
                                 const vec3d & p2,
                                 const vec3d & p3,
                                 const double toll)
{
    return (fabs(orient3d(p0,p1,p2,p3)) <= p0.dist(p1) * p0.dist(p2) * p0.dist(p3) * toll);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// true if the verts of the two polygons are coplanar (up to toll)
CINO_INLINE
bool polys_are_coplanar_toll(const std::vector<vec3d> & poly0,
                             const std::vector<vec3d> & poly1,
                             const double toll)
{
    // select three non-aligned verts in each poly
    bool found0 = false, found1 = false;
    vec3d v0a = poly0.at(0), v0b = poly0.at(1), v0c;
    for (int i = 2; i < poly0.size(); i++) {
        v0c = poly0.at(i);
        if (!points_are_colinear_3d_toll(v0a, v0b, v0c, toll)) {
            found0 = true;
            break;
        }
    }
    vec3d v1a = poly1.at(0), v1b = poly1.at(1), v1c;
    for (int i = 2; i < poly1.size(); i++) {
        v1c = poly1.at(i);
        if (!points_are_colinear_3d_toll(v1a, v1b, v1c, toll)) {
            found1 = true;
            break;
        }
    }
    assert(found0 && found1);

    // check if all the combinations of points are coplanar
    bool Da = points_are_coplanar_3d_toll(v0a, v0b, v0c, v1a, toll);
    bool Db = points_are_coplanar_3d_toll(v0a, v0b, v0c, v1b, toll);
    bool Dc = points_are_coplanar_3d_toll(v0a, v0b, v0c, v1c, toll);
    return Da && Db && Dc;
}

}

#endif // EXTENDED_PREDICATES
