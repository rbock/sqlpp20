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

#include <sqlpp20/clause/from.h>
#include <sqlpp20/clause/group_by.h>
#include <sqlpp20/clause/having.h>
#include <sqlpp20/clause/limit.h>
#include <sqlpp20/clause/lock.h>
#include <sqlpp20/clause/offset.h>
#include <sqlpp20/clause/order_by.h>
#include <sqlpp20/clause/select_columns.h>
#include <sqlpp20/clause/select_flags.h>
#include <sqlpp20/clause/where.h>
#include <sqlpp20/clause_fwd.h>
#include <sqlpp20/type_traits.h>

namespace sqlpp {
struct select_t {};

template <>
constexpr auto clause_tag<select_t> = ::std::string_view{"select"};

export template <typename Statement>
class clause_base<select_t, Statement> {
 public:
  template <typename OtherStatement>
  clause_base(const clause_base<select_t, OtherStatement>&) {}

  clause_base() = default;
};

export template <typename Context, typename Statement>
[[nodiscard]] auto to_sql_string(Context& context,
                                 const clause_base<select_t, Statement>& t) {
  return std::string{"SELECT"};
}

// select with no args or an empty tuple yields a blank select statement

export [[nodiscard]] constexpr auto select() {
  return statement<select_t, no_select_flags_t, no_select_columns_t, no_from_t,
                   no_where_t, no_group_by_t, no_having_t, no_order_by_t,
                   no_limit_t, no_offset_t, no_lock_t>{};
}

// select with at least one argument will either create flags or columns

export template <SelectFlag... Fs>
[[nodiscard]] constexpr auto select(Fs... fs) {
    return ::sqlpp::select().flags(fs...);
}

export template <Selectable... Fs>
[[nodiscard]] constexpr auto select(Fs... fs) {
    return ::sqlpp::select().columns(fs...);
}

export template <Selectable F, Selectable... Fs>
[[nodiscard]] constexpr auto select(std::tuple<F, Fs...> fs) {
  return ::sqlpp::select().columns(fs);
}

}  // namespace sqlpp
