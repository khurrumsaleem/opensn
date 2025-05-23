// SPDX-FileCopyrightText: 2024 The OpenSn Authors <https://open-sn.github.io/opensn/>
// SPDX-License-Identifier: MIT

#pragma once

#include "framework/mesh/mesh_generator/mesh_generator.h"

namespace opensn
{

/**
 * This class is responsible for generating a mesh, partitioning it, and distributing the
 * individual partitions to different MPI locations. The mesh is generated on location 0,
 * partitioned into multiple parts, serialized, and distributed to all other MPI ranks.
 *
 * The mesh data is serialized into a `ByteArray` for efficient MPI communication and
 * deserialized at the receiving locations to reconstruct the mesh locally.
 */
class DistributedMeshGenerator : public MeshGenerator
{
public:
  /**
   * Constructor for the DistributedMeshGenerator.
   *
   * \param params Input parameters for mesh generation and distribution.
   */
  explicit DistributedMeshGenerator(const InputParameters& params);

  /**
   * Executes the mesh generation and distribution process.
   *
   * On location 0, the mesh is generated, partitioned, serialized, and distributed to
   * other MPI ranks. Other ranks receive the serialized mesh data, deserialize it,
   * and set up the local mesh.
   */
  std::shared_ptr<MeshContinuum> Execute() override;

private:
  /**
   * Structure holding distributed mesh data.
   *
   * This structure holds the essential information about the mesh that will be distributed
   * among MPI ranks, including cells, vertices, boundary ID mappings, and mesh attributes.
   */
  struct DistributedMeshData
  {
    /// Dimension of the mesh (2D, 3D).
    unsigned int dimension;
    /// The coordinate system of the mesh.
    CoordinateSystemType coord_sys;
    /// Type of mesh.
    MeshType mesh_type;
    /// Whether the mesh is extruded or not.
    bool extruded;
    /// Attributes for orthogonal mesh, if applicable.
    OrthoMeshAttributes ortho_attributes;

    /// Map of cells (partition ID, cell global ID).
    std::map<std::pair<int, uint64_t>, UnpartitionedMesh::LightWeightCell> cells;
    /// Map of vertices by global vertex ID.
    std::map<uint64_t, Vector3> vertices;
    /// Map of boundary IDs to boundary names.
    std::map<uint64_t, std::string> boundary_id_map;
    /// Number of global vertices in the mesh.
    size_t num_global_vertices;
  };

  /// The number of partitions for distributing the mesh.
  const int num_partitions_;

public:
  /**
   * Get the input parameters for this class.
   *
   * \return Input parameters for configuring the DistributedMeshGenerator.
   */
  static InputParameters GetInputParameters();
  static std::shared_ptr<DistributedMeshGenerator> Create(const ParameterBlock& params);

private:
  /**
   * Serializes and distributes the mesh data to other MPI ranks.
   *
   * The mesh data is serialized into a `ByteArray` and distributed via MPI. The partitioned
   * mesh is sent to different ranks based on the partitioning.
   *
   * \param cell_pids A vector of cell partition IDs.
   * \param umesh The unpartitioned mesh object containing mesh information.
   * \param num_partitions The number of partitions to distribute.
   * \return The serialized mesh data for location 0.
   */
  static ByteArray DistributeSerializedMeshData(const std::vector<int64_t>& cell_pids,
                                                const UnpartitionedMesh& umesh,
                                                int num_partitions);

  /**
   * Deserializes the mesh data from a `ByteArray`.
   *
   * This function reconstructs the mesh from the serialized data received from location 0.
   *
   * \param serial_data The serialized mesh data in a `ByteArray`.
   * \return The deserialized mesh data.
   */
  static DistributedMeshData DeserializeMeshData(ByteArray& serial_data);

  /**
   * Sets up the local mesh on each MPI rank from the deserialized mesh data.
   *
   * The mesh is reconstructed from the deserialized mesh data on each rank. The local mesh
   * is then prepared and added to the mesh stack.
   *
   * \param mesh_info The deserialized mesh data containing information about cells, vertices, and
   * boundaries.
   * \return A shared pointer to the local mesh.
   */
  static std::shared_ptr<MeshContinuum> SetupLocalMesh(DistributedMeshData& mesh_info);
};

} // namespace opensn
