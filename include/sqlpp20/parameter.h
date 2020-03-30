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

#include <sqlpp20/type_traits.h>

namespace sqlpp {
export template <typename ValueType, typename NameTag>
struct parameter_t {};

template <typename ValueType, typename NameTag>
struct value_type_of<parameter_t<ValueType, NameTag>> {
  using type = ValueType;
};

template <typename ValueType, typename NameTag>
struct name_tag_of<parameter_t<ValueType, NameTag>> {
  using type = NameTag;
};

template <typename ValueType, typename NameTag>
struct parameters_of<parameter_t<ValueType, NameTag>> {
  static constexpr auto value = type_vector<parameter_t<ValueType, NameTag>>{};
};

export template <typename ValueType>
struct unnamed_parameter_t {
  template <::sqlpp::concepts::alias Tag>
  [[nodiscard]] constexpr auto operator()([[maybe_unused]] Tag) const {
    return parameter_t<ValueType, name_tag_of_t<Tag>>{};
  }
};

export template <typename ValueType>
constexpr auto parameter = unnamed_parameter_t<ValueType>{};

export template <typename Context, typename ValueType, typename NameTag>
[[nodiscard]] auto to_sql_string(Context& context,
                                 const parameter_t<ValueType, NameTag>& t) {
  return std::string{"?"};
}

}  // namespace sqlpp
