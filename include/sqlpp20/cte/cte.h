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

#include <sqlpp20/clause/union.h>
#include <sqlpp20/column.h>
#include <sqlpp20/result_row.h>
#include <sqlpp20/table_spec.h>
#include <sqlpp20/type_traits.h>

namespace sqlpp {
template <typename TableSpec, typename ResultRow>
struct cte_columns_t {
  static_assert(wrong<cte_columns_t>, "Invalid arguments for cte_columns_t");
};

template <typename TableSpec, typename... ResultColumnSpecs>
struct cte_columns_t<TableSpec, result_row_t<ResultColumnSpecs...>>
    : member_t<ResultColumnSpecs, column_t<TableSpec, ResultColumnSpecs>>... {};

struct flat_t;
struct recursive_t;

export template <typename CteType, typename TableSpec, typename Stat>
struct cte_t;

namespace detail {
template <typename TableSpec, typename... ResultColumnSpecs>
[[nodiscard]] constexpr auto all_of(
    const cte_columns_t<TableSpec, ResultColumnSpecs...>& t) {
  return std::tuple{column_t<TableSpec, ResultColumnSpecs>{}...};
}
}  // namespace detail

export template <typename CteType, typename TableSpec, typename Stat>
struct cte_t : cte_columns_t<TableSpec, result_row_of_t<Stat>> {
  Stat _statement;

  cte_t(Stat statement) : _statement(statement) {}

  template <SelectStatement SecondStat>
  requires(not is_cte_recursive_v<cte_t> and are_union_compatible_v<cte_t, SecondStat>)
  constexpr auto union_all(SecondStat second_statement) {
    auto _union = ::sqlpp::union_all(_statement, second_statement);
    return cte_t<recursive_t, TableSpec, decltype(_union)>(_union);
  }

  template <SelectStatement SecondStat>
  requires(not is_cte_recursive_v<cte_t> and are_union_compatible_v<cte_t, SecondStat>)
  constexpr auto union_distinct(SecondStat second_statement) {
    auto _union = ::sqlpp::union_distinct(_statement, second_statement);
    return cte_t<recursive_t, TableSpec, decltype(_union)>(_union);
  }
};

template <typename CteType, typename TableSpec, typename Stat>
struct is_cte_recursive<cte_t<CteType, TableSpec, Stat>>
    : std::integral_constant<bool, std::is_same_v<CteType, recursive_t>> {};

template <typename CteType, typename TableSpec, typename Stat>
[[nodiscard]] constexpr auto all_of(
    const cte_t<CteType, TableSpec, Stat>& t) {
  using _base = cte_columns_t<TableSpec, result_row_of_t<Stat>>;
  return detail::all_of(static_cast<const _base&>(t));
}

template <typename CteType, typename TableSpec, typename Stat>
[[nodiscard]] constexpr auto provided_tables_of([
    [maybe_unused]] type_t<cte_t<CteType, TableSpec, Stat>>) {
  return type_vector<TableSpec>();
}

template <typename CteType, typename TableSpec, typename Stat>
[[nodiscard]] constexpr auto required_tables_of([
    [maybe_unused]] type_t<cte_t<CteType, TableSpec, Stat>>) {
  return type_set<>();
}

template <typename CteType, typename TableSpec, typename Stat>
[[nodiscard]] constexpr auto required_ctes_of(
    type_t<cte_t<CteType, TableSpec, Stat>> c) {
  return provided_tables_of(c);
};

template <typename CteType, typename TableSpec, typename Stat>
struct nodes_of<cte_t<CteType, TableSpec, Stat>> {
  using type = type_vector<Stat>;
};

template <typename CteType, typename TableSpec, typename Stat>
struct result_row_of<cte_t<CteType, TableSpec, Stat>> {
  using type = result_row_of_t<Stat>;
};

template <typename CteType, typename TableSpec, typename Stat>
constexpr auto is_table_v<cte_t<CteType, TableSpec, Stat>> = true;

template <typename CteType, typename TableSpec, typename Stat>
struct is_cte<cte_t<CteType, TableSpec, Stat>> : std::true_type {};

template <typename CteType, typename TableSpec, typename Stat>
struct name_tag_of<cte_t<CteType, TableSpec, Stat>> {
  using type = name_tag_of_t<TableSpec>;
};

export template <typename Context, typename CteType, typename TableSpec,
          typename Stat>
[[nodiscard]] auto to_full_sql_string(
    Context& context, const cte_t<CteType, TableSpec, Stat>& t) {
  return to_sql_name(context, t) + " AS (" +
         to_sql_string(context, t._statement) + ")";
}

export template <typename Context, typename CteType, typename TableSpec,
          typename Stat>
[[nodiscard]] auto to_sql_string(
    Context& context, const cte_t<CteType, TableSpec, Stat>& t) {
  return to_sql_name(context, t);
}
}  // namespace sqlpp
