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

#include <sqlpp20/concepts.h>
#include <sqlpp20/clause_fwd.h>
#include <sqlpp20/default_value.h>
#include <sqlpp20/detail/first.h>
#include <sqlpp20/exception.h>
#include <sqlpp20/free_column.h>
#include <sqlpp20/statement.h>
#include <sqlpp20/tuple_to_sql_string.h>
#include <sqlpp20/type_traits.h>
#include <sqlpp20/type_vector_is_subset_of.h>

#include <tuple>
#include <vector>

namespace sqlpp {
template <typename Assignment>
struct insert_assignment_t {
  Assignment _assignment;
};

template <typename Context, typename Assignment>
[[nodiscard]] auto to_sql_string(
    Context& context, const insert_assignment_t<Assignment>& assignment) {
  if constexpr (::sqlpp::is_optional_v<Assignment>) {
    if (assignment._assignment) {
      return to_sql_string(context, assignment._assignment.value().value);
    } else {
      return to_sql_string(context, ::sqlpp::default_value);
    }
  } else {
    return to_sql_string(context, assignment._assignment.value);
  }
}
}  // namespace sqlpp

namespace sqlpp {
template <typename... Assignments>
struct insert_values_t {
  std::tuple<Assignments...> _assignments;
};

template <typename... Assignments>
struct nodes_of<insert_values_t<Assignments...>> {
  using type = type_vector<Assignments...>;
};

template <typename... Assignments>
constexpr auto clause_tag<insert_values_t<Assignments...>> =
    ::std::string_view{"insert_values"};

export template <typename Statement, typename... Assignments>
class clause_base<insert_values_t<Assignments...>, Statement> {
 public:
  template <typename OtherStatement>
  clause_base(
      const clause_base<insert_values_t<Assignments...>, OtherStatement>& s)
      : _assignments(s._assignments) {}

  clause_base(const insert_values_t<Assignments...>& f)
      : _assignments(f._assignments) {}

  std::tuple<Assignments...> _assignments;
};

namespace detail {
template <typename... Ls, typename Pred, typename... Rs>
constexpr auto some_elements_of_L_matching_P_are_not_in_R(
    ::sqlpp::type_vector<Ls...>, Pred, ::sqlpp::type_vector<Rs...>) -> bool {
  return (false or ... or
          (Pred::template value<Ls> and detail::is_not_in_rhs<Ls, Rs...>()));
}

template <typename... Ls, typename Pred, typename... Rs>
constexpr auto some_elements_of_L_do_not_match_P(::sqlpp::type_vector<Ls...>,
                                                 Pred) -> bool {
  return (false or ... or (not Pred::template value<Ls>));
}
}  // namespace detail

#warning: Move to creation of insert values?
template <typename Db, typename Statement, typename... Assignments>
constexpr auto check_clause_preparable(
    const type_t<clause_base<insert_values_t<Assignments...>, Statement>>& t) -> bool {
  using _table_t = typename Statement::insert_into_table_t;
  constexpr auto _set_columns =
      type_vector<column_of_t<remove_optional_t<Assignments>>...>{};

  if constexpr (::sqlpp::detail::some_elements_of_L_matching_P_are_not_in_R(
                    columns_of_v<_table_t>, ::sqlpp::is_insert_required_pred,
                    _set_columns)) {
    static_assert(sizeof(t) == 0, "at least one required column is missing in set()");
    return false;
  } else {
    return true;
  }
}

export template <typename Context, typename Statement, typename... Assignments>
[[nodiscard]] auto to_sql_string(
    Context& context,
    const clause_base<insert_values_t<Assignments...>, Statement>& t) {
  auto ret = std::string{};
  // columns
  {
    ret += " (";
    ret += tuple_to_sql_string(
        context, ", ",
        std::tuple(
            free_column_t<column_of_t<remove_optional_t<Assignments>>>{}...));
    ret += ")";
  }

  // values
  {
    ret += " VALUES (";
    ret += tuple_to_sql_string(context, ", ",
                               std::tuple(insert_assignment_t<Assignments>{
                                   std::get<Assignments>(t._assignments)}...));
    ret += ")";
  }

  return ret;
}

struct insert_default_values_t {};

template <>
constexpr auto clause_tag<insert_default_values_t> =
    ::std::string_view{"insert_values"};

export template <typename Statement>
class clause_base<insert_default_values_t, Statement> {
 public:
  template <typename OtherStatement>
  clause_base(const clause_base<insert_default_values_t, OtherStatement>& s) {}

