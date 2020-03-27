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
#include <sqlpp20/column_spec.h>
#include <sqlpp20/result.h>
#include <sqlpp20/result_row.h>
#include <sqlpp20/statement.h>
#include <sqlpp20/to_sql_name.h>
#include <sqlpp20/tuple_to_sql_string.h>
#include <sqlpp20/type_traits.h>

#include <tuple>

namespace sqlpp {
template <typename... Columns>
struct select_columns_t {
  std::tuple<Columns...> _columns;
};

template <typename... Columns>
struct nodes_of<select_columns_t<Columns...>> {
  using type = type_vector<Columns...>;
};

template <typename... Columns>
constexpr auto clause_tag<select_columns_t<Columns...>> =
    ::std::string_view{"select_columns"};

template <typename Column>
struct select_column_t {
  constexpr select_column_t(Column column) : _column(column) {}

  constexpr select_column_t(const select_column_t&) = default;
  constexpr select_column_t(select_column_t&&) = default;
  select_column_t& operator=(const select_column_t&) = default;
  select_column_t& operator=(select_column_t&&) = default;
  ~select_column_t() = default;

  Column _column;
};

template <typename Context, typename Column>
[[nodiscard]] auto to_sql_string(Context& context,
                                 const select_column_t<Column>& t) {
  return has_value(t._column)
             ? to_sql_string(context, get_value(t._column))
             : std::string("NULL AS ") +
                   to_sql_name(context,
                               name_tag_of_t<remove_optional_t<Column>>{});
}

export template <typename... Columns, typename Statement>
class clause_base<select_columns_t<Columns...>, Statement> {
 public:
  template <typename OtherStatement>
  clause_base(
      const clause_base<select_columns_t<Columns...>, OtherStatement>& s)
      : _columns(s._columns) {}

  clause_base(const select_columns_t<Columns...>& f) : _columns(f._columns) {}

  std::tuple<select_column_t<Columns>...> _columns;
};

template <typename Db, typename... Columns, typename... Clauses>
constexpr auto check_clause_preparable(
    const type_t<
        clause_base<select_columns_t<Columns...>, statement<Clauses...>>>& t) {
  constexpr auto known_aggregates =
      (::sqlpp::make_type_vector() + ... + provided_aggregates_of_v<Clauses>);

  constexpr auto all_aggregates =
      (true and ... and
       recursive_is_aggregate(known_aggregates,
                              ::sqlpp::type_vector<Columns>{}));
  constexpr auto no_aggregates =
      not(false or ... or
          recursive_contains_aggregate(known_aggregates,
                                       ::sqlpp::type_vector<Columns>{}));

  if constexpr (not(all_aggregates or no_aggregates)) {
      static_assert(sizeof(t) == 0, "selected columns need to be either all aggregates or all non-aggregates");
    return false;
  } else {
    return true;
  }
}

template <typename... Columns>
constexpr auto is_result_clause_v<select_columns_t<Columns...>> = true;

template <typename... Columns>
struct clause_result_type<select_columns_t<Columns...>> {
  using type = select_result;
};

template <typename... Columns, typename Statement>
struct result_row_of<clause_base<select_columns_t<Columns...>, Statement>> {
  using type = result_row_t<make_column_spec_t<Statement, Columns>...>;
};

template <typename Context, typename... Columns, typename Statement>
[[nodiscard]] auto to_sql_string(
    Context& context,
    const clause_base<select_columns_t<Columns...>, Statement>& t) {
  return " " + tuple_to_sql_string(context, ", ", t._columns);
}

struct no_select_columns_t {};

export template <typename Statement>
class clause_base<no_select_columns_t, Statement> {
 public:
  template <typename OtherStatement>
  constexpr clause_base(
      const clause_base<no_select_columns_t, OtherStatement>& s) {}

  constexpr clause_base() = default;

  template <Selectable... Columns>
  requires(sizeof...(Columns) > 0)
  [[nodiscard]] constexpr auto columns(Columns... columns) const {
      return new_statement(
          *this, select_columns_t<Columns...>{std::tuple(columns...)});
  }

  template <Selectable... Columns>
  requires(sizeof...(Columns) > 0)
  [[nodiscard]] constexpr auto columns(std::tuple<Columns...> columns) const {
      return new_statement(*this, select_columns_t<Columns...>{columns});
  }
};

template <typename Context, typename Statement>
[[nodiscard]] auto to_sql_string(
    Context& context, const clause_base<no_select_columns_t, Statement>& t) {
  return std::string{};
}

export template <typename... Columns>
requires(sizeof...(Columns) > 0)
[[nodiscard]] constexpr auto select_columns(Columns... columns) {
  return statement<no_select_columns_t>{}.columns(columns...);
}
}  // namespace sqlpp
