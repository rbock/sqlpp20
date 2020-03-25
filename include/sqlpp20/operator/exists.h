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

#include <sqlpp20/to_sql_string.h>
#include <sqlpp20/type_traits.h>

namespace sqlpp {
template <typename SubQuery>
struct exists_t {
  SubQuery sub_query;
};

template <typename SubQuery>
struct nodes_of<exists_t<SubQuery>> {
  using type = type_vector<SubQuery>;
};

#warning: Use concept here
export template <typename SubQuery>
requires(is_statement_v<SubQuery>and has_result_row_v<
         SubQuery>) constexpr auto exists(SubQuery sub_query)
    -> exists_t<SubQuery> {
  return exists_t<SubQuery>{sub_query};
}

template <typename SubQuery>
struct value_type_of<exists_t<SubQuery>> {
  using type = bool;
};

export template <typename Context, typename SubQuery>
[[nodiscard]] auto to_sql_string(Context& context,
                                 const exists_t<SubQuery>& t) {
  return " EXISTS(" + to_sql_string(context, t.sub_query) + ") ";
}
}  // namespace sqlpp