  clause_base(const insert_default_values_t& f) {}
};

template <typename Db, typename Statement>
constexpr auto check_clause_preparable(
    const type_t<clause_base<insert_default_values_t, Statement>>& t) -> bool {
  using _table_t = typename Statement::insert_into_table_t;

  if constexpr (::sqlpp::detail::some_elements_of_L_do_not_match_P(
                    columns_of_v<_table_t>, ::sqlpp::has_default_pred)) {
    static_assert(sizeof(t) == 0, "default_values() requires all columns to have a default value");
    return false;
  } else {
    return true;
  }
}

export template <typename Context, typename Statement>
[[nodiscard]] auto to_sql_string(
    Context& context,
    const clause_base<insert_default_values_t, Statement>& t) {
  return std::string{" DEFAULT VALUES"};
}

template <typename... Assignments>
struct insert_multi_values_t {
  std::vector<std::tuple<Assignments...>> _rows;
};

template <typename... Assignments>
struct nodes_of<insert_multi_values_t<Assignments...>> {
  using type = type_vector<Assignments...>;
};

template <typename... Assignments>
constexpr auto clause_tag<insert_multi_values_t<Assignments...>> =
    ::std::string_view{"insert_values"};

export template <typename Statement, typename... Assignments>
class clause_base<insert_multi_values_t<Assignments...>, Statement> {
 public:
  template <typename OtherStatement>
  clause_base(const clause_base<insert_multi_values_t<Assignments...>,
                                OtherStatement>& s)
      : _rows(s._rows) {}

  clause_base(const insert_multi_values_t<Assignments...>& f)
      : _rows(f._rows) {}

  std::vector<std::tuple<Assignments...>> _rows;
};

template <typename Db, typename Statement, typename... Assignments>
constexpr auto check_clause_preparable(
    const type_t<
        clause_base<insert_multi_values_t<Assignments...>, Statement>>&) {
  return check_clause_preparable<Db>(
      type_t<clause_base<insert_values_t<Assignments...>, Statement>>{});
}

// this function assumes that there is something to do
// the _check if there is at least one row has to be performed elsewhere
export template <typename Context, typename Statement, typename... Assignments>
[[nodiscard]] auto to_sql_string(
    Context& context,
    const clause_base<insert_multi_values_t<Assignments...>, Statement>& t) {
  auto ret = std::string{};

  // columns
  {
    ret += " (";
    ret += tuple_to_sql_string(
        context, ", ",
        std::tuple(
            free_column_t<column_of_t<remove_optional_t<Assignments>>>{}...));
    ret += ")";
  }

  // values
  {
    ret += " VALUES ";
    auto first = true;
    for (const auto& row : t._rows) {
      if (!first) ret += ", ";
      first = false;
      ret += "(";
      ret += tuple_to_sql_string(context, ", ",
                                 std::tuple(insert_assignment_t<Assignments>{
                                     std::get<Assignments>(row)}...));
      ret += ")";
    }
  }

  return ret;
}

#warning: Assignments need to prevent read-only
#warning: check table of assignments before executing query
struct no_insert_values_t {};

export template <typename Statement>
class clause_base<no_insert_values_t, Statement> {
 public:
  template <typename OtherStatement>
  constexpr clause_base(
      const clause_base<no_insert_values_t, OtherStatement>& s) {}

  constexpr clause_base() = default;

#warning need to check if all columns have default value
  [[nodiscard]] constexpr auto default_values() const {
    return new_statement(*this, insert_default_values_t{});
  }

  template <typename... Assignments>
  requires(sqlpp::concepts::valid_insert_assignments<Assignments>)
  [[nodiscard]] constexpr auto set(Assignments... as) const {
    using row_t = std::tuple<Assignments...>;
    return new_statement(*this, insert_values_t<Assignments...>{row_t{as...}});
  }

  template <typename... Assignments>
  requires(sqlpp::concepts::valid_insert_assignments<Assignments>)
  [[nodiscard]] constexpr auto multiset(
      std::vector<std::tuple<Assignments...>> assignments) const {
    return new_statement(*this, insert_multi_values_t<Assignments...>{assignments});
  }
};

export template <typename Context, typename Statement>
[[nodiscard]] auto to_sql_string(Context& context,
                                 clause_base<no_insert_values_t, Statement>&) {
  return std::string{};
}
}  // namespace sqlpp
