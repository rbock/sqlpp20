#pragma once

/*
Copyright (c) 2020 - 2020, Roland Bock
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

#include <type_traits>
#include <utility>

namespace sqlpp {
namespace detail {
template <typename>
struct type {};
template <typename T, auto>
struct indexed_type : type<T> {};

template <typename... Ts>
struct mixed_type : Ts... {};
template <typename... Ts, auto... Is>
constexpr auto make_mixed_type(std::index_sequence<Is...>)
    -> mixed_type<indexed_type<Ts, Is>...>;

template <typename... Ts>
constexpr auto use_types(type<Ts>...) -> std::true_type;

// MixedType inherits indirectly from type<T> for each T.
// When calling use_types, MixedType has to be converted to type<T> for each
// type. If there is a duplicate, this conversion is ambiguous and the other
// overload is chosen.
template <typename... Ts, typename MixedType = decltype(make_mixed_type<Ts...>(
                              std::make_index_sequence<sizeof...(Ts)>()))>
constexpr auto has_no_duplicates(int)
    -> decltype(use_types<Ts...>((type<Ts>{}, MixedType{})...));

template <typename... Ts>
constexpr auto has_no_duplicates(...) -> std::false_type;
}  // namespace detail

template <typename... Ts>
constexpr auto unique_types_v =
    decltype(detail::has_no_duplicates<Ts...>(0))::value;

static_assert(unique_types_v<int, float, bool>);
static_assert(!unique_types_v<int, double, int>);
}  // namespace sqlpp

