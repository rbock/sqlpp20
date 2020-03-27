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
#include <sqlpp20/flags.h>
#include <sqlpp20/result_row.h>
#include <sqlpp20/statement.h>
#include <sqlpp20/type_traits.h>

namespace sqlpp {
template <typename Flag, typename LeftSelect, typename RightSelect>
struct union_t {
  Flag _flag;
  LeftSelect _left;
  RightSelect _right;
};

template <typename Flag, typename LeftSelect, typename RightSelect>
struct nodes_of<union_t<Flag, LeftSelect, RightSelect>> {
  using type = type_vector<LeftSelect, RightSelect>;
};

template <typename Flag, typename LeftSelect, typename RightSelect>
constexpr auto is_result_clause_v<union_t<Flag, LeftSelect, RightSelect>> =
    true;

template <typename Flag, typename LeftSelect, typename RightSelect>
struct clause_result_type<union_t<Flag, LeftSelect, RightSelect>> {
  using type = select_result;
};

template <typename Flag, typename LeftSelect, typename RightSelect>
constexpr auto clause_tag<union_t<Flag, LeftSelect, RightSelect>> =
    ::std::string_view{"union"};

template <typename Flag, typename LeftSelect, typename RightSelect,
          typename BaseStatement>
class clause_base<union_t<Flag, LeftSelect, RightSelect>, BaseStatement> {
 public:
  template <typename OtherStatement>
  clause_base(const clause_base<union_t<Flag, LeftSelect, RightSelect>,
                                OtherStatement>& s)
      : _left(s._left), _right(s._right) {}

  clause_base(const union_t<Flag, LeftSelect, RightSelect>& f)
      : _flag(f._flag), _left(f._left), _right(f._right) {}

  Flag _flag;
  LeftSelect _left;
  RightSelect _right;
};

template <typename Flag, typename LeftSelect, typename RightSelect,
          typename BaseStatement>
struct result_row_of<
    clause_base<union_t<Flag, LeftSelect, RightSelect>, BaseStatement>> {
  using type = result_row_of_t<LeftSelect>;
};

template <typename Context, typename Flag, typename LeftSelect,
          typename RightSelect, typename BaseStatement>
[[nodiscard]] auto to_sql_string(
    Context& context,
    const clause_base<union_t<Flag, LeftSelect, RightSelect>, BaseStatement>& t) {
#warning missing flag serialization?
  return to_sql_string(context, t._left) + " UNION " +
         to_sql_string(context, t._right);
}

struct no_union_t {};

template <typename BaseStatement>
class clause_base<no_union_t, BaseStatement> {
 public:
  template <typename OtherStatement>
  constexpr clause_base(const clause_base<no_union_t, OtherStatement>& s) {}

  constexpr clause_base() = default;

  template <SelectStatement RightSelect>
  requires(are_union_compatible_v<BaseStatement, RightSelect> and provided_ctes_of_v<RightSelect>.empty())
  [[nodiscard]] constexpr auto union_all(RightSelect rhs) const {
    return union_all(statement_of(*this), rhs);
  }

  template <SelectStatement RightSelect>
  requires(are_union_compatible_v<BaseStatement, RightSelect> and provided_ctes_of_v<RightSelect>.empty())
  [[nodiscard]] constexpr auto union_distinct(RightSelect rhs) const {
    return union_distinct(statement_of(*this), rhs);
  }
};

template <typename Context, typename BaseStatement>
[[nodiscard]] auto to_sql_string(Context& context,
                                 const clause_base<no_union_t, BaseStatement>&) {
  return std::string{};
}

template <SelectStatement LeftSelect, SelectStatement RightSelect>
requires(are_union_compatible_v<LeftSelect, RightSelect> and provided_ctes_of_v<RightSelect>.empty())
[[nodiscard]] constexpr auto union_all(LeftSelect l, RightSelect r) {
  using u = union_t<all_t, LeftSelect, RightSelect>;
  return statement<u>{detail::statement_constructor_arg(u{all, l, r})};
}

template <SelectStatement LeftSelect, SelectStatement RightSelect>
requires(are_union_compatible_v<LeftSelect, RightSelect> and provided_ctes_of_v<RightSelect>.empty())
[[nodiscard]] constexpr auto union_distinct(LeftSelect l, RightSelect r) {
  using u = union_t<distinct_t, LeftSelect, RightSelect>;
  return statement<u>{detail::statement_constructor_arg(u{distinct, l, r})};
}
}  // namespace sqlpp
