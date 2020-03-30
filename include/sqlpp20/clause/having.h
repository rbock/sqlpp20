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
#include <sqlpp20/type_vector.h>
#include <sqlpp20/type_traits.h>

#include <vector>

namespace sqlpp {
template <typename Condition>
struct having_t {
  Condition _condition;
};

template <typename Condition>
struct nodes_of<having_t<Condition>> {
  using type = type_vector<Condition>;
};

template <typename Table>
constexpr auto clause_tag<having_t<Table>> = ::std::string_view{"having"};

export template <typename Condition, typename Statement>
class clause_base<having_t<Condition>, Statement> {
 public:
  template <typename OtherStatement>
  clause_base(const clause_base<having_t<Condition>, OtherStatement>& s)
      : _condition(s._condition) {}

  clause_base(const having_t<Condition>& f) : _condition(f._condition) {}

  Condition _condition;
};

template <typename Db, typename Condition, typename... Clauses>
constexpr auto check_clause_preparable(
    const type_t<clause_base<having_t<Condition>, statement<Clauses...>>>& t)
    -> bool {
  constexpr auto known_aggregates =
      (::sqlpp::make_type_vector() + ... + provided_aggregates_of_v<Clauses>);

  if constexpr (not recursive_is_aggregate(known_aggregates,
                                           type_t<Condition>{})) {
    static_assert(sizeof(t) == 0,
                  "having condition must consist of aggregates");
    return false;
  } else {
    return true;
  }
}

export template <typename Context, typename Condition, typename Statement>
[[nodiscard]] auto to_sql_string(
    Context& context, const clause_base<having_t<Condition>, Statement>& t) {
  return std::string(" HAVING ") + to_sql_string(context, t._condition);
}

struct no_having_t {};

export template <typename Statement>
class clause_base<no_having_t, Statement> {
 public:
  template <typename OtherStatement>
  constexpr clause_base(const clause_base<no_having_t, OtherStatement>& s) {}

  constexpr clause_base() = default;

  template <::sqlpp::concepts::boolean_expression Condition>
  [[nodiscard]] constexpr auto having(Condition condition) const {
    return new_statement(*this, having_t<Condition>{condition});
  }
};

export template <typename Context, typename Statement>
[[nodiscard]] auto to_sql_string(Context& context,
                                 const clause_base<no_having_t, Statement>&) {
  return std::string{};
}

export template <::sqlpp::concepts::boolean_expression Condition>
[[nodiscard]] constexpr auto having(Condition&& condition) {
  return statement<no_having_t>{}.having(std::forward<Condition>(condition));
}
}  // namespace sqlpp
