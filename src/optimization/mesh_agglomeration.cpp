#include "mesh_agglomeration.h"

namespace cinolib {

template<class M, class V, class E, class P>
CINO_INLINE
int poly_label_map(AbstractMesh<M,V,E,P> &m, std::map<int, std::vector<uint>> &label_map) {
    int l_max = -1;
    for (uint pid = 0; pid < m.num_polys(); pid++) {
      int l = m.poly_data(pid).label;
      label_map[l].push_back(pid);
      l_max = std::max(l, l_max);
    }
    return l_max;
}

CINO_INLINE
int face_label_map(Polyhedralmesh<> &m, std::map<int, std::vector<uint>> &label_map) {
    int l_max = -1;
    for (uint fid = 0; fid < m.num_faces(); fid++) {
      int l = m.face_data(fid).label;
      label_map[l].push_back(fid);
      l_max = std::max(l, l_max);
    }
    return l_max;
}

template<class M, class V, class E, class P>
CINO_INLINE
int edge_label_map(AbstractMesh<M,V,E,P> &m, std::map<int, std::vector<uint>> &label_map) {
    int l_max = -1;
    for (uint eid = 0; eid < m.num_edges(); eid++) {
      int l = m.edge_data(eid).label;
      label_map[l].push_back(eid);
      l_max = std::max(l, l_max);
    }
    return l_max;
}

}

//:::::::::::::::::::::::::::POLYGONAL MESHES:::::::::::::::::::::::::::::::

