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

#include <sqlpp20/comparison.h>

namespace sqlpp {
struct equal_to_t {
  static constexpr auto symbol = " = ";
};

export template <typename L, typename R>
requires(has_text_value_v<L>and has_text_value_v<R>) constexpr auto operator==(
    L l, R r) {
  return comparison_t<L, equal_to_t, R>{l, r};
}

export template <typename L, typename R>
requires(has_numeric_value_v<L>and has_numeric_value_v<R>) constexpr auto
operator==(L l, R r) {
  return comparison_t<L, equal_to_t, R>{l, r};
}

export template <typename L, typename R>
requires(has_boolean_value_v<L>and has_boolean_value_v<R>) constexpr auto
operator==(L l, R r) {
  return comparison_t<L, equal_to_t, R>{l, r};
}

}  // namespace sqlpp
