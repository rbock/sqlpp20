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

#include <sqlpp20/concepts.h>
#include <sqlpp20/join/join_types.h>
#include <sqlpp20/type_traits.h>

namespace sqlpp {
template <typename Lhs, typename JoinType, typename Rhs>
class conditionless_join_t;

#warning : When turning a SELECT into a temporary table, check for missing required tables
template <typename Lhs, typename Rhs>
requires(::sqlpp::concepts::valid_join_arguments<Lhs, Rhs>)
[[nodiscard]] constexpr auto inner_join(Lhs lhs, Rhs rhs) {
  return ::sqlpp::conditionless_join_t<Lhs, inner_join_t, Rhs>{lhs, rhs};
}

template <typename Lhs, typename Rhs>
requires(::sqlpp::concepts::valid_join_arguments<Lhs, Rhs>)
[[nodiscard]] constexpr auto join(Lhs lhs, Rhs rhs) {
  return ::sqlpp::conditionless_join_t<Lhs, inner_join_t, Rhs>{lhs, rhs};
}

template <typename Lhs, typename Rhs>
requires(::sqlpp::concepts::valid_join_arguments<Lhs, Rhs>)
[[nodiscard]] constexpr auto left_outer_join(Lhs lhs, Rhs rhs) {
  return ::sqlpp::conditionless_join_t<Lhs, outer_join_t, Rhs>{lhs, rhs};
}

template <typename Lhs, typename Rhs>
requires(::sqlpp::concepts::valid_join_arguments<Lhs, Rhs>)
[[nodiscard]] constexpr auto right_outer_join(Lhs lhs, Rhs rhs) {
  return ::sqlpp::conditionless_join_t<Lhs, right_outer_join_t, Rhs>{lhs, rhs};
}

template <typename Lhs, typename Rhs>
requires(::sqlpp::concepts::valid_join_arguments<Lhs, Rhs>)
[[nodiscard]] constexpr auto outer_join(Lhs lhs, Rhs rhs) {
  return ::sqlpp::conditionless_join_t<Lhs, left_outer_join_t, Rhs>{lhs, rhs};
}

template <typename Lhs, typename Rhs>
requires(::sqlpp::concepts::valid_join_arguments<Lhs, Rhs>)
[[nodiscard]] constexpr auto cross_join(Lhs lhs, Rhs rhs) {
  return ::sqlpp::conditionless_join_t<Lhs, cross_join_t, Rhs>{lhs, rhs}.unconditionally();
}

#warning: need to move the constraints here to reduce error message length
export template <typename Derived>
class join_functions {
  constexpr decltype(auto) ref() const {
    return static_cast<const Derived&>(*this);
  }

 public:
  template <typename Rhs>
  requires(::sqlpp::concepts::valid_join_arguments<Derived, Rhs>)
  [[nodiscard]] constexpr auto join(Rhs rhs) const {
    return ::sqlpp::conditionless_join_t<Derived, inner_join_t, Rhs>{ref(), rhs};
  }

  template <typename Rhs>
  requires(::sqlpp::concepts::valid_join_arguments<Derived, Rhs>)
  [[nodiscard]] constexpr auto inner_join(Rhs rhs) const {
    return ::sqlpp::conditionless_join_t<Derived, inner_join_t, Rhs>{ref(), rhs};
  }

  template <typename Rhs>
  requires(::sqlpp::concepts::valid_join_arguments<Derived, Rhs>)
  [[nodiscard]] constexpr auto left_outer_join(Rhs rhs) const {
    return ::sqlpp::conditionless_join_t<Derived, left_outer_join_t, Rhs>{ref(), rhs};
  }

  template <typename Rhs>
  requires(::sqlpp::concepts::valid_join_arguments<Derived, Rhs>)
  [[nodiscard]] constexpr auto right_outer_join(Rhs rhs) const {
    return ::sqlpp::conditionless_join_t<Derived, right_outer_join_t, Rhs>{ref(),
                                                                       rhs};
  }

  template <typename Rhs>
  requires(::sqlpp::concepts::valid_join_arguments<Derived, Rhs>)
  [[nodiscard]] constexpr auto outer_join(Rhs rhs) const {
    return ::sqlpp::conditionless_join_t<Derived, outer_join_t, Rhs>{ref(), rhs};
  }

  template <typename Rhs>
  requires(::sqlpp::concepts::valid_join_arguments<Derived, Rhs>)
  [[nodiscard]] constexpr auto cross_join(Rhs rhs) const {
    return ::sqlpp::conditionless_join_t<Derived, cross_join_t, Rhs>{ref(), rhs}
        .unconditionally();
  }
};
}  // namespace sqlpp
