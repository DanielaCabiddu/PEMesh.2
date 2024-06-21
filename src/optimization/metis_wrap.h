#ifndef CINO_METIS_WRAP_H
#define CINO_METIS_WRAP_H

#include <cinolib/cino_inline.h>
#include "mesh_agglomeration.h"
#include "sum_polys.h"

#ifdef CINOLIB_USES_METIS
#include "MetisUtilities.hpp"
#endif

namespace cinolib {

double (*indicator)(const std::vector<vec3d>&);

CINO_INLINE
int metis_wrap_dual(Polygonmesh<> &m, const double parts,
                    double (*quality)(const std::vector<vec3d> &),
                    const bool N_WEIGHTS=true, const bool A_WEIGHTS=true);
/* nodes -> polys, arcs -> edges */

CINO_INLINE
int metis_wrap_dual(Polyhedralmesh<> &m, const double parts,
                    double (*quality)(const std::vector<vec3d> &, const std::vector<std::vector<uint>> &, const std::vector<vec3d> &),
                    const bool N_WEIGHTS=true, const bool A_WEIGHTS=true);
/* nodes -> polys, arcs -> faces */

CINO_INLINE
int metis_wrap_primal(Polygonmesh<> &m, const double parts,
                      double (*quality)(const std::vector<vec3d> &),
                      const bool N_WEIGHTS=true, const bool A_WEIGHTS=true);
/* nodes -> verts, arcs -> edges */

CINO_INLINE
int metis_wrap_primal(Polyhedralmesh<> &m, const double parts,
                      double (*quality)(const std::vector<vec3d> &, const std::vector<std::vector<uint>> &, const std::vector<vec3d> &),
                      const bool N_WEIGHTS=true, const bool A_WEIGHTS=true);
/* nodes -> verts, arcs -> edges */

}; // namespace cinolib

#ifndef CINO_STATIC_LIB
#include "metis_wrap.cpp"
#endif

#endif // CINO_METIS_WRAP_H
