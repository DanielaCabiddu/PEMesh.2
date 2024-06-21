#include "mesh_check.h"

namespace cinolib {

CINO_INLINE
bool manifold_check(const Polygonmesh<> &m) {
  for (uint vid = 0; vid < m.num_verts(); vid++)
    if (!m.vert_is_manifold(vid))
        return false;

  for (uint eid = 0; eid < m.num_edges(); eid++)
    if (!m.edge_is_manifold(eid))
        return false;

  return true;
}

CINO_INLINE
bool manifold_check(const Polyhedralmesh<> &m) {
  for (uint vid = 0; vid < m.num_verts(); vid++)
    if (!m.vert_is_manifold(vid))
        return false;

  for (uint eid = 0; eid < m.num_edges(); eid++)
    if (!m.edge_is_manifold(eid))
        return false;

  return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool size_check(const Polygonmesh<> &m, const int dim) {
  for (uint eid = 0; eid < m.num_edges(); ++eid)
    if (m.edge_length(eid) < TOLL_1D)
        return false;

  for (uint pid = 0; pid < m.num_polys(); ++pid)
    if (m.poly_area(pid) < TOLL_2D)
        return false;

  if (dim == 3) // only for non-planar 2D meshes
    if (m.mesh_volume() < TOLL_3D)
        return false;

  return true;
}

CINO_INLINE
bool size_check(const Polyhedralmesh<> &m, const int dim) {
  for (uint eid = 0; eid < m.num_edges(); ++eid)
    if (m.edge_length(eid) < TOLL_1D)
        return false;

  for (uint fid = 0; fid < m.num_faces(); ++fid)
    if (m.face_area(fid) < TOLL_2D)
        return false;

  for (uint pid = 0; pid < m.num_polys(); ++pid)
    if (m.poly_volume(pid) < TOLL_3D)
        return false;

  if (m.mesh_volume() < TOLL_3D)
      return false;

  return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool duplicates_check(const Polygonmesh<> &m) {
  std::vector<vec3d> verts = m.vector_verts();
  REMOVE_DUPLICATES_FROM_VEC(verts);
  if (m.num_verts() != verts.size())
      return false;

  std::vector<std::vector<uint>> polys = m.vector_polys();
  REMOVE_DUPLICATES_FROM_VEC(polys);
  if (m.num_polys() != polys.size())
      return false;

  return true;
}

CINO_INLINE
bool duplicates_check(const Polyhedralmesh<> &m) {
  std::vector<vec3d> verts = m.vector_verts();
  REMOVE_DUPLICATES_FROM_VEC(verts);
  if (m.num_verts() != verts.size())
      return false;

  std::vector<std::vector<uint>> faces = m.vector_faces();
  REMOVE_DUPLICATES_FROM_VEC(faces);
  if (m.num_faces() != faces.size())
      return false;

  std::vector<std::vector<uint>> polys = m.vector_polys();
  REMOVE_DUPLICATES_FROM_VEC(polys);
  if (m.num_polys() != polys.size())
      return false;

  return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void mesh_check_assert(const Polygonmesh<> &m) {
  assert(manifold_check(m));
  assert(size_check(m, 2));
  assert(duplicates_check(m));
  assert(connected_components(m) == 1);
  assert(m.genus() == 0);
}

CINO_INLINE
void mesh_check_assert(const Trimesh<> &m) {
  Polygonmesh<> m_tmp(m.vector_verts(), m.vector_polys());
  mesh_check_assert(m_tmp);
}

CINO_INLINE
void mesh_check_assert(const Polyhedralmesh<> &m) {
  assert(manifold_check(m));
  assert(size_check(m, 3));
  assert(duplicates_check(m));
  assert(connected_components(m) == 1);
  assert(m.Euler_characteristic() == 1);
  // assert(m.Euler_characteristic_srf() == 2);
}

CINO_INLINE
void mesh_check_assert(const Tetmesh<> &m) {
  assert(false && "not implemented yet!");
  // Polyhedralmesh<> m_tmp(m.vector_verts(), m.vector_faces(), m.vector_polys(), m.vector_windings());
  // mesh_check_assert(m_tmp);
}

/* from Euler formula: X = B0 - B1 + B2, where
 * B0 = n. of connected components = 1;
 * B1 = n. of tunnels
 * B2 = n. of cavities
 * if X > 1 then B2 > 0 */

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool mesh_check(const Polygonmesh<> &m, const int dim, bool verbose) {
  bool T = manifold_check(m);
  bool S = size_check(m, dim);
  bool D = duplicates_check(m);
  bool C = (connected_components(m) == 1);
  bool G = (m.genus() == 0);
  bool check = T && S && D && C && G;
  if (verbose && !check)
      std::cout << "\nMesh Check \n"
                << "topology: " << T << std::endl
                << "size: " << S << std::endl
                << "duplicates: " << D << std::endl
                << "connected components: " << C << std::endl
                << "genus: " << G << std::endl;
  return check;
}

CINO_INLINE
bool mesh_check(const Trimesh<> &m, const int dim, bool verbose) {
  Polygonmesh<> m_tmp(m.vector_verts(), m.vector_polys());
  return mesh_check(m_tmp, dim, verbose);
}

CINO_INLINE
bool mesh_check(const Polyhedralmesh<> &m, const int dim, bool verbose) {
  bool T = manifold_check(m);
  bool S = size_check(m, dim);
  bool D = duplicates_check(m);
  bool C = (connected_components(m) == 1);
  bool G = (m.Euler_characteristic() == 1);
  // bool GS = (m.Euler_characteristic_srf() == 2);
  bool check = T && S && D && C && G /*&& GS*/;
  if (verbose && !check)
      std::cout << "\nMesh Check \n"
                << "topology: " << T << std::endl
                << "size: " << S << std::endl
                << "duplicates: " << D << std::endl
                << "connected components: " << C << std::endl
                << "Euler char: " << G << std::endl;
                // << "srf Euler char: " << GS << std::endl;
  return check;
}

CINO_INLINE
bool mesh_check(const Tetmesh<> &m, const int dim, bool verbose) {
  assert(false && "not implemented yet!");
  // Polyhedralmesh<> m_tmp(m.vector_verts(), m.vector_faces(), m.vector_polys(), m.vector_windings());
  // return mesh_check(m_tmp, dim, verbose);
}

} // namespace cinolib
