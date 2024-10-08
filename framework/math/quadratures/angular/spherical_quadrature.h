// SPDX-FileCopyrightText: 2024 The OpenSn Authors <https://open-sn.github.io/opensn/>
// SPDX-License-Identifier: MIT

#pragma once

#include "framework/math/quadratures/angular/curvilinear_quadrature.h"
#include "framework/math/quadratures/gauss_quadrature.h"

namespace opensn
{

class SphericalQuadrature : public CurvilinearQuadrature
{
private:
  /// Initialize with one-dimensional quadrature.
  void Initialize(const GaussQuadrature& quad_polar, const bool verbose = false);

  /**
   * Initialize parametrizing factors of the spherical angular quadrature, starting from a fully
   * initialized underlying product quadrature.
   */
  void InitializeParameters();

public:
  /**
   * Effective constructor. Initialize with one-dimensional quadrature. If not already present in
   * the quadrature, the method inserts the starting directions.
   */
  SphericalQuadrature(const GaussQuadrature& quad_polar, const bool verbose = false);

  virtual ~SphericalQuadrature() = default;

  void MakeHarmonicIndices(unsigned int scattering_order, int dimension) override;
};

} // namespace opensn
