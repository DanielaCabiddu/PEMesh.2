#ifndef HIERARCHY_H
#define HIERARCHY_H

#include <cinolib/cino_inline.h>
#include <cinolib/geometry/vec_mat.h>
#include <cinolib/meshes/meshes.h>
#include <map>

namespace cinolib {

class Hierarchy {

private:
  std::map<int, std::vector<uint>> polys;
  std::map<int, std::vector<uint>> faces;
  std::map<uint, uint> verts;

  template<class M, class V, class E, class P> CINO_INLINE
  void compute_verts(AbstractMesh<M,E,V,P> &m_old,
                     AbstractMesh<M,E,V,P> &m_new);

  void compute_polys(Polygonmesh<> &m_old,
                     Polygonmesh<> &m_new);

  void compute_polys(Polyhedralmesh<> &m_old,
                     Polyhedralmesh<> &m_new);

public:
  explicit Hierarchy() {};

  void compute(Polygonmesh<> &m_old,
               Polygonmesh<> &m_new);

  void compute(Polyhedralmesh<> &m_old,
               Polyhedralmesh<> &m_new);

  template<class M, class V, class E, class P> CINO_INLINE
  bool check(AbstractMesh<M,E,V,P> &m_old,
             AbstractMesh<M,E,V,P> &m_new);

  bool empty();

  void clear();

  void print(const std::string filename);
};

template <typename T> void rescale(T &H, const uint offset);

} // namespace cinolib

#ifndef CINO_STATIC_LIB
#include "hierarchy.cpp"
#endif

#endif // HIERARCHY_H
