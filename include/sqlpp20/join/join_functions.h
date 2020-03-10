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

#include <sqlpp20/bad_expression.h>
#include <sqlpp20/join/join_types.h>
#include <sqlpp20/type_traits.h>
#include <sqlpp20/wrapped_static_assert.h>

namespace sqlpp {
template <typename Lhs, typename JoinType, typename Rhs>
class conditionless_join_t;

namespace detail {
template <typename JoinType, typename Lhs, typename Rhs>
requires(
        is_table_v<Lhs> 
        and is_table_v<remove_optional_t<Rhs>> 
        and table_names_of_v<Lhs>.is_disjoint_from(table_names_of_v<remove_optional_t<Rhs>>)
#warning : The next two should be checked when turning a select into a table
        and not is_a_required_table_missing(provided_tables_of_v<Lhs>, type_t<Lhs>{}) 
        and not is_a_required_table_missing(provided_tables_of_v<Rhs>, type_t<Rhs>{}))
constexpr auto join_impl(Lhs lhs, Rhs rhs) {
  return ::sqlpp::conditionless_join_t{lhs, JoinType{}, rhs};
}
}  // namespace detail

template <typename Lhs, typename Rhs>
[[nodiscard]] constexpr auto inner_join(Lhs lhs, Rhs rhs) {
  return detail::join_impl<inner_join_t>(lhs, rhs);
}

template <typename Lhs, typename Rhs>
[[nodiscard]] constexpr auto join(Lhs lhs, Rhs rhs) {
  return inner_join(lhs, rhs);
}

template <typename Lhs, typename Rhs>
[[nodiscard]] constexpr auto left_outer_join(Lhs lhs, Rhs rhs) {
  return detail::join_impl<left_outer_join_t>(lhs, rhs);
}

template <typename Lhs, typename Rhs>
[[nodiscard]] constexpr auto right_outer_join(Lhs lhs, Rhs rhs) {
  return detail::join_impl<right_outer_join_t>(lhs, rhs);
}

template <typename Lhs, typename Rhs>
[[nodiscard]] constexpr auto outer_join(Lhs lhs, Rhs rhs) {
  return detail::join_impl<outer_join_t>(lhs, rhs);
}

namespace detail {
template <typename Lhs, typename Rhs>
requires(
        is_table_v<Lhs> 
        and is_table_v<remove_optional_t<Rhs>> 
        and table_names_of_v<Lhs>.is_disjoint_from(table_names_of_v<remove_optional_t<Rhs>>) 
#warning : The next two should be checked when turning a select into a table
        and not is_a_required_table_missing(provided_tables_of_v<Lhs>, type_t<Lhs>{}) 
        and not is_a_required_table_missing(provided_tables_of_v<Rhs>, type_t<Rhs>{}))
constexpr auto cross_join_impl(Lhs lhs, Rhs rhs) {
    return detail::join_impl<cross_join_t>(lhs, rhs).unconditionally();
}
}  // namespace detail

template <typename Lhs, typename Rhs>
[[nodiscard]] constexpr auto cross_join(Lhs lhs, Rhs rhs) {
  return detail::cross_join_impl(lhs, rhs);
}

template <typename Derived>
class join_functions {
  constexpr decltype(auto) ref() const {
    return static_cast<const Derived&>(*this);
  }

 public:
  template <typename T>
  [[nodiscard]] constexpr auto join(T t) const {
    return ::sqlpp::join(ref(), t);
  }

  template <typename T>
  [[nodiscard]] constexpr auto inner_join(T t) const {
    return ::sqlpp::inner_join(ref(), t);
  }

  template <typename T>
  [[nodiscard]] constexpr auto left_outer_join(T t) const {
    return ::sqlpp::left_outer_join(ref(), t);
  }

  template <typename T>
  [[nodiscard]] constexpr auto right_outer_join(T t) const {
    return ::sqlpp::right_outer_join(ref(), t);
  }

  template <typename T>
  [[nodiscard]] constexpr auto outer_join(T t) const {
    return ::sqlpp::outer_join(ref(), t);
  }

  template <typename T>
  [[nodiscard]] constexpr auto cross_join(T t) const {
    return ::sqlpp::cross_join(ref(), t);
  }
};
}  // namespace sqlpp
