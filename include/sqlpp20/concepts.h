#pragma once

/*
Copyright (c) 2017 - 2020, Roland Bock
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

#include <sqlpp20/type_traits.h>
#include <sqlpp20/concepts.h>

namespace sqlpp::concepts {

#warning: Need to fix all UNSPECIFIED concepts!
inline constexpr auto UNSPECIFIED = true;

export template <typename T>
concept table = UNSPECIFIED;

export template <typename T>
concept expression = UNSPECIFIED;

export template <typename T>
concept statement = UNSPECIFIED; // 

export template <typename T>
concept select_statement = UNSPECIFIED; // expression with a name

export template <typename T>
concept select_expression = ::sqlpp::is_selectable_v<remove_optional_t<T>>;

export template <typename T>
concept flat_common_table_expression = UNSPECIFIED; // 

export template <typename T>
concept common_table_expression = UNSPECIFIED; // 

export template <typename T>
concept select_flag = ::sqlpp::is_select_flag_v<remove_optional_t<T>>;

export template <typename T>
concept boolean_expression = ::sqlpp::is_expression_v<T> and ::sqlpp::has_boolean_value_v<T>;

export template <typename T>
concept integral_expression = UNSPECIFIED;

export template <typename T>
concept numeric_expression = UNSPECIFIED;

export template <typename T>
concept table_spec = UNSPECIFIED;

export template <typename T>
concept column_spec = UNSPECIFIED;

export template <typename T>
concept mutable_table = UNSPECIFIED;

export template <typename T>
concept where_condition = UNSPECIFIED; // bool expression, requires(not recursive_contains_aggregate(type_vector<>{}, type_t<Condition>{}))

export template <typename T>
concept group_by_expression = UNSPECIFIED; // requires(not recursive_contains_aggregate(type_vector<>{}, type_t<Condition>{}))

export template <typename T>
concept order_expression = UNSPECIFIED;

export template <typename T>
concept alias = UNSPECIFIED;

export template <typename Left, typename Right>
concept valid_assignment_arguments = UNSPECIFIED; // requires((can_be_null_v<L> or not can_be_null_v<R>)and has_same_valuei_type_v<L, R> and L is mutable column)

export template <typename Left, typename Right>
concept valid_as_arguments = UNSPECIFIED;

export template <typename Left, typename Right>
concept valid_like_arguments = UNSPECIFIED;

export template <typename Left, typename... Right>
concept valid_in_arguments = UNSPECIFIED;

export template <typename Left, typename Right>
concept valid_cte_union_arguments = UNSPECIFIED; // requires(not is_cte_recursive_v<cte_t> and are_union_compatible_v<cte_t, SecondStat>)

export template <typename Left>
concept can_be_null = UNSPECIFIED;

export template<typename T>
concept provides_no_ctes = UNSPECIFIED; //   requires(provided_ctes_of_v<Stat>.empty())

export template<typename... Clauses>
concept unique_clauses = UNSPECIFIED;

export template<typename... Clauses>
concept is_mutable = UNSPECIFIED;

export template<typename... Assignments>
concept valid_insert_assignments = UNSPECIFIED; // (sizeof...(Assignments) > 0 and unique_assignment_columns_v<Assignments...>), is_assignment, all from the same table, no missing assignment, all optional assignments belong to defaulted columns, read_only-columns are assigned to DEFAULT (or not at all)

export template<typename... Assignments>
concept valid_update_assignments = UNSPECIFIED; // (sizeof...(Assignments) > 0 and unique_assignment_columns_v<Assignments...>), is_assignment, all from the same table, default is assigned to defaulted columns, only, read_only-columns are not assigned

export template<typename... Texts>
concept valid_concat_arguments = UNSPECIFIED; // (sizeof...(Args) > 1) and ... and has_text_value_v<Args>) constexpr auto concat(Args... args

export template<typename Expression>
concept numeric_aggregate_argument = UNSPECIFIED; // requires(has_numeric_value_v<Expr> and not is_alias_v<Expr> and not is_aggregate_v<Expr>)

export template < typename Expression>
concept aggregate_argument = UNSPECIFIED; //requires(not is_alias_v<Expr> and not is_aggregate_v<Expr>)

export template < typename left, typename Right>
concept valid_comparison_arguments = UNSPECIFIED; // left and right have comparable value types

export template < typename... Args>
concept valid_coalesce_arguments = UNSPECIFIED; //(sizeof...(Args) != 0 and same value category (e.g. numeric))

export template <typename Lhs, typename Rhs>
concept valid_join_arguments = UNSPECIFIED; // requires(table_names_of_v<Lhs>.is_disjoint_from(table_names_of_v<remove_optional_t<Rhs>>)

export template<typename Join, typename Expr>
concept valid_join_condition = UNSPECIFIED; // requires(BooleanExpression<Condition> and not is_a_required_table_missing(provided_tables_of_v<conditionless_join_t>,type_t<Expr>{}))

export template<typename Case, typename When, typename Then>
concept valid_case_when_then_arguments = UNSPECIFIED; // requires(has_boolean_value_v<When> and std::is_same_v<value_type_of_t<case_when_then_t>, value_type_of_t<Then>>)

export template<typename Case, typename Else>
concept valid_case_else_arguments = UNSPECIFIED; // requires(has_boolean_value_v<When> and std::is_same_v<value_type_of_t<case_when_then_t>, value_type_of_t<Then>>)

export template<typename When, typename Then>
concept valid_when_then_arguments = UNSPECIFIED; // requires(has_boolean_value_v<When> and expression<Then>

export template<typename Expr>
concept cte_expression = UNSPECIFIED; // 

export template<typename Statement>
concept cte_statement = UNSPECIFIED; // select statement that does not provide CTEs

export template<typename Left, typename Right>
concept valid_union_arguments = UNSPECIFIED; // requires(are_union_compatible_v<BaseStatement, RightSelect> and provided_ctes_of_v<RightSelect>.empty())

}  // namespace sqlpp::concepts
