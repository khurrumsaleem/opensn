// SPDX-FileCopyrightText: 2024 The OpenSn Authors <https://open-sn.github.io/opensn/>
// SPDX-License-Identifier: MIT

#pragma once

#include "framework/object.h"
#include <functional>

namespace opensn
{

using ScalarScalarFunction = std::function<double(double)>;
using ScalarXYZTFunction = std::function<double(double, double, double, double)>;

class FunctionDimAToDimB : public Object
{
private:
  const size_t input_dimension_;
  const size_t output_dimension_;

public:
  static InputParameters GetInputParameters();
  explicit FunctionDimAToDimB(const InputParameters& params);

  size_t InputDimension() const { return input_dimension_; }
  size_t OutputDimension() const { return output_dimension_; }

  virtual bool HasSlope() const = 0;
  virtual bool HasCurvature() const = 0;

  virtual double ScalarFunction1Parameter(double) const;
  virtual double ScalarFunctionSlope1Parameter(double) const;
  virtual double ScalarFunctionCurvature1Parameter(double) const;

  virtual double ScalarFunction4Parameters(double, double, double, double) const;
  virtual double ScalarFunctionSlope4Parameters(double, double, double, double) const;
  virtual double ScalarFunctionCurvature4Parameters(double, double, double, double) const;

  virtual std::vector<double> Evaluate(const std::vector<double>& vals) const = 0;
  virtual std::vector<double> EvaluateSlope(const std::vector<double>& vals) const { return {0.0}; }
};

} // namespace opensn
