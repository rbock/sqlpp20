#pragma once

/*
Copyright (c) 2018 - 2020, Roland Bock
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
#include <sqlpp20/type_traits.h>
#include <sqlpp20/wrapped_static_assert.h>
#include <sqlpp20/wrong.h>

namespace sqlpp {
template <typename Tab>
struct truncate_t {};

template <typename Tab>
struct nodes_of<truncate_t<Tab>> {
  using type = type_vector<Tab>;
};

template <typename Tab>
constexpr auto clause_tag<truncate_t<Tab>> = ::std::string_view{"truncate"};

export template <typename Tab, typename Statement>
class clause_base<truncate_t<Tab>, Statement> {
 public:
  template <typename OtherStatement>
  clause_base(const clause_base<truncate_t<Tab>, OtherStatement>& t)
      : _table(t.table) {}

  clause_base(Tab table) : _table(table) {}

  Tab _table;
};

template <typename Tab>
constexpr auto is_result_clause_v<truncate_t<Tab>> = true;

template <typename Tab>
struct clause_result_type<truncate_t<Tab>> {
  using type = execute_result;
};

export template <typename Context, typename Tab, typename Statement>
[[nodiscard]] auto to_sql_string(
    Context& context, const clause_base<truncate_t<Tab>, Statement>& t) {
  return "TRUNCATE " + to_sql_name(context, name_tag_of_t<Tab>{});
}

export template <Table Tab>
requires(not is_read_only_v<Tab>)
[[nodiscard]] constexpr auto truncate(Tab table) {
    return statement<truncate_t<Tab>>{table};
}
}  // namespace sqlpp
