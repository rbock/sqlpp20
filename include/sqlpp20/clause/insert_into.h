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

#include <sqlpp20/clause/insert_values.h>
#include <sqlpp20/clause_fwd.h>
#include <sqlpp20/statement.h>
#include <sqlpp20/type_traits.h>

namespace sqlpp {
template <typename Tab>
struct insert_into_t {
  Tab _table;
};

template <typename Tab>
struct nodes_of<insert_into_t<Tab>> {
  using type = type_vector<Tab>;
};

template <typename Tab>
constexpr auto clause_tag<insert_into_t<Tab>> =
    ::std::string_view{"insert_into"};

export template <typename Tab, typename Statement>
class clause_base<insert_into_t<Tab>, Statement> {
 public:
  using insert_into_table_t = Tab;

  template <typename OtherStatement>
  clause_base(const clause_base<insert_into_t<Tab>, OtherStatement>& s)
      : _table(s._table) {}

  clause_base(Tab table) : _table(table) {}

  Tab _table;
};

export template <typename Context, typename Tab, typename Statement>
[[nodiscard]] auto to_sql_string(
    Context& context, const clause_base<insert_into_t<Tab>, Statement>& t) {
  return std::string("INSERT INTO ") + to_sql_string(context, t._table);
}

template <typename Tab>
constexpr auto is_result_clause_v<insert_into_t<Tab>> = true;

template <typename Tab>
struct clause_result_type<insert_into_t<Tab>> {
  using type = insert_result;
};

export template <PrimaryTable Tab>
[[nodiscard]] constexpr auto insert_into(Tab t) {
    return statement<insert_into_t<Tab>>{t}
           << statement<no_insert_values_t>{};
}
}  // namespace sqlpp
