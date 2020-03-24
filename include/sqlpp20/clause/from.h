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
#include <sqlpp20/type_traits.h>
#include <sqlpp20/wrapped_static_assert.h>

#include <vector>

namespace sqlpp {
template <typename Tab>
struct from_t {
  Tab _table;
};

template <typename Tab>
struct nodes_of<from_t<Tab>> {
  using type = type_vector<Tab>;
};

template <typename Tab>
constexpr auto clause_tag<from_t<Tab>> = ::std::string_view{"from"};

export template <typename Tab, typename Statement>
class clause_base<from_t<Tab>, Statement> {
 public:
  template <typename OtherStatement>
  clause_base(const clause_base<from_t<Tab>, OtherStatement>& s)
      : _table(s._table) {}

  clause_base(const from_t<Tab>& f) : _table(f._table) {}

  Tab _table;
};

export template <typename Context, typename Tab, typename Statement>
[[nodiscard]] auto to_sql_string(
    Context& context, const clause_base<from_t<Tab>, Statement>& t) {
  return std::string(" FROM ") + to_sql_string(context, t._table);
}

struct no_from_t {};

export template <typename Statement>
class clause_base<no_from_t, Statement> {
 public:
  template <typename OtherStatement>
  constexpr clause_base(const clause_base<no_from_t, OtherStatement>& s) {}

  constexpr clause_base() = default;

  template <Table Tab>
  [[nodiscard]] constexpr auto from(Tab t) const {
      return new_statement(*this, from_t<Tab>{t});
  }
};

export template <typename Context, typename Statement>
[[nodiscard]] auto to_sql_string(Context& context,
                                 const clause_base<no_from_t, Statement>&) {
  return std::string{};
}

export template <typename Tab>
[[nodiscard]] constexpr auto from(Tab t) {
  return statement<no_from_t>{}.from(t);
}
}  // namespace sqlpp