namespace cinolib {

CINO_INLINE
Polygonmesh<> build_element_from_polys(const Polygonmesh<> &m, const std::vector<uint> &plist) {
  Polygonmesh<> element;
  int vid_off = 0;
  for (uint pid : plist) {
    std::vector<uint> vlist;
    for (const vec3d &v : m.poly_verts(pid)) {
      element.vert_add(v);
      vlist.push_back(vid_off);
      vid_off++;
    }
    element.poly_add(vlist);
  }
  /* merge eventual coincident verts */
  for (uint vid0 = 0; vid0 < element.num_verts(); ++vid0) {
    for (uint vid1 = 0; vid1 < element.num_verts(); ++vid1) {
      vec3d v0 = element.vert(vid0);
      vec3d v1 = element.vert(vid1);
      if (vid0 != vid1 && v0.operator==(v1)) {
        bool merge = element.vert_merge(vid0, vid1);
        assert(merge);
        --vid1;
      }
    }
  }
  return element;
}

CINO_INLINE
bool preserve_constrained_edges(const Polygonmesh<> &new_poly, const Polygonmesh<> &m, const std::map<vec3d, uint> &verts_map, std::vector<uint> &constrained_edges) {
  bool found = false;
  for (uint eid = 0; eid < new_poly.num_edges(); eid++) {
    auto it0 = verts_map.find(new_poly.edge_vert(eid, 0));
    auto it1 = verts_map.find(new_poly.edge_vert(eid, 1));
    assert(it0 != verts_map.end() && it1 != verts_map.end());
    std::vector<uint> vids_old = {it0->second, it1->second};
    int eid_old = m.edge_id(vids_old);
    assert(eid_old != -1);
    /* if the constr edge is on the boundary of new_poly we preserve it, if it is inside we cannot agglomerate */
    if (m.edge_data(eid_old).flags[CONSTRAINED]) {
        found = new_poly.edge_is_boundary(eid);
        constrained_edges.push_back(eid_old);
    }
  }
  return found;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void mesh_agglomerate_wrt_labels(Polygonmesh<> &m) {
    /* create a map <label, all polys with that label> */
    std::map<int, std::vector<uint>> labels;
    int l_max = poly_label_map(m, labels);

    std::vector<uint> polys2remove;
    int bad_guys = 0;
    for (auto l = labels.begin(); l != labels.end();) {
      int label = l->first;
      std::vector<uint> plist = l->second;
      if (plist.size() < 2) { ++l; continue; }

      /* build the new poly */
      Polygonmesh<> new_poly = build_element_from_polys(m, plist);
      std::map<vec3d, uint> verts_map;
      for (uint pid : plist)
          for (uint vid : m.adj_p2v(pid))
              verts_map[m.vert(vid)] = vid;

      /* check for the presence of constrained edges to preserve */
      std::vector<uint> constr_edges;
      bool constr_check = preserve_constrained_edges(new_poly, m, verts_map, constr_edges);

      /* check the correctness of the new poly */
      if (mesh_check(new_poly, 2) && !constr_check) { /* good elements are agglomerated and added to the mesh */
          std::vector<uint> vlist;
          for (uint vid : new_poly.get_ordered_boundary_vertices()) {
              auto it = verts_map.find(new_poly.vert(vid));
              assert(it != verts_map.end());
              vlist.push_back(it->second);
          }

          uint pid_new = m.poly_add(vlist);
          m.poly_data(pid_new).label = label;
          for (uint eid : constr_edges) {
                uint vid0_new = m.pick_vert(new_poly.edge_vert(eid, 0));
                uint vid1_new = m.pick_vert(new_poly.edge_vert(eid, 1));
                int eid_new = m.edge_id(vid0_new, vid1_new);
                assert(eid_new != -1);
                m.vert_data(vid0_new).flags[CONSTRAINED] = true;
                m.vert_data(vid1_new).flags[CONSTRAINED] = true;
                m.edge_data(eid_new).flags[CONSTRAINED] = true;
              }
          polys2remove.insert(polys2remove.end(), plist.begin(), plist.end());
          ++l;
      }
      else { /* bad elements receive new labels and are not agglomerated */
        for (uint pid : plist) {
          ++l_max;
          m.poly_data(pid).label = l_max;
          labels.insert({l_max, {pid}});
        }
        l = labels.erase(l);
        ++bad_guys;
      }
    }
    if (bad_guys != 0)
      std::cout << "mesh_agglomeration: refused to merge " << bad_guys << " polyhedra!\n";

    m.polys_remove(polys2remove);
    mesh_agglomerate_edges(m);
}

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void mesh_agglomerate_edges(Polygonmesh<> &m) {
  std::vector<uint> to_remove;
  for (uint pid = 0; pid < m.num_polys(); pid++) {
    std::vector<uint> verts = m.poly_verts_id(pid);
    uint poly_label = m.poly_data(pid).label;
    std::vector<uint> new_poly;
    std::vector<bool> new_edges;

    /* add vertex vid to new_poly only if it satisfies one of the followings:
     * - vid has more than two incident edges
     * - vid is marked but one of its adj edges is not (extremity of a marked sequence)
     * - vid is not aligned with its neighboring verts */

    for (uint i = 0; i < verts.size(); i++) {
      uint vid = verts.at(i);
      uint vid_prev = verts.at((i > 0) ? (i - 1) : verts.size() - 1);
      uint vid_next = verts.at((i + 1) % verts.size());
      uint eid_prev = m.poly_edge_id(pid, vid, vid_prev);
      uint eid_next = m.poly_edge_id(pid, vid, vid_next);

      bool case_1 = m.vert_valence(vid) > 2;
      bool case_2 = m.vert_data(vid).flags[CONSTRAINED] &&
                  (!m.edge_data(eid_prev).flags[CONSTRAINED] ||
                   !m.edge_data(eid_next).flags[CONSTRAINED]);
      bool case_3 = !points_are_colinear_3d_toll(m.vert(vid_prev), m.vert(vid), m.vert(vid_next), TOLL_3D);

      if (case_1 || case_2 || case_3) {
          new_poly.push_back(vid);
          new_edges.push_back(m.edge_data(eid_next).flags[CONSTRAINED]);
      }
    }

    /* if new_poly is different from pid, add it to m and remove pid. */
    if (new_poly.size() != m.verts_per_poly(pid)) {
      uint pid_new = m.poly_add(new_poly);
      m.poly_data(pid_new).label = poly_label;
      for (uint i = 0; i < m.edges_per_poly(pid_new); ++i) {
          uint eid = m.adj_p2e(pid_new).at(i);
          m.edge_data(eid).flags[CONSTRAINED] = new_edges.at(i);
      }
      to_remove.push_back(pid);
    }
  }
  m.polys_remove(to_remove);
}

}; // namespace cinolib

//:::::::::::::::::::::::::::POLYHEDRAL MESHES::::::::::::::::::::::::::::

namespace cinolib {

CINO_INLINE
Polygonmesh<> build_element_from_faces(const Polyhedralmesh<> &m, const std::vector<uint> &flist, const std::vector<bool> &winding) {
  Polygonmesh<> element;
  int vid_off = 0;
  for (uint i = 0; i < flist.size(); ++i) {
    uint fid = flist.at(i);
    std::vector<uint> vlist;
    for (const vec3d &v : m.face_verts(fid)) {
      element.vert_add(v);
      vlist.push_back(vid_off);
      vid_off++;
    }
    if (!winding.at(i))
        REVERSE_VEC(vlist);
    element.poly_add(vlist);
  }
  /* merge eventual coincident verts */
  for (uint vid0 = 0; vid0 < element.num_verts(); ++vid0) {
    for (uint vid1 = 0; vid1 < element.num_verts(); ++vid1) {
      vec3d v0 = element.vert(vid0);
      vec3d v1 = element.vert(vid1);
      if (vid0 != vid1 && v0.operator==(v1)) {
        bool merge = element.vert_merge(vid0, vid1);
        assert(merge);
        --vid1;
      }
    }
  }
  return element;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void mesh_agglomerate_wrt_labels(Polyhedralmesh<> &m) {
  /* create a map <label, all polys with that label> */
  std::map<int, std::vector<uint>> labels;
  int l_max = poly_label_map(m, labels);

  std::vector<uint> polys2remove;
  int bad_guys = 0;
  for (auto l = labels.begin(); l != labels.end();) {
    int label = l->first;
    std::vector<uint> plist = l->second;
    if (plist.size() < 2) { ++l; continue; }

    /* build the new poly */
    std::vector<uint> flist;
    std::vector<bool> fwinding;
    for (uint pid : plist) {
      for (uint fid : m.adj_p2f(pid)) {
        /* save all the faces of the elements in polys that
         * are boundary (nbr = -1) or not shared by other elements in polys */
        int nbr = m.poly_adj_through_face(pid, fid);
        if (DOES_NOT_CONTAIN_VEC(plist, nbr)) {
          flist.push_back(fid);
          fwinding.push_back(m.poly_face_winding(pid, fid));
        }
      }
    }
    assert(flist.size() > 3);
    Polygonmesh<> new_poly = build_element_from_faces(m, flist, fwinding);

    /* check the correctness of the new poly */
    if (mesh_check(new_poly, 3)) { /* good elements are agglomerated and added to the mesh */
        uint pid_new = m.poly_add(flist, fwinding);
        m.poly_data(pid_new).label = label;
        polys2remove.insert(polys2remove.end(), plist.begin(), plist.end());
        ++l;
    }
    else { /* bad elements receive new labels and are not agglomerated */
      for (uint pid : plist) {
        ++l_max;
        m.poly_data(pid).label = l_max;
        labels.insert({l_max, {pid}});
      }
      l = labels.erase(l);
      ++bad_guys;
    }
  }
  if (bad_guys != 0)
    std::cout << "mesh_agglomeration: refused to merge " << bad_guys << " polyhedra!\n";

  m.polys_remove(polys2remove);
  assert(m.poly_fix_orientation());
  mesh_agglomerate_faces(m);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void label_faces(Polyhedralmesh<> &m) {
    /* mark with the same label the faces that are coplanar and with the same neighboring cells. */
    m.face_apply_label(-1);
    uint l = 0;
    std::set<uint> visited;
    for (uint fid = 0; fid < m.num_faces(); fid++) {
        if (CONTAINS(visited, fid)) continue;

        std::queue<uint> front;
        front.push(fid);
        while (!front.empty()) {
            uint seed = front.front();
            visited.insert(seed);
            front.pop();

            for (uint nbr : m.adj_f2f(seed)) {
                if (CONTAINS(visited, nbr)) continue;
                if (m.adj_f2p(seed) == m.adj_f2p(nbr) && polys_are_coplanar_toll(m.face_verts(seed), m.face_verts(nbr), TOLL_3D)) {
                    m.face_data(seed).label = l;
                    m.face_data(nbr).label = l;
                    front.push(nbr);
                }
            }
        }
        l++;
    }
}

CINO_INLINE
void mesh_agglomerate_faces(Polyhedralmesh<> &m) {
  /* create a map <label, all faces with that label> */
  label_faces(m);
  std::map<int, std::vector<uint>> labels;
  face_label_map(m, labels);

  /* add the new faces to the mesh */
  std::vector<uint> polys2remove;
  int bad_guys = 0;
  std::map<uint, std::vector<std::pair<uint, bool>>> polys2add;
  for (auto l = labels.begin(); l != labels.end();) {
    int label = l->first;
    std::vector<uint> flist = l->second;
    std::vector<bool> fwinding(flist.size(), true);
    if (flist.size() < 2 || label == -1) { ++l; continue; }

    /* build the new face and check its correctness */
    Polygonmesh<> new_face = build_element_from_faces(m, flist, fwinding);
    if (mesh_check(new_face, 2)) { /* good faces are agglomerated and added to the mesh */
        std::map<vec3d, uint> verts_map;
        for (uint fid : flist)
            for (uint vid : m.adj_f2v(fid))
                verts_map[m.vert(vid)] = vid;
        std::vector<uint> vlist;
        for (uint vid : new_face.get_ordered_boundary_vertices()) {
            auto it = verts_map.find(new_face.vert(vid));
            assert(it != verts_map.end());
            vlist.push_back(it->second);
        }
        uint fid_new = m.face_add(vlist);
        m.face_data(fid_new).label = label;

        /* update the list of polys to add and to remove */
        std::vector<uint> plist = m.adj_f2p(flist.front());
        bool w_ref = m.poly_face_winding(plist.front(), flist.front());
        for (uint pid : plist) {
          std::pair<uint, bool> f{fid_new, w_ref};
          polys2add[pid].push_back(f);
          w_ref = !w_ref;
        }
        polys2remove.insert(polys2remove.end(), plist.begin(), plist.end());
        ++l;
    }
    else { /* bad elements receive label -1 and are not agglomerated */
      for (uint fid : flist) {
        m.face_data(fid).label = -1;
        labels[-1].push_back(fid);
      }
      l = labels.erase(l);
      ++bad_guys;
    }
  }
  if (bad_guys != 0)
    std::cout << "mesh_agglomeration: refused to merge " << bad_guys << " faces!\n";

  /* add the new polys to the mesh */
  for (int pid = 0; pid < m.num_polys(); ++pid) {
    std::vector<uint> flist;
    std::vector<bool> fwinding;
    if (DOES_NOT_CONTAIN(polys2add, pid)) continue;

    for (auto poly : polys2add[pid]) { /* add aggregated faces */
      flist.push_back(poly.first);
      fwinding.push_back(poly.second);
    }
    for (uint fid : m.adj_p2f(pid)) { /* add original faces with label -1 */
      if (m.face_data(fid).label == -1) {
        flist.push_back(fid);
        fwinding.push_back(m.poly_face_winding(pid, fid));
      }
    }
    assert(flist.size() > 3);
    uint pid_new = m.poly_add(flist, fwinding);
    m.poly_data(pid_new).label = m.poly_data(pid).label;
  }

  /* remove old polys and unreferenced verts from the mesh */
  REMOVE_DUPLICATES_FROM_VEC(polys2remove);
  m.polys_remove(polys2remove);
  for (int vid = m.num_verts() - 1; vid >= 0; vid--)
    if (m.adj_v2e(vid).size() == 0)
      m.vert_remove_unreferenced(vid);
  assert(m.poly_fix_orientation());
}

} // namespace cinolib
