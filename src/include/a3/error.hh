/*
 * ERROR -- Error type for use with Result.
 *
 * Copyright (c) 2022, Alex O'Brien <3541ax@gmail.com>
 *
 * This file is licensed under the BSD 3-clause license. See the LICENSE file in
 * the project root for details.
 *
 * This is an error type for use with Result, similarly inspired by Rust's Error trait and the
 * anyhow library.
 */

#pragma once

#ifndef __cpp_concepts
#warning "Error requires C++20 concepts."
#else

#include <cstring>
#include <iterator>
#include <memory>
#include <sstream>
#include <string_view>

#include <a3/option.hh>
#include <a3/result.hh>
#include <a3/util.hh>

namespace a3 {

namespace detail {

template <typename T>
concept IntoError = Streamable<T> && !std::derived_from<Error, T>;

}

class Error {
    A3_DEFAULTS(Error);

public:
    class Chain {
        Error const* m_current;

    public:
        Chain() : m_current { nullptr } {}
        explicit Chain(Error const& err) : m_current { &err } {}

        Chain& operator++() {
            if (m_current)
                m_current = m_current->source().map_or(nullptr, [](auto const& v) { return &v; });
            return *this;
        }

        Chain operator++(int) {
            Chain ret = *this;
            ++*this;
            return ret;
        }

        bool operator==(Chain const& other) const { return m_current == other.m_current; }

        Error const& operator*() const { return *m_current; }
        Error const* operator->() const { return m_current; }
    };

    Error() = default;
    virtual ~Error() {}

    virtual Option<Error const&>     source() const { return {}; }
    virtual Option<std::string_view> description() const = 0;

    Chain chain() const { return Chain { *this }; }
    Chain begin() const { return chain(); }
    Chain end() const { return Chain {}; };
};

inline std::ostream& operator<<(std::ostream& o, Error const& err) {
    o << "Error: " << err.description().value_or("") << " caused by: \n";
    for (auto const& c : err)
        o << "  * " << c.description().value_or("UNKNOWN") << "\n";
    return o;
}

template <detail::Streamable E>
class ErrorWrapper : public Error {
private:
    std::string m_message;

public:
    explicit ErrorWrapper(E const& err) {
        std::ostringstream stream;
        stream << err;
        m_message = std::move(stream).str();
    }

    Option<std::string_view> description() const override { return m_message; }
};

template <>
inline ErrorWrapper<std::error_code>::ErrorWrapper(std::error_code const& err) :
    m_message { 32, '\0' } {
#ifdef _GNU_SOURCE
    auto* str = strerror_r(err.value(), m_message.data(), m_message.size());
    if (str != m_message.c_str())
        m_message = std::string { str };
#elif defined(_MSC_VER)
    strerror_s(m_message.data(), m_message.size(), err.value());
#else
    strerror_r(err.value(), m_message.data(), m_message.size());
#endif

    m_message.resize(strnlen(m_message.c_str(), m_message.size()));
}

class ErrorContext : public Error {
private:
    std::unique_ptr<Error> m_source;
    std::string            m_description;

public:
    template <std::derived_from<Error> E>
    explicit ErrorContext(E err, std::string description) :
        m_source { std::make_unique<E>(std::move(err)) },
        m_description { std::move(description) } {}

    template <detail::IntoError E>
    explicit ErrorContext(E const& err, std::string description) :
        m_source { std::make_unique<ErrorWrapper<E>>(err) },
        m_description { std::move(description) } {}

    Option<Error const&>     source() const override { return *m_source; }
    Option<std::string_view> description() const override { return m_description; }
};

template <typename T, typename E>
    template <std::invocable<E const&> Fn>
    Result<T, std::unique_ptr<Error>> Result<T, E>::with_context(Fn&& f) &&
    requires(std::derived_from<E, Error> || detail::Streamable<E>) {
    static_assert(std::is_constructible_v<ErrorContext, E, std::invoke_result_t<Fn, E const&>>,
                  "Context callback must return valid value.");
    if (is_ok())
        return std::move(m_ok);

    if constexpr (detail::IS_REF<T>) {
        auto desc = std::forward<Fn>(f)(m_ok.get());
        return Err { std::make_unique<ErrorContext>(std::move(m_err).err(), std::move(desc)) };
    } else {
        auto desc = std::forward<Fn>(f)(m_ok);
        return Err { std::make_unique<ErrorContext>(std::move(m_err).err(), std::move(desc)) };
    }
}

} // namespace a3

#endif
