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
#include <sqlpp20/free_column.h>
#include <sqlpp20/statement.h>
#include <sqlpp20/tuple_to_sql_string.h>
#include <sqlpp20/type_traits.h>

#include <tuple>

namespace sqlpp {
template <typename Assignment>
struct update_assignment_t {
  Assignment _assignment;
};

template <typename Context, typename Assignment>
[[nodiscard]] auto to_sql_string(
    Context& context, const update_assignment_t<Assignment>& assignment) {
  auto ret = to_sql_string(
      context, free_column_t<column_of_t<remove_optional_t<Assignment>>>{});
  if constexpr (::sqlpp::is_optional_v<Assignment>) {
    if (assignment._assignment)
      return ret + " = " +
             to_sql_string(context, assignment._assignment.value().value);
    else {
      return ret + " = " + ret;
    }
  } else {
    return ret + " = " + to_sql_string(context, assignment._assignment.value);
  }
}
}  // namespace sqlpp

namespace sqlpp {
template <typename... Assignments>
struct update_set_t {
  std::tuple<Assignments...> _assignments;
};

template <typename... Assignments>
struct nodes_of<update_set_t<Assignments...>> {
  using type = type_vector<Assignments...>;
};

template <typename... Assignments>
constexpr auto clause_tag<update_set_t<Assignments...>> =
    ::std::string_view{"update_set"};

export template <typename... Assignments, typename Statement>
class clause_base<update_set_t<Assignments...>, Statement> {
 public:
  template <typename OtherStatement>
  clause_base(
      const clause_base<update_set_t<Assignments...>, OtherStatement>& s)
      : _assignments(s._assignments) {}

  clause_base(const update_set_t<Assignments...>& f)
      : _assignments(f._assignments) {}

  std::tuple<Assignments...> _assignments;
};

template <typename... Assignments>
constexpr auto is_result_clause_v<update_set_t<Assignments...>> = true;

template <typename... Assignments>
struct clause_result_type<update_set_t<Assignments...>> {
  using type = update_result;
};

export template <typename Context, typename... Assignments, typename Statement>
[[nodiscard]] auto to_sql_string(
    Context& context,
    const clause_base<update_set_t<Assignments...>, Statement>& t) {
  return std::string{" SET "} +
         tuple_to_sql_string(context, ", ",
                             std::tuple(update_assignment_t<Assignments>{
                                 std::get<Assignments>(t._assignments)}...));
}

struct no_update_set_t {};

export template <typename Statement>
class clause_base<no_update_set_t, Statement> {
 public:
  template <typename OtherStatement>
  constexpr clause_base(const clause_base<no_update_set_t, OtherStatement>& s) {
  }

  constexpr clause_base() = default;

  template <OptionalAssignment... Assignments>
  requires(sizeof...(Assignments) > 0 and unique_assignment_columns_v<Assignments...>)
  [[nodiscard]] constexpr auto set(Assignments... assignments) const {
    return new_statement(
        *this, update_set_t<Assignments...>{std::tuple{assignments...}});
  }
};

export template <typename Context, typename Statement>
[[nodiscard]] auto to_sql_string(
    Context& context, const clause_base<no_update_set_t, Statement>&) {
  return std::string{};
}

export template <typename... Assignments>
[[nodiscard]] constexpr auto update_set(Assignments&&... assignments) {
  return statement<no_update_set_t>{}.set(
      std::forward<Assignments>(assignments)...);
}
}  // namespace sqlpp
