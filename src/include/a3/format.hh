/*
 * FORMAT -- Better string formatting.
 *
 * Copyright (c) 2022, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 */

#pragma once

#include <string_view>
#include <variant>
#include <vector>

#include <a3/util.hh>

namespace a3 {

template <typename... Ts>
class TypeList;

template <typename T, typename... Ts>
class TypeList<T, Ts...> {
public:
    T               m_value;
    TypeList<Ts...> m_rest;

    consteval TypeList(T&& value, Ts&&... rest) :
        m_value { std::forward<T>(value) }, m_rest { std::forward<Ts>(rest)... } {}

    consteval size_t size() const { return 1 + m_rest.size(); }

    constexpr std::ostream& operator<<(std::ostream& stream) { return stream << m_value << m_rest; }
};

template <>
class TypeList<> {
public:
    consteval TypeList() {}

    consteval size_t size() const { return 0; }

    constexpr std::ostream& operator<<(std::ostream& stream) { return stream; }
};

template <typename T, typename... Us>
constexpr auto operator+(TypeList<T> lhs, TypeList<Us...> rhs) {
    return TypeList { lhs.m_value, rhs.m_value, rhs.m_rest };
}

template <typename... Ts, typename... Us>
constexpr auto operator+(TypeList<Ts...> lhs, TypeList<Us...> rhs) {
    return TypeList { lhs.m_value } + (lhs.m_rest + rhs);
}

template <typename... Ts>
TypeList(Ts&&...) -> TypeList<Ts...>;

template <typename... Ts>
class Formatted {
    A3_DEFAULTS(Formatted);

private:
    TypeList<Ts...> m_values;

    static consteval auto parse(std::string_view fmt) {
        //        auto pos = fmt.find('{');
        //        static_assert(pos == std::string_view::npos);
        return TypeList { fmt };
    }

    template <typename T, typename... Args>
    static consteval auto parse(std::string_view fmt, T&& arg, Args&&... args) {
        auto pos = fmt.find('{');
        //        static_assert(pos != std::string_view::npos);
        // static_assert(sizeof...(args) == 0, "Too many arguments provided.");
        // static_assert(fmt[pos + 1] == '}', "Invalid format string. Braces must be matched.");
        return TypeList { fmt.substr(0, pos), std::forward<T>(arg) } +
               parse(fmt.substr(pos + 2), std::forward<Args>(args)...);
    }

public:
    template <typename... Args>
    consteval Formatted(std::string_view fmt, Args&&... args) :
        m_values { parse(fmt, std::forward<Args>(args)...) } {}

    constexpr ~Formatted() {}

    constexpr std::ostream& operator<<(std::ostream& stream) {
        stream << m_values;
        return stream;
    }
};

/*template <size_t N, typename... Args>
consteval auto format(char const (&fmt)[N], Args&&... args) {
    return Formatted { fmt, std::forward<Args>(args)... };
    }*/

} // namespace a3
