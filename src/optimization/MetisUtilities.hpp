#ifndef __METIS_UTILITIES_H
#define __METIS_UTILITIES_H

#include "Eigen/Eigen"
#include <cinolib/meshes/meshes.h>
#include <vector>

namespace Gedim
{
  /// Class for mesh used by METIS
  class IMeshDAO
  {
    public:
      virtual ~IMeshDAO() { }

      /// Mesh num total edges
      virtual unsigned int Cell1DTotalNumber() const = 0;
      /// Mesh num total polygons
      virtual unsigned int Cell2DTotalNumber() const = 0;
      /// Mesh num total polyhedrons
      virtual unsigned int Cell3DTotalNumber() const = 0;
      /// For each mesh edge identified by cell1DIndex get the number of neighbour polygons
      virtual unsigned int Cell1DNumberNeighbourCell2D(const unsigned int& cell1DIndex) const = 0;
      /// For each mesh edge identified by cell1DIndex return true if the neighbour polygon
      /// at position neighbourIndex exists
      virtual bool Cell1DHasNeighbourCell2D(const unsigned int& cell1DIndex,
                                            const unsigned int& neighbourIndex) const = 0;
      /// For each mesh edge identified by cell1DIndex get the cell2DIndex of the neighbour polygon
      /// at position neighbourIndex
      virtual unsigned int Cell1DNeighbourCell2D(const unsigned int& cell1DIndex,
                                                 const unsigned int& neighbourIndex) const = 0;
      /// For each mesh polygon identified by cell2DIndex get the number of neighbour polyhedrons
      virtual unsigned int Cell2DNumberNeighbourCell3D(const unsigned int& cell2DIndex) const = 0;
      /// For each mesh polygon identified by cell2DIndex return true if the neighbour polyhedron
      /// at position neighbourIndex exists
      virtual bool Cell2DHasNeighbourCell3D(const unsigned int& cell2DIndex,
                                            const unsigned int& neighbourIndex) const = 0;
      /// For each mesh polygon identified by cell2DIndex get the cell3DIndex of the neighbour polyhedron
      /// at position neighbourIndex
      virtual unsigned int Cell2DNeighbourCell3D(const unsigned int& cell2DIndex,
                                                 const unsigned int& neighbourIndex) const = 0;

  };


  class cino2gedim_srf : public Gedim::IMeshDAO {
  private:
    cinolib::Polygonmesh<> &m;

  public:
    cino2gedim_srf(cinolib::Polygonmesh<> &m) : m(m) {}
    unsigned int Cell1DTotalNumber() const { return m.num_edges(); }
    unsigned int Cell2DTotalNumber() const { return m.num_polys(); }
    unsigned int Cell3DTotalNumber() const { return 0; }
    unsigned int Cell1DNumberNeighbourCell2D(const unsigned int &cell1DIndex) const {
      return m.adj_e2p(cell1DIndex).size();
    }
    bool Cell1DHasNeighbourCell2D(const unsigned int &cell1DIndex,
                                  const unsigned int &neighbourIndex) const {
      std::vector<unsigned int> polys = m.adj_e2p(cell1DIndex);
      return (neighbourIndex < polys.size());
    }
    unsigned int Cell1DNeighbourCell2D(const unsigned int &cell1DIndex,
                                       const unsigned int &neighbourIndex) const {
      return m.adj_e2p(cell1DIndex).at(neighbourIndex);
    }
    unsigned int Cell2DNumberNeighbourCell3D(const unsigned int &cell2DIndex) const {
      return 0;
    }
    bool Cell2DHasNeighbourCell3D(const unsigned int &cell2DIndex,
                                  const unsigned int &neighbourIndex) const {
      return false;
    }
    unsigned int Cell2DNeighbourCell3D(const unsigned int &cell2DIndex,
                                       const unsigned int &neighbourIndex) const {
      return 0;
    }
  };

  class cino2gedim_vol : public Gedim::IMeshDAO {
  private:
    cinolib::Polyhedralmesh<> &m;

