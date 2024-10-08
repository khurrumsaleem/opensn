// SPDX-FileCopyrightText: 2024 The OpenSn Authors <https://open-sn.github.io/opensn/>
// SPDX-License-Identifier: MIT

#include "lua/framework/lua.h"
#include "lua/framework/mesh/field_function_interpolation/ffinterpol.h"
#include "lua/framework/console/console.h"
#include "framework/field_functions/interpolation/ffinter_point.h"
#include "framework/field_functions/interpolation/ffinter_line.h"
#include "framework/field_functions/interpolation/ffinter_volume.h"
#include "framework/runtime.h"
#include "framework/logging/log.h"

using namespace opensn;

namespace opensnlua
{

RegisterLuaFunctionInNamespace(FFInterpolationCreate, fieldfunc, FFInterpolationCreate);
RegisterLuaConstant(SLICE, Varying(1));
RegisterLuaConstant(LINE, Varying(2));
RegisterLuaConstant(VOLUME, Varying(3));
RegisterLuaConstant(POINT, Varying(4));

int
FFInterpolationCreate(lua_State* L)
{
  // Process types
  auto ffitype = LuaArg<int>(L, 1);
  if (ffitype == static_cast<int>(FieldFunctionInterpolationType::POINT))
  {
    auto new_ffi = std::make_shared<FieldFunctionInterpolationPoint>();

    opensn::field_func_interpolation_stack.emplace_back(new_ffi);
    const size_t index = opensn::field_func_interpolation_stack.size() - 1;
    opensn::log.LogAllVerbose2() << "Created point Field Function Interpolation";
    return LuaReturn(L, index);
  }
  else if (ffitype == static_cast<int>(FieldFunctionInterpolationType::LINE))
  {
    auto new_ffi = std::make_shared<FieldFunctionInterpolationLine>();

    opensn::field_func_interpolation_stack.emplace_back(new_ffi);
    const size_t index = opensn::field_func_interpolation_stack.size() - 1;
    opensn::log.LogAllVerbose2() << "Created line Field Function Interpolation";
    return LuaReturn(L, index);
  }
  else if (ffitype == static_cast<int>(FieldFunctionInterpolationType::VOLUME))
  {
    auto new_ffi = std::make_shared<FieldFunctionInterpolationVolume>();

    opensn::field_func_interpolation_stack.emplace_back(new_ffi);
    const size_t index = opensn::field_func_interpolation_stack.size() - 1;
    opensn::log.LogAllVerbose2() << "Created Volume Field Function Interpolation";
    return LuaReturn(L, index);
  }
  else // Fall back
    throw std::logic_error("Invalid FFITypeIndex used in FFInterpolationCreate.");
}

} // namespace opensnlua
