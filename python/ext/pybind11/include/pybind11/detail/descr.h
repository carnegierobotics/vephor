/**
 * Copyright 2023
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Carnegie Robotics, LLC nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CARNEGIE ROBOTICS, LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/

/*
    pybind11/detail/descr.h: Helper type for concatenating type signatures at compile time

    Copyright (c) 2016 Wenzel Jakob <wenzel.jakob@epfl.ch>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#pragma once

#include "common.h"

PYBIND11_NAMESPACE_BEGIN(PYBIND11_NAMESPACE)
PYBIND11_NAMESPACE_BEGIN(detail)

#if !defined(_MSC_VER)
#    define PYBIND11_DESCR_CONSTEXPR static constexpr
#else
#    define PYBIND11_DESCR_CONSTEXPR const
#endif

/* Concatenate type signatures at compile time */
template <size_t N, typename... Ts>
struct descr {
    char text[N + 1]{'\0'};

    constexpr descr() = default;
    // NOLINTNEXTLINE(google-explicit-constructor)
    constexpr descr(char const (&s)[N + 1]) : descr(s, make_index_sequence<N>()) {}

    template <size_t... Is>
    constexpr descr(char const (&s)[N + 1], index_sequence<Is...>) : text{s[Is]..., '\0'} {}

    template <typename... Chars>
    // NOLINTNEXTLINE(google-explicit-constructor)
    constexpr descr(char c, Chars... cs) : text{c, static_cast<char>(cs)..., '\0'} {}

    static constexpr std::array<const std::type_info *, sizeof...(Ts) + 1> types() {
        return {{&typeid(Ts)..., nullptr}};
    }
};

template <size_t N1, size_t N2, typename... Ts1, typename... Ts2, size_t... Is1, size_t... Is2>
constexpr descr<N1 + N2, Ts1..., Ts2...> plus_impl(const descr<N1, Ts1...> &a,
                                                   const descr<N2, Ts2...> &b,
                                                   index_sequence<Is1...>,
                                                   index_sequence<Is2...>) {
    PYBIND11_WORKAROUND_INCORRECT_MSVC_C4100(b);
    return {a.text[Is1]..., b.text[Is2]...};
}

template <size_t N1, size_t N2, typename... Ts1, typename... Ts2>
constexpr descr<N1 + N2, Ts1..., Ts2...> operator+(const descr<N1, Ts1...> &a,
                                                   const descr<N2, Ts2...> &b) {
    return plus_impl(a, b, make_index_sequence<N1>(), make_index_sequence<N2>());
}

template <size_t N>
constexpr descr<N - 1> const_name(char const (&text)[N]) {
    return descr<N - 1>(text);
}
constexpr descr<0> const_name(char const (&)[1]) { return {}; }

template <size_t Rem, size_t... Digits>
struct int_to_str : int_to_str<Rem / 10, Rem % 10, Digits...> {};
template <size_t... Digits>
struct int_to_str<0, Digits...> {
    // WARNING: This only works with C++17 or higher.
    static constexpr auto digits = descr<sizeof...(Digits)>(('0' + Digits)...);
};

// Ternary description (like std::conditional)
template <bool B, size_t N1, size_t N2>
constexpr enable_if_t<B, descr<N1 - 1>> const_name(char const (&text1)[N1], char const (&)[N2]) {
    return const_name(text1);
}
template <bool B, size_t N1, size_t N2>
constexpr enable_if_t<!B, descr<N2 - 1>> const_name(char const (&)[N1], char const (&text2)[N2]) {
    return const_name(text2);
}

template <bool B, typename T1, typename T2>
constexpr enable_if_t<B, T1> const_name(const T1 &d, const T2 &) {
    return d;
}
template <bool B, typename T1, typename T2>
constexpr enable_if_t<!B, T2> const_name(const T1 &, const T2 &d) {
    return d;
}

template <size_t Size>
auto constexpr const_name() -> remove_cv_t<decltype(int_to_str<Size / 10, Size % 10>::digits)> {
    return int_to_str<Size / 10, Size % 10>::digits;
}

template <typename Type>
constexpr descr<1, Type> const_name() {
    return {'%'};
}

// If "_" is defined as a macro, py::detail::_ cannot be provided.
// It is therefore best to use py::detail::const_name universally.
// This block is for backward compatibility only.
// (The const_name code is repeated to avoid introducing a "_" #define ourselves.)
#ifndef _
#    define PYBIND11_DETAIL_UNDERSCORE_BACKWARD_COMPATIBILITY
template <size_t N>
constexpr descr<N - 1> _(char const (&text)[N]) {
    return const_name<N>(text);
}
template <bool B, size_t N1, size_t N2>
constexpr enable_if_t<B, descr<N1 - 1>> _(char const (&text1)[N1], char const (&text2)[N2]) {
    return const_name<B, N1, N2>(text1, text2);
}
template <bool B, size_t N1, size_t N2>
constexpr enable_if_t<!B, descr<N2 - 1>> _(char const (&text1)[N1], char const (&text2)[N2]) {
    return const_name<B, N1, N2>(text1, text2);
}
template <bool B, typename T1, typename T2>
constexpr enable_if_t<B, T1> _(const T1 &d1, const T2 &d2) {
    return const_name<B, T1, T2>(d1, d2);
}
template <bool B, typename T1, typename T2>
constexpr enable_if_t<!B, T2> _(const T1 &d1, const T2 &d2) {
    return const_name<B, T1, T2>(d1, d2);
}

template <size_t Size>
auto constexpr _() -> remove_cv_t<decltype(int_to_str<Size / 10, Size % 10>::digits)> {
    return const_name<Size>();
}
template <typename Type>
constexpr descr<1, Type> _() {
    return const_name<Type>();
}
#endif // #ifndef _

constexpr descr<0> concat() { return {}; }

template <size_t N, typename... Ts>
constexpr descr<N, Ts...> concat(const descr<N, Ts...> &descr) {
    return descr;
}

template <size_t N, typename... Ts, typename... Args>
constexpr auto concat(const descr<N, Ts...> &d, const Args &...args)
    -> decltype(std::declval<descr<N + 2, Ts...>>() + concat(args...)) {
    return d + const_name(", ") + concat(args...);
}

template <size_t N, typename... Ts>
constexpr descr<N + 2, Ts...> type_descr(const descr<N, Ts...> &descr) {
    return const_name("{") + descr + const_name("}");
}

PYBIND11_NAMESPACE_END(detail)
PYBIND11_NAMESPACE_END(PYBIND11_NAMESPACE)
