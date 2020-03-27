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
#include <sqlpp20/cte.h>
#include <sqlpp20/statement.h>
#include <sqlpp20/tuple_to_sql_string.h>
#include <sqlpp20/type_traits.h>

namespace sqlpp {
enum class with_mode { flat, recursive };

template <with_mode Mode, typename... CommonTableExpressions>
struct with_t {
  std::tuple<CommonTableExpressions...> _ctes;
};

template <with_mode Mode, typename... CommonTableExpressions>
struct nodes_of<with_t<Mode, CommonTableExpressions...>> {
  using type = type_vector<CommonTableExpressions...>;
};

template <with_mode Mode, typename... CommonTableExpressions>
[[nodiscard]] constexpr auto provided_ctes_of([
    [maybe_unused]] type_t<with_t<Mode, CommonTableExpressions...>>) {
  return type_vector<table_spec_of_t<CommonTableExpressions>...>{};
};

template <with_mode Mode, typename... CommonTableExpressions>
constexpr auto clause_tag<with_t<Mode, CommonTableExpressions...>> =
    ::std::string_view{"with"};

export template <with_mode Mode, typename... CommonTableExpressions,
          typename Statement>
class clause_base<with_t<Mode, CommonTableExpressions...>, Statement> {
 public:
  template <typename OtherStatement>
  clause_base(const clause_base<with_t<Mode, CommonTableExpressions...>,
                                OtherStatement>& s)
      : _ctes(s._ctes) {}

  clause_base(const with_t<Mode, CommonTableExpressions...>& f)
      : _ctes(f._ctes) {}

  std::tuple<CommonTableExpressions...> _ctes;
};

export template <typename Context>
[[nodiscard]] auto to_sql_string(Context& context, with_mode mode) {
  switch (mode) {
    case with_mode::flat:
      return std::string("");
    case with_mode::recursive:
      return std::string("RECURSIVE ");
  }
}

export template <typename Context, with_mode Mode, typename... CommonTableExpressions,
          typename Statement>
[[nodiscard]] auto to_sql_string(
    Context& context,
    const clause_base<with_t<Mode, CommonTableExpressions...>, Statement>& t) {
  int index = -1;
  return std::string{"WITH "} + to_sql_string(context, Mode) +
         (std::string() + ... +
          ((++index ? ", " : "") +
           to_full_sql_string(context,
                              std::get<CommonTableExpressions>(t._ctes)))) +
         " ";
}

struct no_with_t {};

export template <typename Statement>
class clause_base<no_with_t, Statement> {
 public:
  template <typename OtherStatement>
  constexpr clause_base(const clause_base<no_with_t, OtherStatement>& s) {}

  constexpr clause_base() = default;

  template <FlatCommonTableExpression... CommonTableExpressions>
  [[nodiscard]] constexpr auto with(CommonTableExpressions... ctes) const {
      return new_statement(
          *this, with_t<with_mode::flat, CommonTableExpressions...>{ctes...});
  }

  template <CommonTableExpression... CommonTableExpressions>
  [[nodiscard]] constexpr auto with_recursive(
      CommonTableExpressions... ctes) const {
      return new_statement(
          *this,
          with_t<with_mode::recursive, CommonTableExpressions...>{ctes...});
  }
};

export template <typename Context, typename Statement>
[[nodiscard]] auto to_sql_string(Context& context,
                                 const clause_base<no_with_t, Statement>&) {
  return std::string{};
}

export template <typename... CommonTableExpressions>
[[nodiscard]] constexpr auto with(CommonTableExpressions&&... ctes) {
  return statement<no_with_t>{}.with(
      std::forward<CommonTableExpressions...>(ctes...));
}

export template <typename... CommonTableExpressions>
[[nodiscard]] constexpr auto with_recursive(CommonTableExpressions&&... ctes) {
  return statement<no_with_t>{}.with_recursive(
      std::forward<CommonTableExpressions...>(ctes...));
}
}  // namespace sqlpp
