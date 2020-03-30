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

#include <sqlpp20/to_sql_string.h>
#include <sqlpp20/tuple_to_sql_string.h>
#include <sqlpp20/type_traits.h>

namespace sqlpp {
export template <typename... Args>
struct coalesce_t {
  std::tuple<Args...> args;
};

template <typename... Args>
struct nodes_of<coalesce_t<Args...>> {
  using type = type_vector<Args...>;
};

export template <typename... Args>
requires(::sqlpp::concepts::valid_coalesce_arguments<Args...>)
    [[nodiscard]] constexpr auto coalesce(Args... args) {
  return coalesce_t<Args...>{std::tuple{args...}};
}

template <typename Arg0, typename... Args>
struct value_type_of<coalesce_t<Arg0, Args...>> {
  using type = value_type_of_t<Arg0>;
};

export template <typename Context, typename... Args>
[[nodiscard]] auto to_sql_string(Context& context,
                                 const coalesce_t<Args...>& t) {
  return "COALESCE(" + tuple_to_sql_string(context, ", ", t.args) + ")";
}

}  // namespace sqlpp
