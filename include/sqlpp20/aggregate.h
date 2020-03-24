#pragma once

/*
Copyright (c) 2018 - 2020, Roland Bock
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <sqlpp20/operator/as.h>
#include <sqlpp20/to_sql_string.h>
#include <sqlpp20/type_traits.h>

namespace sqlpp {
export template <typename FunctionSpec, typename Expression>
struct aggregate_t {
  aggregate_t() = delete;
  constexpr aggregate_t(Expression expression) : _expression(expression) {}
  aggregate_t(const aggregate_t&) = default;
  aggregate_t(aggregate_t&&) = default;
  aggregate_t& operator=(const aggregate_t&) = default;
  aggregate_t& operator=(aggregate_t&&) = default;
  ~aggregate_t() = default;

  template <typename Alias>
  [[nodiscard]] constexpr auto as(const Alias& alias) const {
    return ::sqlpp::as(*this, alias);
  }

  Expression _expression;
};

template <typename FunctionSpec, typename Expression>
struct value_type_of<aggregate_t<FunctionSpec, Expression>> {
  using type = typename FunctionSpec::value_type;
};

template <typename FunctionSpec, typename Expression>
constexpr auto is_aggregate_v<aggregate_t<FunctionSpec, Expression>> = true;

template <typename Context, typename FunctionSpec, typename Expression>
[[nodiscard]] auto to_sql_string(
    Context& context, const aggregate_t<FunctionSpec, Expression>& t) {
  return std::string(FunctionSpec::name) + "(" +
         to_sql_string(context, typename FunctionSpec::flag_type{}) +
         to_sql_string(context, t._expression) + ")";
}

}  // namespace sqlpp
