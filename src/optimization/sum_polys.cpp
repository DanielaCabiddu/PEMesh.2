#include "sum_polys.h"

namespace cinolib {

CINO_INLINE
void sum_polys(const uint &pid0, const uint &pid1, const Polygonmesh<> &m,
               std::vector<vec3d> &verts) {
  /* check that the polygons are adjacent and coplanar */
  verts.clear();
  assert(m.polys_are_adjacent(pid0, pid1) && "ERROR: polys are not adjacent");
  if(!polys_are_coplanar_toll(m.poly_verts(pid0), m.poly_verts(pid1), TOLL_3D))
    return;

  Polygonmesh<> new_poly;
  int vid_off = 0;

  /* add all the vertices of pid0 to new_poly, and the relative indices to poly0 */
  std::vector<uint> poly0;
  for (const vec3d &v : m.poly_verts(pid0)) {
    new_poly.vert_add(v);
    poly0.push_back(vid_off);
    vid_off++;
  }
  new_poly.poly_add(poly0);

  /* add all the vertices of pid1 to new_poly, and the relative indices to poly1 */
  std::vector<uint> poly1;
  for (const vec3d &v : m.poly_verts(pid1)) {
    new_poly.vert_add(v);
    poly1.push_back(vid_off);
    vid_off++;
  }
  new_poly.poly_add(poly1);

  /* merge eventual coincident vertices in new_poly */
  for (uint vid0 = 0; vid0 < new_poly.num_verts(); vid0++) {
    for (uint vid1 = vid0 + 1; vid1 < new_poly.num_verts(); vid1++) {
      vec3d v0 = new_poly.vert(vid0);
      vec3d v1 = new_poly.vert(vid1);
      if (v0.operator==(v1))
        new_poly.vert_merge(vid0, vid1);
    }
  }

  /* store in verts only the (ordered) boundary vertices of new_poly */
  for (uint vid : new_poly.get_ordered_boundary_vertices()) {
    verts.push_back(new_poly.vert(vid));
  }
  if (verts.size() != new_poly.get_boundary_edges().size())
    verts.clear();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void sum_polys(const uint &pid0, const uint &pid1, const Polyhedralmesh<> &m,
               std::vector<vec3d> &verts, std::vector<std::vector<uint>> &faces,
               std::vector<vec3d> &normals) {
  verts.clear();
  faces.clear();
  normals.clear();
  bool polys_are_adjacent = false;
  for(uint fid : m.adj_p2f(pid0)) {
      if ((uint)m.poly_adj_through_face(pid0, fid) == pid1) {
          polys_are_adjacent = true;
          break;
      }
  }
  assert(polys_are_adjacent);

  /* store all the faces that belong to pid0 and not to pid1 */
  for (uint fid : m.adj_p2f(pid0))
    if (!m.poly_contains_face(pid1, fid)) {
      std::vector<uint> f = m.face_verts_id(fid);
      if (!m.poly_face_winding(pid0, fid))
        REVERSE_VEC(f);
      faces.push_back(f);
      normals.push_back(m.poly_face_normal(pid0, fid));
    }

  /* add all the faces that belong to pid1 and not to pid0 */
  for (uint fid : m.adj_p2f(pid1))
    if (!m.poly_contains_face(pid0, fid)) {
      std::vector<uint> f = m.face_verts_id(fid);
      if (!m.poly_face_winding(pid1, fid))
        REVERSE_VEC(f);
      faces.push_back(f);
      normals.push_back(m.poly_face_normal(pid1, fid));
    }

  /* add all the vertices that belong to the saved faces */
  for (std::vector<uint> &f : faces)
    for (uint &vid : f) {
      vec3d v = m.vert(vid);
      auto it = std::find(verts.begin(), verts.end(), v);
      if (it == verts.end()) {
        verts.push_back(v);
        vid = verts.size() - 1;
      } else
        vid = it - verts.begin();
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void sum_polys(const std::vector<uint> &polys, const Polygonmesh<> &m,
               std::vector<vec3d> &verts) {
    /* check that the polygons are coplanar */
    verts.clear();
    assert(polys.size() > 1);

    uint pid0 = polys.front();
    for (uint pid1 : polys) {
        if(!polys_are_coplanar_toll(m.poly_verts(pid0), m.poly_verts(pid1), TOLL_3D))
          return;
    }

    Polygonmesh<> new_poly;
    int vid_off = 0;

    /* add all the vertices of the elements in polys to new_poly */
    std::vector<bool> visited(m.num_verts(), false);
    for (uint pid : polys) {
        std::vector<uint> p;
        for (uint vid : m.poly_verts_id(pid)) {
          vec3d v = m.vert(vid);
          if (visited.at(vid)) {
              auto it = std::find(new_poly.vector_verts().begin(), new_poly.vector_verts().end(), v);
              p.push_back(it - new_poly.vector_verts().begin());
          } else {
              new_poly.vert_add(v);
              p.push_back(vid_off);
              vid_off++;
              visited.at(vid) = true;
          }
        }
        assert(p.size() > 2);
        new_poly.poly_add(p);
    }

    /* store in verts only the (ordered) boundary vertices of new_poly */
    for (uint vid : new_poly.get_ordered_boundary_vertices()) {
      verts.push_back(new_poly.vert(vid));
    }
    if (verts.size() != new_poly.get_boundary_edges().size())
      verts.clear();

    // TODO: riscrivere AbstractPolygonMesh<M,V,E,P>::get_ordered_boundary_vertices() senza la classe mesh
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void sum_polys(const std::vector<uint> &polys, const Polyhedralmesh<> &m,
               std::vector<vec3d> &verts, std::vector<std::vector<uint>> &faces,
               std::vector<vec3d> &normals) {
    verts.clear();
    faces.clear();
    normals.clear();
    assert(polys.size() > 1);

    /* count how many times each face appears in *polys* */
    std::unordered_map<uint, int> face_count;
    for (uint pid : polys) {
        for (uint fid : m.adj_p2f(pid)) {
            ++face_count[fid];
        }
    }

    /* add all the faces that belong to only one poly, with their normals */
    for (auto &it : face_count) {
        if (it.second > 1) continue;
        uint fid = it.first;
        std::vector<uint> nbr = m.adj_f2p(fid);
        uint pid;
        if (CONTAINS_VEC(polys, nbr.front())) {
            pid = nbr.front();
        } else if (CONTAINS_VEC(polys, nbr.back())) {
            pid = nbr.back();
        } else assert(false && "sum_polys: pid not found!");

        std::vector<uint> f = m.face_verts_id(fid);
        if (!m.poly_face_winding(pid, fid))
          REVERSE_VEC(f);
        faces.push_back(f);
        normals.push_back(m.poly_face_normal(pid, fid));
    }

    /* add all the vertices that belong to the saved faces */
    for (std::vector<uint> &f : faces) {
        for (uint &vid : f) {
            vec3d v = m.vert(vid);
            auto it = std::find(verts.begin(), verts.end(), v);
            if (it == verts.end()) {
                verts.push_back(v);
                vid = verts.size() - 1;
            } else {
                vid = it - verts.begin();
            }
        }
    }
}

} // namespace cinolib

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// CINO_INLINE
// void sum_polys(const uint &pid0, const uint &pid1, const Polyhedralmesh<> &m,
//               std::vector<vec3d> &verts,
//               std::vector<std::vector<uint>> &faces,
//               std::vector<vec3d> &normals) {
//  verts.clear();
//  faces.clear();
//  assert(m.polys_are_adjacent(pid0, pid1));
//  Polyhedralmesh<> new_poly;

//  // compute polys pid0 and pid1
//  std::vector<vec3d> verts0, verts1;
//  std::vector<vec3d> normals0, normals1;
//  std::vector<std::vector<uint>> faces0, faces1;
//  m.poly_export_element(pid0, verts0, faces0);
//  m.poly_export_element(pid1, verts1, faces1);
//  for (uint fid : m.adj_p2f(pid0))
//      normals0.push_back(m.poly_face_normal(pid0, fid));
//  for (uint fid : m.adj_p2f(pid1))
//      normals1.push_back(m.poly_face_normal(pid1, fid));

//  // add pid0 to the new polyhedron
//  std::map<vec3d, uint> vmap;
//  for (const vec3d &v : verts0) {
//    uint fresh_id = vmap.size();
//    vmap[v] = fresh_id;
//    new_poly.vert_add(v);
//  }
//  for (const std::vector<uint> &f : faces0)
//    new_poly.face_add(f);
//  std::vector<bool> wind0(faces0.size(), true);
//  std::vector<uint> poly0(faces0.size());
//  std::iota(poly0.begin(), poly0.end(), 0);
//  new_poly.poly_add(poly0, wind0);

//  // add pid1 to the new polyhedron
//  std::vector<uint> poly1;
//  std::vector<bool> wind1;
//  for (const std::vector<uint> &f : faces1) {
//    std::vector<uint> f_new;
//    for (const uint &vid : f) {
//      vec3d v = verts1.at(vid);
//      auto it = vmap.find(v);
//      if (it == vmap.end()) {
//        uint fresh_id = vmap.size();
//        vmap[v] = fresh_id;
//        new_poly.vert_add(v);
//        f_new.push_back(fresh_id);
//      } else
//        f_new.push_back(it->second);
//    }
//    int fid = new_poly.face_id(f_new);
//    if (fid == -1) {
//      new_poly.face_add(f_new);
//      poly1.push_back(new_poly.num_faces() - 1);
//      wind1.push_back(true);
//    } else {
//      poly1.push_back(fid);
//      wind1.push_back(false);
//    }
//  }
//  new_poly.poly_add(poly1, wind1);

//  new_poly = merge_faces(new_poly);

//  // check the topology of the new polyhedron
//  for (uint vid : new_poly.get_surface_verts())
//    verts.push_back(new_poly.vert(vid));
//  for (uint fid : new_poly.get_surface_faces())
//    faces.push_back(new_poly.face_verts_id(fid));
//  if (verts.size() - new_poly.num_srf_edges() + faces.size() != 2) {
//    verts.clear();
//    faces.clear();
//    return;
//  }

//  // merge coplanar faces
//  for (uint pid = 0; pid < new_poly.num_polys(); pid++) {
//    std::vector<uint> faces = new_poly.adj_p2f(pid);
//    SORT_VEC(faces, false);
//    for (int i = faces.size() - 1; i >= 0; i--) {
//      uint fid0 = faces.at(i);
//      vec3d n0 = new_poly.poly_face_normal(pid, fid0);
//      for (int j = i - 1; j >= 0; j--) {
//        uint fid1 = faces.at(j);
//        vec3d n1 = new_poly.poly_face_normal(pid, fid1);
//        if (!new_poly.faces_are_adjacent(fid0, fid1) ||
//            new_poly.adj_f2p(fid0) != new_poly.adj_f2p(fid1))
//          continue;
//        if (fabs(n0.dot(n1) - 1.0) < TOLL_1D) {
//          new_poly.faces_merge(fid0, fid1);
//          faces = new_poly.adj_p2f(pid);
//          SORT_VEC(faces, false);
//        }
//      }
//    }
//  }
//}

// CINO_INLINE
// Polyhedralmesh<> sum_polys(const std::vector<vec3d> &verts0,
//                           const std::vector<vec3d> &verts1,
//                           const std::vector<std::vector<uint>> &faces0,
//                           const std::vector<std::vector<uint>> &faces1) {
//  Polyhedralmesh<> new_poly;

//  // add pid0 to the new polyhedron
//  std::map<vec3d, uint> vmap;
//  for (const vec3d &v : verts0) {
//    uint fresh_id = vmap.size();
//    vmap[v] = fresh_id;
//    new_poly.vert_add(v);
//  }
//  for (const std::vector<uint> &f : faces0)
//    new_poly.face_add(f);
//  std::vector<bool> wind0(faces0.size(), true);
//  std::vector<uint> poly0(faces0.size());
//  std::iota(poly0.begin(), poly0.end(), 0);
//  new_poly.poly_add(poly0, wind0);

//  // add pid1 to the new polyhedron
//  std::vector<uint> poly1;
//  std::vector<bool> wind1;
//  for (const std::vector<uint> &f : faces1) {
//    std::vector<uint> f_new;
//    for (const uint &vid : f) {
//      vec3d v = verts1.at(vid);
//      auto it = vmap.find(v);
//      if (it == vmap.end()) {
//        uint fresh_id = vmap.size();
//        vmap[v] = fresh_id;
//        new_poly.vert_add(v);
//        f_new.push_back(fresh_id);
//      } else
//        f_new.push_back(it->second);
//    }
//    int fid = new_poly.face_id(f_new);
//    if (fid == -1) {
//      new_poly.face_add(f_new);
//      poly1.push_back(new_poly.num_faces() - 1);
//      wind1.push_back(true);
//    } else {
//      poly1.push_back(fid);
//      wind1.push_back(false);
//    }
//  }
//  new_poly.poly_add(poly1, wind1);
//  return new_poly;
//}
