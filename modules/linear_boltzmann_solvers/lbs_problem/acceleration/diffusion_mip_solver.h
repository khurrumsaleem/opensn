// SPDX-FileCopyrightText: 2024 The OpenSn Authors <https://open-sn.github.io/opensn/>
// SPDX-License-Identifier: MIT

#pragma once

#include "modules/linear_boltzmann_solvers/lbs_problem/acceleration/diffusion.h"
#include "framework/math/functions/function.h"

namespace opensn
{
class MeshContinuum;
class Cell;
struct Vector3;
class SpatialDiscretization;
struct UnitCellMatrices;

/**
 * Generalized diffusion solver for both WGDSA and TGDSA based on the MIP-method
 * of Bruno Turcksin and Jean Ragusa.
 */
class DiffusionMIPSolver : public DiffusionSolver
{
public:
  DiffusionMIPSolver(std::string name,
                     const opensn::SpatialDiscretization& sdm,
                     const UnknownManager& uk_man,
                     std::map<uint64_t, BoundaryCondition> bcs,
                     MatID2XSMap map_mat_id_2_xs,
                     const std::vector<UnitCellMatrices>& unit_cell_matrices,
                     bool suppress_bcs,
                     bool verbose);
  virtual ~DiffusionMIPSolver() = default;

  inline void SetSourceFunction(const ScalarSpatialFunction& function)
  {
    source_function_ = function;
  }

  inline void SetReferenceSolutionFunction(const ScalarSpatialFunction& function)
  {
    ref_solution_function_ = function;
  }

  /**
   * Assembles both the matrix and the RHS using quadrature points. These routines exist for
   * implementing MMS.
   */
  void AssembleAand_b_wQpoints(const std::vector<double>& q_vector);

  /**
   * Assembles just the RHS using quadrature points. These routines exist for implementing MMS.
   */
  void Assemble_b_wQpoints(const std::vector<double>& q_vector);

  /**
   * Assembles both the matrix and the RHS using unit cell-matrices. These are the routines used in
   * the production versions.
   */
  void AssembleAand_b(const std::vector<double>& q_vector) override;

  /**
   * Assembles the RHS using unit cell-matrices. These are the routines used in the production
   * versions.
   */
  void Assemble_b(const std::vector<double>& q_vector) override;
  void Assemble_b(Vec petsc_q_vector) override;

  /**
   * Still searching for a reference for this.
   *
   * For Polygons:
   * Defined from paper  \n
   * Turcksin B, Ragusa J, "Discontinuous diffusion synthetic acceleration
   * for S_n transport on 2D arbitrary polygonal meshes", Journal of
   * Computational Physics 274, pg 356-369, 2014.\n
   * \n
   * Nv = Number of vertices. If Nv <= 4 then the perimeter parameter
   * should be replaced by edge length.
   */
  double HPerpendicular(const Cell& cell, unsigned int f);

  /// Maps a face, in a discontinuous sense, using the spatial discretization.
  int MapFaceNodeDisc(const Cell& cur_cell,
                      const Cell& adj_cell,
                      const std::vector<Vector3>& cc_node_locs,
                      const std::vector<Vector3>& ac_node_locs,
                      size_t ccf,
                      size_t acf,
                      size_t ccfi,
                      double epsilon = 1.0e-12);

private:
  ScalarSpatialFunction source_function_;
  ScalarSpatialFunction ref_solution_function_;
};

} // namespace opensn
