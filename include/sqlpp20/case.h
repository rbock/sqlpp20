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

#include <sqlpp20/bad_expression.h>
#include <sqlpp20/embrace.h>
#include <sqlpp20/tuple_to_sql_string.h>
#include <sqlpp20/type_traits.h>
#include <sqlpp20/wrapped_static_assert.h>

namespace sqlpp {
template <typename CaseWhenThen, typename Else>
struct case_when_then_else_t {
  CaseWhenThen _case_when_then;  // one or more when/then combinations
  Else _else;                    // The final else
};

template <typename CaseWhenThen, typename Else>
struct nodes_of<case_when_then_else_t<CaseWhenThen, Else>> {
  using type = type_vector<CaseWhenThen, Else>;
};

template <typename Expr>
struct then_t {
  Expr _expr;
};

template <typename When, typename Then>
struct when_then_t {
  When _when;
  Then _then;
};

template <typename When, typename Then>
struct nodes_of<when_then_t<When, Then>> {
  using type = type_vector<When, Then>;
};

template <typename... WhenThens>
struct case_when_then_t {
  std::tuple<WhenThens...> _when_thens;

  template <typename When, typename Then>
  requires(has_boolean_value_v<When> and std::is_same_v<value_type_of_t<case_when_then_t>,
                                          value_type_of_t<Then>>)
  [[nodiscard]] constexpr auto when(When when, then_t<Then> then) const {
    auto _wt = when_then_t<When, Then>{when, then._expr};
    return case_when_then_t<WhenThens..., decltype(_wt)>{
        std::tuple_cat(_when_thens, std::tuple{_wt})};
  }

  template <typename Else>
  requires(std::is_same_v<value_type_of_t<case_when_then_t>, value_type_of_t<Else>>)
  [[nodiscard]] constexpr auto else_(Else els) const {
    return case_when_then_else_t<case_when_then_t, Else>{*this, els};
  }
};

template <typename... WhenThens>
struct nodes_of<case_when_then_t<WhenThens...>> {
  using type = type_vector<WhenThens...>;
};

template <typename... WhenThens>
struct value_type_of<case_when_then_t<WhenThens...>> {
  using type =
      value_type_of_t<std::tuple_element_t<0, std::tuple<WhenThens...>>>;
};

template <typename When, typename Then>
struct value_type_of<when_then_t<When, Then>> {
  using type = value_type_of_t<Then>;
};

template <typename Context, typename When, typename Then>
[[nodiscard]] auto to_sql_string(Context& context,
                                 const when_then_t<When, Then>& t) {
  return std::string{" WHEN "} + to_sql_string(context, embrace(t._when)) +
         " THEN " + to_sql_string(context, embrace(t._then));
}

template <typename Context, typename... WhenThens>
[[nodiscard]] auto to_sql_string(Context& context,
                                 const case_when_then_t<WhenThens...>& t) {
  return std::string{" CASE"} + tuple_to_sql_string(context, "", t._when_thens);
}

template <typename Context, typename CaseWhenThen, typename Else>
[[nodiscard]] auto to_sql_string(
    Context& context, const case_when_then_else_t<CaseWhenThen, Else>& t) {
  return to_sql_string(context, t._case_when_then) + " ELSE " +
         to_sql_string(context, embrace(t._else));
}

template <Expression Then>
[[nodiscard]] constexpr auto then(Then expr) {
    return then_t<Then>{expr};
}

template <typename When, Expression Then>
requires(has_boolean_value_v<When>)
[[nodiscard]] constexpr auto case_when(When when, then_t<Then> then) {
    auto wt = when_then_t<When, Then>{when, then._expr};
    return case_when_then_t<decltype(wt)>{std::tuple{wt}};
}

}  // namespace sqlpp