  public:
    cino2gedim_vol(cinolib::Polyhedralmesh<> &m) : m(m) {}
    unsigned int Cell1DTotalNumber() const { return m.num_edges(); }
    unsigned int Cell2DTotalNumber() const { return m.num_faces(); }
    unsigned int Cell3DTotalNumber() const { return m.num_polys(); }
    unsigned int Cell1DNumberNeighbourCell2D(const unsigned int &cell1DIndex) const {
      return m.adj_e2f(cell1DIndex).size();
    }
    bool Cell1DHasNeighbourCell2D(const unsigned int &cell1DIndex,
                                  const unsigned int &neighbourIndex) const {
      std::vector<unsigned int> faces = m.adj_e2f(cell1DIndex);
      return (neighbourIndex < faces.size());
    }
    unsigned int Cell1DNeighbourCell2D(const unsigned int &cell1DIndex,
                                       const unsigned int &neighbourIndex) const {
      return m.adj_e2f(cell1DIndex).at(neighbourIndex);
    }
    unsigned int Cell2DNumberNeighbourCell3D(const unsigned int &cell2DIndex) const {
      return m.adj_f2p(cell2DIndex).size();
    }
    bool Cell2DHasNeighbourCell3D(const unsigned int &cell2DIndex,
                                  const unsigned int &neighbourIndex) const {
      std::vector<unsigned int> polys = m.adj_f2p(cell2DIndex);
      return (neighbourIndex < polys.size());
    }
    unsigned int Cell2DNeighbourCell3D(const unsigned int &cell2DIndex,
                                       const unsigned int &neighbourIndex) const {
      return m.adj_f2p(cell2DIndex).at(neighbourIndex);
    }
  };

  class MetisUtilities final
  {
    public:
      struct NetworkPartitionOptions final
      {
          enum struct PartitionTypes
          {
            Unknown = -1,
            CutBalancing = 0,
            VolBalancing = 1
          };

          enum struct CoarseningSchemes
          {
            Default = -1, // METIS_CTYPE_SHEM
            METIS_CTYPE_RM = 0,
            METIS_CTYPE_SHEM = 1
          };

          enum struct InitialPartitioningSchemes
          {
            Default = -1, // METIS_IPTYPE_METISRB
            METIS_IPTYPE_GROW = 0,
            METIS_IPTYPE_RANDOM = 1,
            METIS_IPTYPE_EDGE = 2,
            METIS_IPTYPE_NODE = 3,
            METIS_IPTYPE_METISRB = 4
          };

          enum struct RefinementSchemes
          {
            Default = -1, // METIS_RTYPE_GREEDY
            METIS_RTYPE_FM = 0,
            METIS_RTYPE_GREEDY = 1,
            METIS_RTYPE_SEP2SIDED = 2,
            METIS_RTYPE_SEP1SIDED = 3
          };

          enum struct DebugLevels
          {
            None = -1,
            METIS_DBG_INFO       = 1,       /*!< Shows various diagnostic messages */
            METIS_DBG_TIME       = 2,       /*!< Perform timing analysis */
            METIS_DBG_COARSEN    = 4,	  /*!< Show the coarsening progress */
            METIS_DBG_REFINE     = 8,	  /*!< Show the refinement progress */
            METIS_DBG_IPART      = 16, 	  /*!< Show info on initial partitioning */
            METIS_DBG_MOVEINFO   = 32, 	  /*!< Show info on vertex moves during refinement */
            METIS_DBG_SEPINFO    = 64, 	  /*!< Show info on vertex moves during sep refinement */
            METIS_DBG_CONNINFO   = 128,     /*!< Show info on minimization of subdomain connectivity */
            METIS_DBG_CONTIGINFO = 256,     /*!< Show info on elimination of connected components */
            METIS_DBG_MEMORY     = 2048     /*!< Show info related to wspace allocation */
          };

          PartitionTypes PartitionType = PartitionTypes::Unknown;
          CoarseningSchemes CoarseningSchema = CoarseningSchemes::Default;
          RefinementSchemes RefinementSchema = RefinementSchemes::Default;
          InitialPartitioningSchemes InitialPartitioningSchema = InitialPartitioningSchemes::Default;
          DebugLevels DebugLevel = DebugLevels::None;
          unsigned int NumberOfParts = 0;
          unsigned int NumberRefinementIterations = 10;
          bool ContigousPartitions = true;
          bool CompressGraph = false;
          bool MinimizeConnectivity = false;
          int RandomSeed = -1;
          unsigned int MasterWeight = 100; ///< 0 de-activated; 100 activated totally
      };

      struct MetisNetwork final
      {
          struct MetisAdjacency final
          {
              std::vector<unsigned int> Rows;
              std::vector<unsigned int> Cols;
          };

          MetisAdjacency Adjacency;
          std::vector<bool> EdgesConstrained;
          std::vector<unsigned int> NodesWeight = {};
          std::vector<unsigned int> EdgesWeight = {};
      };

