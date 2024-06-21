#ifndef MESH_CHECK_H
#define MESH_CHECK_H

#include <cinolib/cino_inline.h>
#include <cinolib/meshes/meshes.h>
#include <cinolib/connected_components.h>

#include "global_variables.h"

namespace cinolib {

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool manifold_check(const Polygonmesh<> &m);

CINO_INLINE
bool manifold_check(const Polyhedralmesh<> &m);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool size_check(const Polygonmesh<> &m, const int dim);

CINO_INLINE
bool size_check(const Polyhedralmesh<> &m, const int dim);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool duplicates_check(const Polygonmesh<> &m);

CINO_INLINE
bool duplicates_check(const Polyhedralmesh<> &m);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void mesh_check_assert(const Polygonmesh<> &m);

CINO_INLINE
void mesh_check_assert(const Trimesh<> &m);

CINO_INLINE
void mesh_check_assert(const Polyhedralmesh<> &m);

CINO_INLINE
void mesh_check_assert(const Tetmesh<> &m);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool mesh_check(const Polygonmesh<> &m, const int dim = 3, bool verbose = false);

CINO_INLINE
bool mesh_check(const Trimesh<> &m, const int dim = 3, bool verbose = false);

CINO_INLINE
bool mesh_check(const Polyhedralmesh<> &m, const int dim = 3, bool verbose = false);

CINO_INLINE
bool mesh_check(const Tetmesh<> &m, const int dim = 3, bool verbose = false);

} // namespace cinolib

#ifndef  CINO_STATIC_LIB
#include "mesh_check.cpp"
#endif

#endif // MESH_CHECK_H
