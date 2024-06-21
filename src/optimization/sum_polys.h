#ifndef SUM_POLYS_H
#define SUM_POLYS_H

#include <cinolib/cino_inline.h>
#include <cinolib/meshes/polygonmesh.h>
#include <cinolib/meshes/polyhedralmesh.h>

#include "global_variables.h"
#include "extended_predicates.h"

namespace cinolib {

CINO_INLINE
void sum_polys(const uint &pid0, const uint &pid1, const Polygonmesh<> &m,
               std::vector<vec3d> &verts);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void sum_polys(const uint &pid0, const uint &pid1, const Polyhedralmesh<> &m,
               std::vector<vec3d> &verts, std::vector<std::vector<uint>> &faces, std::vector<vec3d> &normals);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void sum_polys(const std::vector<uint> &polys, const Polygonmesh<> &m,
               std::vector<vec3d> &verts, std::vector<std::vector<uint>> &faces, std::vector<vec3d> &normals);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void sum_polys(const std::vector<uint> &polys, const Polyhedralmesh<> &m,
               std::vector<vec3d> &verts, std::vector<std::vector<uint>> &faces, std::vector<vec3d> &normals);

} // namespace cinolib

#ifndef CINO_STATIC_LIB
#include "sum_polys.cpp"
#endif

#endif // SUM_POLYS_H
