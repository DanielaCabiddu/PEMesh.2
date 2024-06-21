#ifndef MESH_AGGLOMERATION_H
#define MESH_AGGLOMERATION_H

#include <cinolib/cino_inline.h>
#include <cinolib/meshes/polygonmesh.h>
#include <cinolib/meshes/polyhedralmesh.h>

#include "mesh_check.h"
#include "global_variables.h"
#include "extended_predicates.h"

namespace cinolib {

enum { CONSTRAINED = UNUSED_0 };

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void mesh_agglomerate_wrt_labels(Polygonmesh<> &m);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void mesh_agglomerate_edges(Polygonmesh<> &m);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void mesh_agglomerate_wrt_labels(Polyhedralmesh<> &m);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void mesh_agglomerate_faces(Polyhedralmesh<> &m);

} // namespace cinolib

#ifndef CINO_STATIC_LIB
#include "mesh_agglomeration.cpp"
#endif

#endif // MESH_AGGLOMERATION_H
