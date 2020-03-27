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
#include <sqlpp20/tuple_to_sql_string.h>
#include <sqlpp20/type_traits.h>

#include <tuple>

namespace sqlpp {
template <typename... Expressions>
struct group_by_t {
  std::tuple<Expressions...> _expressions;
};

template <typename... Expressions>
struct nodes_of<group_by_t<Expressions...>> {
  using type = type_vector<Expressions...>;
};

template <typename... Expressions>
struct provided_aggregates_of<group_by_t<Expressions...>> {
  static constexpr auto value = type_vector<Expressions...>();
};

template <typename Table>
constexpr auto clause_tag<group_by_t<Table>> = ::std::string_view{"group_by"};

export template <typename... Expressions, typename Statement>
class clause_base<group_by_t<Expressions...>, Statement> {
 public:
  template <typename OtherStatement>
  clause_base(const clause_base<group_by_t<Expressions...>, OtherStatement>& s)
      : _expressions(s._expressions) {}

  clause_base(const group_by_t<Expressions...>& f) : _expressions(f._expressions) {}

  std::tuple<Expressions...> _expressions;
};

template <typename Context, typename... Expressions, typename Statement>
[[nodiscard]] auto to_sql_string(
    Context& context, const clause_base<group_by_t<Expressions...>, Statement>& t) {
  return std::string{" GROUP BY "} +
         tuple_to_sql_string(context, ", ",
                             std::tie(std::get<Expressions>(t._expressions)...));
}

struct no_group_by_t {};

export template <typename Statement>
class clause_base<no_group_by_t, Statement> {
 public:
  template <typename OtherStatement>
  constexpr clause_base(const clause_base<no_group_by_t, OtherStatement>& s) {}

  constexpr clause_base() = default;

  template <Expression... Expressions>
  requires(sizeof...(Expressions) > 0 
           and not recursive_contains_aggregate(type_vector<>{},
                                                type_vector<Expressions...>{}))
  [[nodiscard]] constexpr auto group_by(Expressions... expressions) const {
    return new_statement(*this,
                         group_by_t<Expressions...>{std::tuple(expressions...)});
  }
};

export template <typename Context, typename Statement>
[[nodiscard]] auto to_sql_string(
    Context& context, const clause_base<no_group_by_t, Statement>& t) {
  return std::string{};
}

export template <Expression... Expressions>
[[nodiscard]] constexpr auto group_by(Expressions... expressions) {
  return statement<no_group_by_t>{}.group_by(expressions...);
}
}  // namespace sqlpp
