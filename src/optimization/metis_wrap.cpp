#include "metis_wrap.h"

namespace cinolib {

#ifdef CINOLIB_USES_METIS

typedef Gedim::MetisUtilities Metis;

bool SHOW_MESH = false;

void costs_to_weights(const std::vector<double> &costs, std::vector<uint> &weights) {
    double c_min = *std::min_element(costs.begin(), costs.end());
    double c_max = *std::max_element(costs.begin(), costs.end());
    for (uint i = 0; i < costs.size(); ++i) {
        double c = costs.at(i);
        double c_scaled = (c_max - c_min > TOLL_1D) ? (c - c_min) / (c_max - c_min) : c;
        assert(0. <= c_scaled && c_scaled <= 1.);
        weights.at(i) = (uint)round(c_scaled * weights.size() / 100) + 1;
    }
}

Metis::NetworkPartitionOptions M_set_options(const uint n_parts) {
    Metis::NetworkPartitionOptions M_options;
    M_options.PartitionType = Metis::NetworkPartitionOptions::PartitionTypes::CutBalancing;
    M_options.ContigousPartitions = true;
    M_options.CompressGraph = true;
    M_options.MinimizeConnectivity = true;
    M_options.NumberOfParts = n_parts;
    M_options.RandomSeed = 10;
    return M_options;
}

CINO_INLINE
int metis_wrap_dual(Polygonmesh<> &m, const double parts,
                    double (*quality)(const std::vector<vec3d> &),
                    const bool N_WEIGHTS, const bool A_WEIGHTS) {
    std::vector<bool> arcConstrained(m.num_edges(), false);
    for (uint eid = 0; eid < m.num_edges(); eid++)
        arcConstrained.at(eid) = m.edge_data(eid).flags[CONSTRAINED];

    std::vector<uint> nodeWeights;
    if (N_WEIGHTS) {
        std::vector<double> nodeCosts(m.num_polys());
        for (uint pid = 0; pid < m.num_polys(); pid++) {
            nodeCosts.at(pid) = quality(m.poly_verts(pid));
        }
        nodeWeights.resize(m.num_polys());
        costs_to_weights(nodeCosts, nodeWeights);
    }

    Eigen::SparseMatrix<uint> arcWeights;
    if (A_WEIGHTS) {
        std::vector<double> arcCosts(m.num_edges());
        for (uint eid = 0; eid < m.num_edges(); eid++) {
            if (m.edge_is_boundary(eid)) continue;
            std::vector<uint> adj_polys = m.adj_e2p(eid);
            std::vector<vec3d> v;
            sum_polys(adj_polys, m, v);
            arcCosts.at(eid) = quality(v);
        }
        std::vector<uint> _arcWeights(m.num_edges());
        costs_to_weights(arcCosts, _arcWeights);

        std::list<Eigen::Triplet<uint>> triplets;
        for (uint eid = 0; eid < m.num_edges(); eid++) {
            int w = _arcWeights.at(eid);
            std::vector<uint> adj = m.adj_e2p(eid);
            triplets.push_back(Eigen::Triplet<uint>(adj.front(), adj.back(), w));
            triplets.push_back(Eigen::Triplet<uint>(adj.back(), adj.front(), w));
        }
        arcWeights.resize(m.num_polys(), m.num_polys());
        arcWeights.setFromTriplets(triplets.begin(), triplets.end());
        arcWeights.makeCompressed();
    }

    Metis M_utils;
    Gedim::cino2gedim_srf p(m);
    Metis::MeshToNetwork M_network = M_utils.Mesh2DToDualGraph(p, nodeWeights, arcConstrained, arcWeights);
    Metis::NetworkPartitionOptions M_options = M_set_options((uint)(m.num_polys() * parts / 100.));
    std::vector<uint> M_partition = M_utils.NetworkPartition(M_options, M_network.Network);
    std::vector<uint> M_fix_constraints = M_utils.PartitionCheckConstraints(M_network.Network, M_partition);
    std::vector<uint> M_fix_CC = M_utils.PartitionCheckConnectedComponents(M_network.Network, M_fix_constraints);
    std::vector<int> labels(std::begin(M_fix_CC), std::end(M_fix_CC));
    m.poly_apply_labels(labels);

    return *std::max_element(M_fix_CC.begin(), M_fix_CC.end()) + 1;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
int metis_wrap_dual(Polyhedralmesh<> &m, const double parts,
                    double (*quality)(const std::vector<vec3d> &, const std::vector<std::vector<uint>> &, const std::vector<vec3d> &),
                    const bool N_WEIGHTS, const bool A_WEIGHTS) {
    std::vector<bool> arcConstrained(m.num_faces(), false);
    for (uint fid = 0; fid < m.num_faces(); fid++)
        arcConstrained.at(fid) = m.face_data(fid).flags[CONSTRAINED];

    std::vector<uint> nodeWeights;
    if (N_WEIGHTS) {
        std::vector<double> nodeCosts(m.num_polys());
        for (uint pid = 0; pid < m.num_polys(); pid++) {
            std::vector<vec3d> v, n;
            std::vector<std::vector<uint>> f;
            m.poly_export_element(pid, v, f);
            for (uint fid : m.poly_faces_id(pid)) {
                n.push_back(m.poly_face_normal(pid, fid));
            }
            nodeCosts.at(pid) = quality(v, f, n);
        }
        nodeWeights.resize(m.num_polys());
        costs_to_weights(nodeCosts, nodeWeights);
    }

    Eigen::SparseMatrix<uint> arcWeights;
    if (A_WEIGHTS) {
        std::vector<double> arcCosts(m.num_faces());
        for (uint fid = 0; fid < m.num_faces(); fid++) {
            if (m.face_is_on_srf(fid)) continue;
            std::vector<uint> adj_polys = m.adj_f2p(fid);
            std::vector<vec3d> v, n;
            std::vector<std::vector<uint>> f;
            sum_polys(adj_polys, m, v, f, n);
            arcCosts.at(fid) = quality(v, f, n);
            if(arcCosts.at(fid) == 0) {
                std::cout << "Metis Warning: element with zero rho around face: " << fid
                          << ", pids: " << adj_polys.front() << ", " << adj_polys.back()
                          << std::flush;
            }
        }
        std::vector<uint> _arcWeights(m.num_faces());
        costs_to_weights(arcCosts, _arcWeights);

        std::list<Eigen::Triplet<uint>> triplets;
        for (uint fid = 0; fid < m.num_faces(); ++fid) {
            int w = _arcWeights.at(fid);
            std::vector<uint> adj = m.adj_f2p(fid);
            triplets.push_back(Eigen::Triplet<uint>(adj.front(), adj.back(), w));
            triplets.push_back(Eigen::Triplet<uint>(adj.back(), adj.front(), w));
        }
        arcWeights.resize(m.num_polys(), m.num_polys());
        arcWeights.setFromTriplets(triplets.begin(), triplets.end());
        arcWeights.makeCompressed();
    }

    Metis M_utils;
    Gedim::cino2gedim_vol p(m);
    Metis::MeshToNetwork M_network = M_utils.Mesh3DToDualGraph(p, nodeWeights, arcConstrained, arcWeights);
    Metis::NetworkPartitionOptions M_options = M_set_options((uint)(m.num_polys() * parts / 100.));
    std::vector<uint> M_partition = M_utils.NetworkPartition(M_options, M_network.Network);
    std::vector<uint> M_fix_constraints = M_utils.PartitionCheckConstraints(M_network.Network, M_partition);
    std::vector<uint> M_fix_CC = M_utils.PartitionCheckConnectedComponents(M_network.Network, M_fix_constraints);
    std::vector<int> labels(std::begin(M_fix_CC), std::end(M_fix_CC));
    m.poly_apply_labels(labels);

    return *std::max_element(M_fix_CC.begin(), M_fix_CC.end()) + 1;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
int metis_wrap_primal(Polygonmesh<> &m, const double parts,
                      double (*quality)(const std::vector<vec3d> &),
                      const bool N_WEIGHTS, const bool A_WEIGHTS) {
    std::vector<uint> nodeWeights;
    if (N_WEIGHTS) {
        std::vector<double> nodeCosts(m.num_verts());
        for (uint vid = 0; vid < m.num_verts(); vid++) {
            if (m.vert_is_boundary(vid)) continue;
            std::vector<uint> adj_polys = m.adj_v2p(vid);
            std::vector<vec3d> v;
            sum_polys(adj_polys, m, v);
            nodeCosts.at(vid) = quality(v);
        }
        nodeWeights.resize(m.num_verts());
        costs_to_weights(nodeCosts, nodeWeights);
    }

    std::vector<uint> arcWeights;
    if (A_WEIGHTS) {
        std::vector<double> arcCosts(m.num_edges());
        for (uint eid = 0; eid < m.num_edges(); eid++) {
            if (m.edge_is_boundary(eid)) continue;
            std::vector<uint> adj_polys = m.adj_e2p(eid);
            std::vector<vec3d> v;
            sum_polys(adj_polys, m, v);
            arcCosts.at(eid) = quality(v);
        }
        arcWeights.resize(m.num_edges());
        costs_to_weights(arcCosts, arcWeights);
    }

    Metis M_utils;
    Gedim::cino2gedim_srf p(m);
    Eigen::MatrixXi edges(2, m.num_edges());
    for (uint eid = 0; eid < m.num_edges(); ++eid) {
        edges.col(eid) << m.edge_vert_id(eid, 0), m.edge_vert_id(eid, 1);
    }
    Metis::MetisNetwork M_network = M_utils.MeshToGraph(m.num_verts(), edges, true, nodeWeights, arcWeights);
    Metis::NetworkPartitionOptions M_options = M_set_options((uint)(m.num_polys() * parts / 100.));
    std::vector<uint> M_partition = M_utils.NetworkPartition(M_options, M_network);
    std::vector<uint> M_fix_constraints = M_utils.PartitionCheckConstraints(M_network, M_partition);
    std::vector<uint> M_fix_CC = M_utils.PartitionCheckConnectedComponents(M_network, M_fix_constraints);
    std::vector<int> labels(std::begin(M_fix_CC), std::end(M_fix_CC));
    m.vert_apply_labels(labels);

    return *std::max_element(M_fix_CC.begin(), M_fix_CC.end()) + 1;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

CINO_INLINE
int metis_wrap_primal(Polyhedralmesh<> &m, const double parts,
                      double (*quality)(const std::vector<vec3d> &, const std::vector<std::vector<uint>> &, const std::vector<vec3d> &),
                      const bool N_WEIGHTS, const bool A_WEIGHTS) {
    std::vector<uint> nodeWeights;
    if (N_WEIGHTS) {
        std::vector<double> nodeCosts(m.num_verts());
        for (uint vid = 0; vid < m.num_verts(); vid++) {
            if (m.vert_is_on_srf(vid)) continue;
            std::vector<uint> adj_polys = m.adj_v2p(vid);
            std::vector<vec3d> v, n;
            std::vector<std::vector<uint>> f;
            sum_polys(adj_polys, m, v, f, n);
            nodeCosts.at(vid) = quality(v, f, n);
        }
        nodeWeights.resize(m.num_verts());
        costs_to_weights(nodeCosts, nodeWeights);
    }

    std::vector<uint> arcWeights;
    if (A_WEIGHTS) {
        std::vector<double> arcCosts(m.num_edges());
        for (uint eid = 0; eid < m.num_edges(); eid++) {
            std::vector<uint> adj_polys = m.adj_e2p(eid);
            if (m.edge_is_on_srf(eid)) continue;
            std::vector<vec3d> v, n;
            std::vector<std::vector<uint>> f;
            sum_polys(adj_polys, m, v, f, n);
            arcCosts.at(eid) = quality(v, f, n);
            // arcCosts.at(eid) = 1. / pow(1.0 - rho_poly(v), 3);
        }
        arcWeights.resize(m.num_edges());
        costs_to_weights(arcCosts, arcWeights);
    }

    Metis M_utils;
    Gedim::cino2gedim_vol p(m);
    Eigen::MatrixXi edges(2, m.num_edges());
    for (uint eid = 0; eid < m.num_edges(); ++eid) {
        edges.col(eid)<< m.edge_vert_id(eid, 0), m.edge_vert_id(eid, 1);
    }
    Metis::MetisNetwork M_network = M_utils.MeshToGraph(m.num_verts(), edges, true, nodeWeights, arcWeights);
    Metis::NetworkPartitionOptions M_options = M_set_options((uint)(m.num_polys() * parts / 100.));
    std::vector<uint> M_partition = M_utils.NetworkPartition(M_options, M_network);
    std::vector<uint> M_fix_constraints = M_utils.PartitionCheckConstraints(M_network, M_partition);
    std::vector<uint> M_fix_CC = M_utils.PartitionCheckConnectedComponents(M_network, M_fix_constraints);
    std::vector<int> labels(std::begin(M_fix_CC), std::end(M_fix_CC));
    m.vert_apply_labels(labels);

    return *std::max_element(M_fix_CC.begin(), M_fix_CC.end()) + 1;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

#else
CINO_INLINE
int metis_wrap_primal(Polygonmesh<> &m, const double parts,
                      double (*quality)(const std::vector<vec3d> &),
                      const bool N_WEIGHTS, const bool A_WEIGHTS) {
    std::cerr << "ERROR : Metis missing. Install Metis and recompile defining "
                 "symbol CINOLIB_USES_METIS" << std::endl;
    exit(-1);
}

int metis_wrap_dual(Polygonmesh<> &m, const double parts,
                    double (*quality)(const std::vector<vec3d> &),
                    const bool N_WEIGHTS, const bool A_WEIGHTS) {
    std::cerr << "ERROR : Metis missing. Install Metis and recompile defining "
                 "symbol CINOLIB_USES_METIS" << std::endl;
    exit(-1);
}

CINO_INLINE
int metis_wrap_primal(Polyhedralmesh<> &m, const double parts,
                      double (*quality)(const std::vector<vec3d> &, const std::vector<std::vector<uint>> &, const std::vector<vec3d> &),
                      const bool N_WEIGHTS, const bool A_WEIGHTS) {
    std::cerr << "ERROR : Metis missing. Install Metis and recompile defining "
                 "symbol CINOLIB_USES_METIS" << std::endl;
    exit(-1);
}

CINO_INLINE
int metis_wrap_dual(Polyhedralmesh<> &m, const double parts,
                    double (*quality)(const std::vector<vec3d> &, const std::vector<std::vector<uint>> &, const std::vector<vec3d> &),
                    const bool N_WEIGHTS, const bool A_WEIGHTS) {
    std::cerr << "ERROR : Metis missing. Install Metis and recompile defining "
                 "symbol CINOLIB_USES_METIS" << std::endl;
    exit(-1);
}
#endif

}; // namespace cinolib
