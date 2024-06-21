#include "hierarchy.h"

namespace cinolib {

template<class M, class V, class E, class P> CINO_INLINE
void Hierarchy::compute_verts(AbstractMesh<M,E,V,P> &m_old,
                              AbstractMesh<M,E,V,P> &m_new) {
  std::map<vec3d, uint> old_verts;
  for (uint vid_old = 0; vid_old < m_old.num_verts(); vid_old++) {
    vec3d v = m_old.vert(vid_old);
    old_verts[v] = vid_old;
  }

  for (uint vid_new = 0; vid_new < m_new.num_verts(); vid_new++) {
      vec3d v = m_new.vert(vid_new);
      uint children = old_verts[v];
      verts[vid_new] = children;
  }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Hierarchy::compute_polys(Polygonmesh<> &m_old,
                              Polygonmesh<> &m_new) {
    std::unordered_map<int, std::vector<uint>> old_polys;
    for (uint pid_old = 0; pid_old < m_old.num_polys(); pid_old++) {
        int l = m_old.poly_data(pid_old).label;
        old_polys[l].push_back(pid_old);
    }

    for (uint pid_new = 0; pid_new < m_new.num_polys(); pid_new++) {
      /* add to children the old polys with label l. */
      int l = m_new.poly_data(pid_new).label;
      std::vector<uint> children = old_polys[l];

      /* if children is empty it means that the new poly has not been agglomerated
       * and has received a new label, hence there has to exist an old poly with
       * the same vertices. */
      if (children.empty()) {
        std::vector<uint> vids_new = m_new.poly_verts_id(pid_new);
        std::vector<uint> vids_old;
        for (uint vid : vids_new)
          vids_old.push_back(verts[vid]);
        uint pid_old = m_old.poly_id(vids_old);
        assert(pid_old != -1);
        children.push_back(pid_old);
      }
      polys[pid_new] = children;
    }
}

CINO_INLINE
int m_poly_id_by_verts(Polyhedralmesh<> &m, const std::vector<uint> & vlist) {
    if(vlist.empty()) return -1;
    std::vector<uint> query = SORT_VEC(vlist);

    uint vid = vlist.front();
    for(uint pid : m.adj_v2p(vid))
    {
        if(m.poly_verts_id(pid,true)==query) return pid;
    }
    return -1;
}

CINO_INLINE
void Hierarchy::compute_polys(Polyhedralmesh<> &m_old,
                              Polyhedralmesh<> &m_new) {
    std::unordered_map<int, std::vector<uint>> old_polys;
    for (uint pid_old = 0; pid_old < m_old.num_polys(); pid_old++) {
        int l = m_old.poly_data(pid_old).label;
        old_polys[l].push_back(pid_old);
    }

    for (uint pid_new = 0; pid_new < m_new.num_polys(); pid_new++) {
      /* add to children the old polys with label l. */
      int l = m_new.poly_data(pid_new).label;
      std::vector<uint> children = old_polys[l];

      /* if children is empty it means that the new poly has not been agglomerated
       * and has received a new label, hence there has to exist an old poly with
       * the same vertices. */
      if (children.empty()) {
        std::vector<uint> vids_new = m_new.poly_verts_id(pid_new);
        std::vector<uint> vids_old;
        for (uint vid : vids_new)
          vids_old.push_back(verts[vid]);
        int pid_old = m_poly_id_by_verts(m_old, vids_old);
        assert(pid_old != -1);
        children.push_back(pid_old);
      }
      polys[pid_new] = children;
    }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Hierarchy::compute(Polygonmesh<> &m_old,
                        Polygonmesh<> &m_new) {
  compute_verts(m_old, m_new);
  compute_polys(m_old, m_new);
}

CINO_INLINE
void Hierarchy::compute(Polyhedralmesh<> &m_old,
                        Polyhedralmesh<> &m_new) {
  compute_verts(m_old, m_new);
  compute_polys(m_old, m_new);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

template<class M, class V, class E, class P> CINO_INLINE
bool Hierarchy::check(AbstractMesh<M,E,V,P> &m_old,
                      AbstractMesh<M,E,V,P> &m_new) {
  for (auto &p : polys) {
    uint pid_new = p.first;
    std::vector<uint> pids_old = p.second;
    if (pids_old.size() == 1)
      continue;
    for (uint pid_old : pids_old)
      if (m_old.poly_data(pid_old).label != m_new.poly_data(pid_new).label)
        return false;
  }

  for (auto v : verts) {
    uint vid_new = v.first;
    uint vid_old = v.second;
    if (m_new.vert(vid_new).dist(m_old.vert(vid_old)) != 0)
      return false;
  }
  return true;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Hierarchy::clear() {
  polys.clear();
  faces.clear();
  verts.clear();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
bool Hierarchy::empty() { return polys.empty(); }

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
void Hierarchy::print(const std::string filename) {
  if (polys.empty())
      return;
  FILE *fp = fopen(filename.c_str(), "w");
  assert(fp);

  /* polys hierarchy */
  fprintf(fp, "numCells: %d\n# newCellId, sizeOldCellIdsContainer, oldCellIds\n", (int)polys.size());
  for (auto &father : polys) {
    std::vector<uint> children = father.second;
    fprintf(fp, "%d, %d, ", father.first, (int)children.size());
    assert(!children.empty());
    for (uint i = 0; i < children.size(); i++) {
      fprintf(fp, "%d", children.at(i));
      fprintf(fp, (i != children.size() - 1) ? ", " : "\n");
    }
  }

  /* verts hierarchy */
  fprintf(fp, "\nnumVerts: %d\n# newVertId, oldVertId\n", (int)verts.size());
  for (auto &father : verts)
    fprintf(fp, "%d, %d\n", father.first, father.second);
  fclose(fp);
  return;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

} // namespace cinolib
