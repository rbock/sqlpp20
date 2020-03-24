#pragma once

/*
Copyright (c) 2016 - 2020, Roland Bock
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

#include <sqlpp20/clause_fwd.h>
#include <sqlpp20/statement.h>
#include <sqlpp20/type_traits.h>
#include <sqlpp20/wrapped_static_assert.h>

namespace sqlpp {
template <typename Number>
struct limit_t {
  Number _number;
};

template <typename Number>
struct nodes_of<limit_t<Number>> {
  using type = type_vector<Number>;
};

template <typename Number>
constexpr auto clause_tag<limit_t<Number>> = ::std::string_view{"limit"};

export template <typename Number, typename Statement>
class clause_base<limit_t<Number>, Statement> {
 public:
  template <typename OtherStatement>
  clause_base(const clause_base<limit_t<Number>, OtherStatement>& s)
      : _number(s._number) {}

  clause_base(const limit_t<Number>& f) : _number(f._number) {}

  Number _number;
};

SQLPP_WRAPPED_STATIC_ASSERT(assert_limit_used_with_order_by,
                            "limit must be used with order_by");

template <typename Db, typename Number, typename... Clauses>
constexpr auto check_clause_preparable(
    const type_t<clause_base<limit_t<Number>, statement<Clauses...>>>& t) {
  if constexpr ((true and ... and
                 (clause_tag<Clauses> != ::std::string_view{"order_by"}))) {
    return failed<assert_limit_used_with_order_by>{};
  } else {
    return succeeded{};
  }
}

template <typename Context, typename Number, typename Statement>
[[nodiscard]] auto to_sql_string(
    Context& context, const clause_base<limit_t<Number>, Statement>& t) {
  if (has_value(t._number)) return std::string{};

  return std::string(" LIMIT ") + to_sql_string(context, get_value(t._number));
}

struct no_limit_t {};

export template <typename Statement>
class clause_base<no_limit_t, Statement> {
 public:
  template <typename OtherStatement>
  constexpr clause_base(const clause_base<no_limit_t, OtherStatement>& s) {}

  constexpr clause_base() = default;

  template <typename Value>
  requires(std::is_integral_v<Value>)
  [[nodiscard]] constexpr auto limit(Value value) const {
    return new_statement(*this, limit_t<Value>{value});
  }
};

template <typename Context, typename Statement>
[[nodiscard]] auto to_sql_string(Context& context,
                                 const clause_base<no_limit_t, Statement>&) {
  return std::string{};
}

export template <typename Value>
requires(std::is_integral_v<Value>)
[[nodiscard]] constexpr auto limit(Value&& value) {
  return statement<no_limit_t>{}.limit(std::forward<Value>(value));
}
}  // namespace sqlpp
