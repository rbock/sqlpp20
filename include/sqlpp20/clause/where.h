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
template <typename Condition>
struct where_t {
  Condition _condition;
};

template <typename Condition>
struct nodes_of<where_t<Condition>> {
  using type = type_vector<Condition>;
};

template <typename Table>
constexpr auto clause_tag<where_t<Table>> = ::std::string_view{"where"};

export template <typename Condition, typename Statement>
class clause_base<where_t<Condition>, Statement> {
 public:
  template <typename OtherStatement>
  clause_base(const clause_base<where_t<Condition>, OtherStatement>& s)
      : _condition(s._condition) {}

  clause_base(const where_t<Condition>& f) : _condition(f._condition) {}

  Condition _condition;
};

template <typename Context, typename Condition, typename Statement>
[[nodiscard]] auto to_sql_string(
    Context& context, const clause_base<where_t<Condition>, Statement>& t) {
  return std::string(" WHERE ") + to_sql_string(context, t._condition);
}

struct unconditionally_t {};

template <>
constexpr auto clause_tag<unconditionally_t> = ::std::string_view{"where"};

template <typename Statement>
class clause_base<unconditionally_t, Statement> {
 public:
  template <typename OtherStatement>
  constexpr clause_base(
      const clause_base<unconditionally_t, OtherStatement>& s) {}

  constexpr clause_base(const unconditionally_t& f) {}
};

template <typename Context, typename Statement>
[[nodiscard]] auto to_sql_string(
    Context& context, const clause_base<unconditionally_t, Statement>& t) {
  return std::string{};
}

struct no_where_t {};

export template <typename Statement>
class clause_base<no_where_t, Statement> {
 public:
  template <typename OtherStatement>
  constexpr clause_base(const clause_base<no_where_t, OtherStatement>& s) {}

  constexpr clause_base() = default;

  [[nodiscard]] constexpr auto unconditionally() const {
    return new_statement(*this, unconditionally_t{});
  }

  template <BooleanExpression Condition>
  requires(not recursive_contains_aggregate(type_vector<>{},
                                            type_t<Condition>{}))
  [[nodiscard]] constexpr auto where(Condition condition) const {
    return new_statement(*this, where_t<Condition>{condition});
  }
};

export template <typename Context, typename Statement>
[[nodiscard]] auto to_sql_string(Context& context,
                                 const clause_base<no_where_t, Statement>&) {
  return std::string{};
}

export template <typename Condition>
[[nodiscard]] constexpr auto where(Condition&& condition) {
  return statement<no_where_t>{}.where(std::forward<Condition>(condition));
}

export [[nodiscard]] constexpr auto unconditionally() {
  return statement<no_where_t>{}.unconditionally();
}
}  // namespace sqlpp
