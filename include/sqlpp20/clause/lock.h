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

namespace sqlpp {
struct for_update_t {};

struct for_share_t {};

template <>
constexpr auto clause_tag<for_update_t> = ::std::string_view{"lock"};

template <>
constexpr auto clause_tag<for_share_t> = ::std::string_view{"lock"};

template <typename Statement>
class clause_base<for_update_t, Statement> {
 public:
  template <typename OtherStatement>
  constexpr clause_base(const clause_base<for_update_t, OtherStatement>& s) {}

  constexpr clause_base(const for_update_t& f) {}
};

template <typename Statement>
class clause_base<for_share_t, Statement> {
 public:
  template <typename OtherStatement>
  constexpr clause_base(const clause_base<for_share_t, OtherStatement>& s) {}

  constexpr clause_base(const for_share_t& f) {}
};

template <typename Context, typename Statement>
[[nodiscard]] auto to_sql_string(
    Context& context, const clause_base<for_update_t, Statement>& t) {
  return std::string(" FOR UPDATE");
}

template <typename Context, typename Statement>
[[nodiscard]] auto to_sql_string(Context& context,
                                 const clause_base<for_share_t, Statement>& t) {
  return std::string(" FOR SHARE");
}

struct no_lock_t {};

template <typename Statement>
class clause_base<no_lock_t, Statement> {
 public:
  template <typename OtherStatement>
  constexpr clause_base(const clause_base<no_lock_t, OtherStatement>& s) {}

  constexpr clause_base() = default;

  [[nodiscard]] constexpr auto for_update() const {
    return new_statement(*this, for_update_t{});
  }

  [[nodiscard]] constexpr auto for_share() const {
    return new_statement(*this, for_share_t{});
  }
};

template <typename Context, typename Statement>
[[nodiscard]] auto to_sql_string(Context& context,
                                 const clause_base<no_lock_t, Statement>&) {
  return std::string{};
}

[[nodiscard]] constexpr auto for_update() {
  return statement<no_lock_t>{}.for_update();
}

[[nodiscard]] constexpr auto for_share() {
  return statement<no_lock_t>{}.for_share();
}
}  // namespace sqlpp
