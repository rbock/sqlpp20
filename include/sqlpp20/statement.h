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
#include <sqlpp20/algorithm.h>
#include <sqlpp20/array_unique.h>
#include <sqlpp20/clause_fwd.h>
#include <sqlpp20/detail/statement_constructor_arg.h>
#include <sqlpp20/type_traits.h>

namespace sqlpp {
template <typename T>
struct result_wrapper {
  using type = T;

  template <typename Rhs>
  constexpr auto operator<<(const result_wrapper<Rhs>&) {
    if constexpr (is_result_clause_v<Rhs>) {
      return result_wrapper<Rhs>{};
    } else {
      return result_wrapper<T>{};
    }
  }
};

template <typename... Clauses>
struct result_clause_of<statement<Clauses...>> {
  using type = typename decltype(
      (result_wrapper<no_result>{} << ... << result_wrapper<Clauses>{}))::type;
};

template <typename... Clauses>
struct result_type_of<statement<Clauses...>> {
  using type = clause_result_type_t<result_clause_of_t<statement<Clauses...>>>;
};

template <typename T>
using result_type_of_t = typename result_type_of<T>::type;

template <typename Db, typename Clause, typename Statement>
constexpr auto check_clause_preparable(
    const type_t<clause_base<Clause, Statement>>&) -> bool {
  return true;
}

namespace detail {
template <typename... Ts>
[[nodiscard]] constexpr auto have_unique_names(type_vector<Ts...>) {
  return ::sqlpp::array_unique(::std::array<::std::string_view, sizeof...(Ts)>{
      name_tag_of_t<Ts>::name...});
}

template <typename... Ts>
[[nodiscard]] constexpr auto have_unique_clauses(type_vector<Ts...>) {
  return ::sqlpp::array_unique_or_special_value(
      ::std::array<::std::string_view, sizeof...(Ts)>{clause_tag<Ts>...},
      ::sqlpp::no_clause_tag);
}
}  // namespace detail

export template <typename Db, typename... Clauses>
constexpr auto check_statement_preparable(
    [[maybe_unused]] type_t<statement<Clauses...>> s) -> bool {
  using _statement_t = statement<Clauses...>;

  if constexpr (not detail::have_unique_names(parameters_of_v<_statement_t>)) {
    static_assert(sizeof(s) == 0, "statement uses tables that are not provided");
    return false;
  } else if constexpr (is_a_required_table_missing(
                           provided_tables_of_v<_statement_t>, s)) {
    static_assert(sizeof(s) == 0, "statement uses tables that are not provided");
    return false;
  } else {
    return (true and ... and
            check_clause_preparable<Db>(
                type_v<clause_base<Clauses, _statement_t>>));
  }
}

export template <typename Db, typename... Clauses>
constexpr auto check_statement_executable(
    const type_t<statement<Clauses...>>& s) -> bool {
  if constexpr (parameters_of_v<statement<Clauses...>>.size() != 0) {
    static_assert(sizeof(s) == 0,
                  "directly executed statements must have no parameters");
    return false;
  } else {
    return check_statement_preparable<Db>(s);
  }
}

export template <typename... Clauses>
class statement : public clause_base<Clauses, statement<Clauses...>>... {
 public:
  constexpr statement() {}

  template <typename Arg>
  constexpr statement(Arg arg) : clause_base<Clauses, statement>(arg)... {}
};

template <typename Clause, typename... Clauses>
auto clause_of(const statement<Clauses...>& s) {
  return static_cast<const clause_base<Clause, statement<Clauses...>>&>(s);
}

template <typename Clause, typename... Clauses>
auto statement_of(const clause_base<Clause, statement<Clauses...>>& base) {
  return static_cast<const statement<Clauses...>&>(base);
}

template <typename OldClause, typename... Clauses, typename NewClause>
auto new_statement(const clause_base<OldClause, statement<Clauses...>>& oldBase,
                   NewClause newClause) {
  const auto& old_statement = statement_of(oldBase);
  return statement<std::conditional_t<std::is_same_v<Clauses, OldClause>,
                                      NewClause, Clauses>...>{
      detail::statement_constructor_arg(old_statement, newClause)};
}

template <typename... Clauses>
struct nodes_of<statement<Clauses...>> {
  using type = type_vector<Clauses...>;
};

template <typename... Clauses>
struct is_statement<statement<Clauses...>> : public std::true_type {};

template <typename... Clauses>
constexpr auto requires_braces_v<statement<Clauses...>> = true;

template <typename... Clauses>
struct result_row_of<statement<Clauses...>> {
  using type =
      result_row_of_t<clause_base<result_clause_of_t<statement<Clauses...>>,
                                  statement<Clauses...>>>;
};

export template <typename... Clauses>
constexpr auto unique_statement_clauses() {
  if constexpr (not detail::have_unique_clauses(
                    ::sqlpp::type_vector<Clauses...>{})) {
    return false;
  } else
    return true;
}

export template <typename Context, typename... Clauses>
[[nodiscard]] auto to_sql_string(Context& context,
                                 const statement<Clauses...>& t) {
  return (
      std::string{} + ... +
      to_sql_string(
          context,
          static_cast<const clause_base<Clauses, statement<Clauses...>>&>(t)));
}

#warning: RHS should be a clause, not a statement
export template <typename... LClauses, typename... RClauses>
requires(::sqlpp::concepts::unique_clauses<LClauses..., RClauses...>)
constexpr auto operator<<(statement<LClauses...> l, statement<RClauses...> r) {
    // remove non-clauses from left part
    using clauses_t =
        decltype((type_vector<>{} + ... +
                  std::conditional_t<is_clause_v<LClauses>,
                                     type_vector<LClauses>, type_vector<>>{}) +
                 type_vector<RClauses...>{});
    return algorithm::copy_t<clauses_t, statement>(
        detail::statement_constructor_arg(l, r));
}
}  // namespace sqlpp