      struct MeshToNetwork final
      {
          std::vector<unsigned int> EdgesMeshCellIndex;
          MetisNetwork Network;
      };

    public:
      MetisUtilities();
      ~MetisUtilities();


      MetisUtilities::MeshToNetwork Mesh3DToDualGraph(const IMeshDAO& mesh,
                                                      const std::vector<unsigned int>& cell3DsWeight = {},
                                                      const std::vector<bool>& cell2DsConstrained = {},
                                                      const Eigen::SparseMatrix<unsigned int>& networkEdgesWeight = Eigen::SparseMatrix<unsigned int>()) const;
      MetisUtilities::MeshToNetwork Mesh2DToDualGraph(const IMeshDAO& mesh,
                                                      const std::vector<unsigned int>& cell2DsWeight = {},
                                                      const std::vector<bool>& cell1DsConstrained = {},
                                                      const Eigen::SparseMatrix<unsigned int>& networkEdgesWeight = Eigen::SparseMatrix<unsigned int>()) const;
      MetisUtilities::MetisNetwork MeshToGraph(const unsigned int& numVertices,
                                               const Eigen::MatrixXi& edges,
                                               const bool& undirectEdges,
                                               const std::vector<unsigned int>& verticesWeight = {},
                                               const std::vector<unsigned int>& edgesWeight = {}) const;

      MetisNetwork::MetisAdjacency GraphAdjacencyToMetisAdjacency(const std::vector<std::vector<unsigned int>>& graphAdjacency) const;
      std::vector<std::vector<unsigned int>> MetisAdjacencyToGraphAdjacency(const MetisNetwork::MetisAdjacency& metisAdjacency) const;

      std::vector<unsigned int> NetworkPartition(const NetworkPartitionOptions& options,
                                                 const MetisNetwork& network) const;

      std::vector<unsigned int> PartitionCheckConstraints(const MetisNetwork& network,
                                                          const std::vector<unsigned int>& partitions) const;
      std::vector<unsigned int> PartitionCheckConnectedComponents(const MetisNetwork& network,
                                                                  const std::vector<unsigned int>& partitions) const;
  };

  class TestNetworkPartition
  {
    public:
      void Test2D() const
      {
        Gedim::MetisUtilities metisUtilities;

        const unsigned int numberVertices = 6;
        Eigen::MatrixXi edges(2, 6);
        edges.col(0)<< 0, 1;
        edges.col(1)<< 1, 2;
        edges.col(2)<< 1, 3;
        edges.col(3)<< 1, 5;
        edges.col(4)<< 2, 4;
        edges.col(5)<< 2, 5;

        std::vector<unsigned int> verticesWeights(numberVertices, 1.0);
        verticesWeights[3] = 100.0;
        verticesWeights[4] = 100.0;
        verticesWeights[5] = 100.0;

        std::vector<unsigned int> edgesWeights(edges.cols(), 1.0);
        edgesWeights[3] = 100.0;
        edgesWeights[4] = 100.0;
        edgesWeights[5] = 100.0;

        const Gedim::MetisUtilities::MetisNetwork network = metisUtilities.MeshToGraph(numberVertices,
                                                                                       edges,
                                                                                       true,
                                                                                       verticesWeights,
                                                                                       edgesWeights);


        // ASSERT_EQ(std::vector<unsigned int>({ 0,1,5,8,9,10,12 }), network.Adjacency.Rows);
        // ASSERT_EQ(std::vector<unsigned int>({ 1,0,2,3,5,1,4,5,1,2,1,2 }), network.Adjacency.Cols);

        Gedim::MetisUtilities::NetworkPartitionOptions partitionOptions;
        partitionOptions.PartitionType = Gedim::MetisUtilities::NetworkPartitionOptions::PartitionTypes::CutBalancing;
        partitionOptions.NumberOfParts = 2;

        const std::vector<unsigned int> partitions = metisUtilities.NetworkPartition(partitionOptions,
                                                                                     network);

        const std::vector<unsigned int> fix_constraints_partitions = metisUtilities.PartitionCheckConstraints(network,
                                                                                                              partitions);

        const std::vector<unsigned int> fix_connectedComponents_partitions = metisUtilities.PartitionCheckConnectedComponents(network,
                                                                                                                              fix_constraints_partitions);

        // ASSERT_EQ(std::vector<unsigned int>({ 1, 1, 0, 1, 0, 0 }), partitions);
      }
  };
}

#ifndef CINO_STATIC_LIB
#include "MetisUtilities.cpp"
#endif

#endif // __METIS_UTILITIES_H

