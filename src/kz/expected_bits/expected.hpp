/*
    Copyright (c) 2022, Thierry Tremblay
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include <memory>
#include <kz/expected_bits/exception.hpp>
#include <kz/expected_bits/unexpected.hpp>

// clang does not suport P0848R3. Details at
// https://clang.llvm.org/cxx_status.html#cxx20.
#if defined(__clang__)
#define KZ_P0848R3 0
#else
#define KZ_P0848R3 1
#endif

namespace kz {
    namespace detail {

#if KZ_EXCEPTIONS

        template <class T, class U, class... Args>
        constexpr void reinit_expected(T& newval, U& oldval, Args&&... args) {
            if constexpr (std::is_nothrow_constructible_v<T, Args...>) {
                std::destroy_at(std::addressof(oldval));
                std::construct_at(std::addressof(newval), std::forward<Args>(args)...);
            } else if constexpr (std::is_nothrow_move_constructible_v<T>) {
                T tmp(std::forward<Args>(args)...);
                std::destroy_at(std::addressof(oldval));
                std::construct_at(std::addressof(newval), std::move(tmp));
            } else {
                U tmp(std::move(oldval));
                std::destroy_at(std::addressof(oldval));
                try {
                    std::construct_at(std::addressof(newval), std::forward<Args>(args)...);
                } catch (...) {
                    std::construct_at(std::addressof(oldval), std::move(tmp));
                    throw;
                }
            }
        }

#else

        template <class T, class U, class... Args>
        constexpr void reinit_expected(T& newval, U& oldval, Args&&... args) {
            std::destroy_at(std::addressof(oldval));
            std::construct_at(std::addressof(newval), std::forward<Args>(args)...);
        }

#endif

        template<class T, template <class...> class V>
        struct is_specialization : std::false_type {};

        template<template <class...> class V, class... Args>
        struct is_specialization<V<Args...>, V>: std::true_type {};

    } // namespace detail

    /*
        expected
    */

    template <class T, class E>
    class expected {
    public:
        using value_type = T;
        using error_type = E;
        using unexpected_type = unexpected<E>;

        template <class U>
        using rebind = expected<U, error_type>;

        // Constructors
        constexpr expected()
        requires(std::is_default_constructible_v<T>)
            : _value(), _has_value(true) {}

#if KZ_P0848R3
        constexpr expected(const expected& rhs) noexcept(std::is_nothrow_copy_constructible_v<T> &&
                                                         std::is_nothrow_copy_constructible_v<E>)
        requires(
            std::is_copy_constructible_v<T> &&
            std::is_copy_constructible_v<E> &&
            !(std::is_trivially_copy_constructible_v<T> && std::is_trivially_copy_constructible_v<E>)) {
            if (rhs._has_value) {
                construct_value(rhs._value);
            } else {
                construct_error(rhs._error);
            }
        }
        constexpr expected(const expected&) noexcept
        requires(
            std::is_trivially_copy_constructible_v<T> &&
            std::is_trivially_copy_constructible_v<E>) = default;

        constexpr expected(const expected&)
        requires(
            !std::is_copy_constructible_v<T> ||
            !std::is_copy_constructible_v<E>) = delete;
#else
        constexpr expected(const expected& rhs) noexcept(std::is_nothrow_copy_constructible_v<T> &&
                                                         std::is_nothrow_copy_constructible_v<E>)
        requires(
            std::is_copy_constructible_v<T> &&
            std::is_copy_constructible_v<E>) {
            if (rhs._has_value) {
                construct_value(rhs._value);
            } else {
                construct_error(rhs._error);
            }
        }
#endif

#if KZ_P0848R3
        constexpr expected(expected&& rhs) noexcept(std::is_nothrow_move_constructible_v<T> &&
                                                    std::is_nothrow_move_constructible_v<E>)
        requires(
            std::is_move_constructible_v<T> &&
            std::is_move_constructible_v<E> &&
            !(std::is_trivially_move_constructible_v<T> && std::is_trivially_move_constructible_v<E>)) {
            if (rhs._has_value) {
                construct_value(std::move(rhs._value));
            } else {
                construct_error(std::move(rhs._error));
            }
        }

        constexpr expected(expected&&) noexcept
        requires(
            std::is_trivially_move_constructible_v<T> &&
            std::is_trivially_move_constructible_v<E>) = default;
#else
        constexpr expected(expected&& rhs) noexcept(std::is_nothrow_move_constructible_v<T> &&
                                                    std::is_nothrow_move_constructible_v<E>)
        requires(
            std::is_move_constructible_v<T> &&
            std::is_move_constructible_v<E>) {
            if (rhs._has_value) {
                construct_value(std::move(rhs._value));
            } else {
                construct_error(std::move(rhs._error));
            }
        }
#endif

        template <class U, class G>
        constexpr explicit(!std::is_convertible_v<const U&, T> ||
                           !std::is_convertible_v<const G&, E>)
        expected(const expected<U, G>& rhs)
        requires(
            std::is_constructible_v<T, const U&> &&
            std::is_constructible_v<E, const G&> &&
            !std::is_constructible_v<T, expected<U, G>&> &&
            !std::is_constructible_v<T, expected<U, G>> &&
            !std::is_constructible_v<T, const expected<U, G>&> &&
            !std::is_constructible_v<T, const expected<U, G>> &&
            !std::is_convertible_v<expected<U, G>&, T> &&
            !std::is_convertible_v<expected<U, G>&&, T> &&
            !std::is_convertible_v<const expected<U, G>&, T> &&
            !std::is_convertible_v<const expected<U, G>&&, T> &&
            !std::is_constructible_v<unexpected<E>, expected<U, G>&> &&
            !std::is_constructible_v<unexpected<E>, expected<U, G>> &&
            !std::is_constructible_v<unexpected<E>, const expected<U, G>&> &&
            !std::is_constructible_v<unexpected<E>, const expected<U, G>>) {
            if (rhs.has_value()) {
                construct_value(std::forward<const U&>(*rhs));
            } else {
                construct_error(std::forward<const G&>(rhs.error()));
            }
        }

        template <class U, class G>
        constexpr explicit(!std::is_convertible_v<U, T> ||
                           !std::is_convertible_v<G, E>)
        expected(expected<U, G>&& rhs)
        requires(
            std::is_constructible_v<T, U> &&
            std::is_constructible_v<E, G> &&
            !std::is_constructible_v<T, expected<U, G>&> &&
            !std::is_constructible_v<T, expected<U, G>> &&
            !std::is_constructible_v<T, const expected<U, G>&> &&
            !std::is_constructible_v<T, const expected<U, G>> &&
            !std::is_convertible_v<expected<U, G>&, T> &&
            !std::is_convertible_v<expected<U, G>&&, T> &&
            !std::is_convertible_v<const expected<U, G>&, T> &&
            !std::is_convertible_v<const expected<U, G>&&, T> &&
            !std::is_constructible_v<unexpected<E>, expected<U, G>&> &&
            !std::is_constructible_v<unexpected<E>, expected<U, G>> &&
            !std::is_constructible_v<unexpected<E>, const expected<U, G>&> &&
            !std::is_constructible_v<unexpected<E>, const expected<U, G>>) {
            if (rhs.has_value()) {
                construct_value(std::forward<U>(*rhs));
            } else {
                construct_error(std::forward<G>(rhs.error()));
            }
        }

        template <class U = T>
        constexpr explicit(!std::is_convertible_v<U, T>)
        expected(U&& v)
        requires(
            !std::is_same_v<std::remove_cvref_t<U>, in_place_t> &&
            !std::is_same_v<expected<T, E>, std::remove_cvref_t<U>> &&
            !detail::is_specialization<std::remove_cvref_t<U>, unexpected>::value &&
            std::is_constructible_v<T, U>)
            : _value(std::forward<U>(v)), _has_value(true) {}

        template <class G>
        constexpr explicit(!std::is_convertible_v<const G&, E>)
        expected(const unexpected<G>& e)
        requires(std::is_constructible_v<E, const G&>)
            : _error(std::forward<const G&>(e.value())), _has_value(false) {}

        template <class G>
        constexpr explicit(!std::is_convertible_v<G, E>)
        expected(unexpected<G>&& e)
        requires(std::is_constructible_v<E, G>)
            : _error(std::forward<G>(e.value())), _has_value(false) {}

        template <class... Args>
        constexpr explicit expected(in_place_t, Args&&... args)
        requires(std::is_constructible_v<T, Args...>)
            : _value(std::forward<Args>(args)...), _has_value(true) {}

        template <class U, class... Args>
        constexpr explicit expected(in_place_t, initializer_list<U> il, Args&&... args)
        requires(std::is_constructible_v<T, initializer_list<U>&, Args...>)
            : _value(il, std::forward<Args>(args)...), _has_value(true) {}

        template <class... Args>
        constexpr explicit expected(unexpect_t, Args&&... args)
        requires(std::is_constructible_v<E, Args...>)
            : _error(std::forward<Args>(args)...), _has_value(false) {}

        template <class U, class... Args>
        constexpr explicit expected(unexpect_t, initializer_list<U> il, Args&&... args)
        requires(std::is_constructible_v<E, initializer_list<U>&, Args...>)
            : _error(il, std::forward<Args>(args)...), _has_value(false) {}

        // Destructor
        constexpr ~expected() {
            if (_has_value) {
                std::destroy_at(std::addressof(_value));
            } else {
                std::destroy_at(std::addressof(_error));
            }
        }

#if KZ_P0848R3
        constexpr ~expected() requires(std::is_trivially_destructible_v<T> &&
                                       !std::is_trivially_destructible_v<E>) {
            if (!_has_value) {
                std::destroy_at(std::addressof(_error));
            }
        }

        constexpr ~expected() requires(!std::is_trivially_destructible_v<T> &&
                                       std::is_trivially_destructible_v<E>) {
            if (_has_value) {
                std::destroy_at(std::addressof(_value));
            }
        }

        constexpr ~expected() requires(std::is_trivially_destructible_v<T> &&
                                       std::is_trivially_destructible_v<E>) = default;
#endif

        // Assignment
        constexpr expected& operator=(const expected& rhs) noexcept(
            std::is_nothrow_copy_assignable_v<T> &&
            std::is_nothrow_copy_constructible_v<T> &&
            std::is_nothrow_copy_assignable_v<E> &&
            std::is_nothrow_copy_constructible_v<E>)
        requires(
            std::is_copy_assignable_v<T> &&
            std::is_copy_constructible_v<T> &&
            std::is_copy_assignable_v<E> &&
            std::is_copy_constructible_v<E> &&
            (std::is_nothrow_move_constructible_v<T> || std::is_nothrow_move_constructible_v<E>)) {
            if (_has_value) {
                if (rhs._has_value) {
                    _value = rhs._value;
                } else {
                    detail::reinit_expected(_error, _value, rhs._error);
                    _has_value = false;
                }
            } else {
                if (rhs._has_value) {
                    detail::reinit_expected(_value, _error, rhs._value);
                    _has_value = true;
                } else {
                    _error = rhs._error;
                }
            }
            return *this;
        }

        constexpr expected& operator=(const expected&) = delete;

        constexpr expected& operator=(expected&& rhs) noexcept(
            std::is_nothrow_move_assignable_v<T> &&
            std::is_nothrow_move_constructible_v<T> &&
            std::is_nothrow_move_assignable_v<E> &&
            std::is_nothrow_move_constructible_v<E>)
        requires(
            std::is_move_constructible_v<T> &&
            std::is_move_assignable_v<T> &&
            std::is_move_constructible_v<E> &&
            std::is_move_assignable_v<E> &&
            (std::is_nothrow_move_constructible_v<T> || std::is_nothrow_move_constructible_v<E>)) {
            if (_has_value) {
                if (rhs._has_value) {
                    _value = std::move(rhs._value);
                } else {
                    detail::reinit_expected(_error, _value, std::move(rhs._error));
                    _has_value = false;
                }
            } else {
                if (rhs._has_value) {
                    detail::reinit_expected(_value, _error, std::move(rhs._value));
                    _has_value = true;
                } else {
                    _error = std::move(rhs._error);
                }
            }
            return *this;
        }

        constexpr expected& operator=(expected&&) = delete;

        template <class U = T>
        constexpr expected& operator=(U&& v)
        requires(
            !std::is_same_v<expected, std::remove_cvref_t<U>> &&
            !detail::is_specialization<std::remove_cvref_t<U>, unexpected>::value &&
            std::is_constructible_v<T, U> &&
            std::is_assignable_v<T&, U> &&
            (std::is_nothrow_constructible_v<T, U> || std::is_nothrow_move_constructible_v<T> || std::is_nothrow_move_constructible_v<E>)) {
            if (_has_value) {
                _value = std::forward<U>(v);
            } else {
                detail::reinit_expected(_value, _error, std::forward<U>(v));
                _has_value = true;
            }
            return *this;
        }

        template <class G>
        constexpr expected& operator=(const unexpected<G>& e)
        requires(
            std::is_constructible_v<E, const G&> &&
            std::is_assignable_v<E&, const G&> &&
            (std::is_nothrow_constructible_v<E, const G&> || std::is_nothrow_move_constructible_v<T> || std::is_nothrow_move_constructible_v<E>)) {
            if (_has_value) {
                detail::reinit_expected(_error, _value, std::forward<const G&>(e.value()));
                _has_value = false;
            } else {
                _error = std::forward<const G&>(e.value());
            }
            return *this;
        }

        template <class G>
        constexpr expected& operator=(unexpected<G>&& e)
        requires(
            std::is_constructible_v<E, G> &&
            std::is_assignable_v<E&, G> &&
            (std::is_nothrow_constructible_v<E, G> || std::is_nothrow_move_constructible_v<T>|| std::is_nothrow_move_constructible_v<E>)) {
            if (_has_value) {
                detail::reinit_expected(_error, _value, std::forward<G>(e.value()));
                _has_value = false;
            } else {
                _error = std::forward<G>(e.value());
            }
            return *this;
        }

        // Modifiers
        template <class... Args>
        constexpr T& emplace(Args&&... args) noexcept
        requires(std::is_nothrow_constructible_v<T, Args...>) {
            if (_has_value)
                std::destroy_at(std::addressof(_value));
            else {
                std::destroy_at(std::addressof(_error));
            }
            construct_value(std::forward<Args>(args)...);
            return _value;
        }

        template <class U, class... Args>
        constexpr T& emplace(initializer_list<U> il, Args&&... args) noexcept
        requires(std::is_nothrow_constructible_v<T, std::initializer_list<U>&, Args...>) {
            if (_has_value)
                std::destroy_at(std::addressof(_value));
            else {
                std::destroy_at(std::addressof(_error));
            }
            construct_value(il, std::forward<Args>(args)...);
            return _value;
        }

        // Swap
        constexpr void swap(expected& rhs) noexcept(
            std::is_nothrow_move_constructible_v<T> &&
            std::is_nothrow_swappable_v<T> &&
            std::is_nothrow_move_constructible_v<E> &&
            std::is_nothrow_swappable_v<E>)
        requires(
            std::is_swappable_v<T> &&
            std::is_swappable_v<E> &&
            std::is_move_constructible_v<T> &&
            std::is_move_constructible_v<E> &&
            (std::is_nothrow_move_constructible_v<T> || std::is_nothrow_move_constructible_v<E>)) {
            if (_has_value) {
                if (rhs._has_value) {
                    using std::swap;
                    std::swap(_value, rhs._value);
                } else {
#if KZ_EXCEPTIONS
                    if constexpr (std::is_nothrow_move_constructible_v<T> &&
                                  std::is_nothrow_move_constructible_v<E>) {
                        E tmp(std::move(rhs._error));
                        std::destroy_at(std::addressof(rhs._error));
                        std::construct_at(std::addressof(rhs._value), std::move(_value));
                        std::destroy_at(std::addressof(_value));
                        std::construct_at(std::addressof(_error), std::move(tmp));

                    } else if constexpr (std::is_nothrow_move_constructible_v<E>) {
                        E tmp(std::move(rhs._error));
                        std::destroy_at(std::addressof(rhs._error));
                        try {
                            std::construct_at(std::addressof(rhs._value), std::move(_value));
                            std::destroy_at(std::addressof(_value));
                            std::construct_at(std::addressof(_error), std::move(tmp));
                        } catch (...) {
                            std::construct_at(std::addressof(rhs._error), std::move(tmp));
                            throw;
                        }
                    } else {
                        T tmp(std::move(_value));
                        std::destroy_at(std::addressof(_value));
                        try {
                            std::construct_at(std::addressof(_error), std::move(rhs._error));
                            std::destroy_at(std::addressof(rhs._error));
                            std::construct_at(std::addressof(rhs._value), std::move(tmp));
                        } catch (...) {
                            std::construct_at(std::addressof(_value), std::move(tmp));
                            throw;
                        }
                    }
                    _has_value = false;
                    rhs._has_value = true;
#else
                    E tmp(std::move(rhs._error));
                    std::destroy_at(std::addressof(rhs._error));
                    rhs.construct_value(std::move(_value));
                    std::destroy_at(std::addressof(_value));
                    construct_error(std::move(tmp));
#endif
                }
            } else {
                if (rhs._has_value) {
                    rhs.swap(*this);
                } else {
                    using std::swap;
                    std::swap(_error, rhs._error);
                }
            }
        }

        friend constexpr void swap(expected& x, expected& y) noexcept(noexcept(x.swap(y))) {
            x.swap(y);
        }

        // Observers
        constexpr const T*  operator->() const noexcept    { return std::addressof(_value); }
        constexpr T*        operator->() noexcept          { return std::addressof(_value); }
        constexpr const T&  operator*() const&             { return _value; }
        constexpr T&        operator*() &                  { return _value; }
        constexpr const T&& operator*() const&&            { return std::move(_value); }
        constexpr T&&       operator*() &&                 { return std::move(_value); }
        constexpr explicit  operator bool() const noexcept { return _has_value; }
        constexpr bool      has_value() const noexcept     { return _has_value; }

        constexpr const T& value() const& {
            if (!_has_value) KZ_THROW(bad_expected_access(_error));
            return _value;
        }

        constexpr T& value() & {
            if (!_has_value) KZ_THROW(bad_expected_access(_error));
            return _value;
        }

        constexpr const T&& value() const&& {
            if (!_has_value) KZ_THROW(bad_expected_access(std::move(_error)));
            return std::move(_value);
        }

        constexpr T&& value() && {
            if (!_has_value) KZ_THROW(bad_expected_access(std::move(_error)));
            return std::move(_value);
        }

        constexpr const E&  error() const&  { return _error; }
        constexpr E&        error() &       { return _error; }
        constexpr const E&& error() const&& { return std::move(_error); }
        constexpr E&&       error() &&      { return std::move(_error); }

        template <class U>
        constexpr T value_or(U&& v) const& {
            return _has_value ? _value
                               : static_cast<T>(std::forward<U>(v));
        }

        template <class U>
        constexpr T value_or(U&& v) && {
            return _has_value ? std::move(_value)
                               : static_cast<T>(std::forward<U>(v));
        }

        // Equality operators

        template <class T2, class E2>
        requires(!std::is_void_v<T2>)
        friend constexpr bool operator==(const expected& x, const expected<T2, E2>& y) {
            if (x.has_value())
                return y.has_value() && static_cast<bool>(x.value() == y.value());
            else
                return !y.has_value() && static_cast<bool>(x.error() == y.error());
        }

        template <class T2>
        friend constexpr bool operator==(const expected& x, const T2& v) {
            return x.has_value() && static_cast<bool>(*x == v);
        }

        template <class E2>
        friend constexpr bool operator==(const expected& x, const unexpected<E2>& e) {
            return !x.has_value() && static_cast<bool>(x.error() == e.value());
        }

    private:
        template <class... Args>
        constexpr void construct_value(Args&&... args) {
            std::construct_at(std::addressof(_value), std::forward<Args>(args)...);
            _has_value = true;
        }

        template <class... Args>
        constexpr void construct_error(Args&&... args) {
            std::construct_at(std::addressof(_error), std::forward<Args>(args)...);
            _has_value = false;
        }

        union {
            T _value;
            E _error;
        };
        bool _has_value;
    };

    /*
        Partial specialization for void types
    */

    template <class T, class E>
    requires std::is_void_v<T>
    class expected<T, E> {
    public:
        using value_type = T;
        using error_type = E;
        using unexpected_type = unexpected<E>;

        template <class U>
        using rebind = expected<U, error_type>;

        // Constructors
        constexpr expected() noexcept : _has_value(true) {}

#if KZ_P0848R3
        constexpr expected(const expected& rhs)
        requires(
            std::is_copy_constructible_v<E> &&
            !std::is_trivially_copy_constructible_v<E>) {
            if (rhs._has_value) {
                construct_value();
            } else {
                construct_error(rhs._error);
            }
        }

        constexpr expected(const expected&) noexcept
        requires(std::is_trivially_copy_constructible_v<E>) = default;

        constexpr expected(const expected&)
        requires(!std::is_copy_constructible_v<T> && !std::is_copy_constructible_v<E>) = delete;

#else
        constexpr expected(const expected& rhs)
        requires(std::is_copy_constructible_v<E>) {
            if (rhs._has_value) {
                construct_value();
            } else {
                construct_error(rhs._error);
            }
        }
#endif

#if KZ_P0848R3
        constexpr expected(expected&& rhs) noexcept(std::is_nothrow_move_constructible_v<E>)
        requires(
            std::is_move_constructible_v<E> &&
            !std::is_trivially_move_constructible_v<E>) {
            if (rhs._has_value) {
                construct_value();
            } else {
                construct_error(std::move(rhs._error));
            }
        }

        constexpr expected(expected&&) noexcept
        requires(std::is_trivially_move_constructible_v<E>) = default;

#else
        constexpr expected(expected&& rhs) noexcept(std::is_nothrow_move_constructible_v<E>)
        requires(std::is_move_constructible_v<E>) {
            if (rhs._has_value) {
                construct_value();
            } else {
                construct_error(std::move(rhs._error));
            }
        }
#endif

        template <class U, class G>
        constexpr explicit(!std::is_convertible_v<const G&, E>)
        expected(const expected<U, G>& rhs)
        requires(
                std::is_void_v<U> &&
                std::is_constructible_v<E, const G&> &&
                !std::is_constructible_v<unexpected<E>, expected<U, G>&> &&
                !std::is_constructible_v<unexpected<E>, expected<U, G>> &&
                !std::is_constructible_v<unexpected<E>, const expected<U, G>&> &&
                !std::is_constructible_v<unexpected<E>, const expected<U, G>>) {
            if (bool(rhs)) {
                construct_value();
            } else {
                construct_error(std::forward<const G&>(rhs.error()));
            }
        }

        template <class U, class G>
        constexpr explicit(!std::is_convertible_v<G, E>)
        expected(expected<U, G>&& rhs)
        requires(
                std::is_void_v<U> &&
                std::is_constructible_v<E, G> &&
                !std::is_constructible_v<unexpected<E>, expected<U, G>&> &&
                !std::is_constructible_v<unexpected<E>, expected<U, G>> &&
                !std::is_constructible_v<unexpected<E>, const expected<U, G>&> &&
                !std::is_constructible_v<unexpected<E>, const expected<U, G>>) {
            if (bool(rhs)) {
                construct_value();
            } else {
                construct_error(std::forward<G>(rhs.error()));
            }
        }

        template <class G>
        constexpr explicit(!std::is_convertible_v<const G&, E>)
        expected(const unexpected<G>& e)
        requires(std::is_constructible_v<E, const G&>)
            : _error(std::forward<const G&>(e.value())), _has_value(false) {}

        template <class G>
        constexpr explicit(!std::is_convertible_v<G, E>)
        expected(unexpected<G>&& e)
        requires(std::is_constructible_v<E, G>)
            : _error(std::forward<G>(e.value())), _has_value(false) {}

        constexpr explicit expected(in_place_t) noexcept : _has_value(true) {}

        template <class... Args>
        constexpr explicit expected(unexpect_t, Args&&... args)
        requires(std::is_constructible_v<E, Args...>)
            : _error(std::forward<Args>(args)...), _has_value(false) {}

        template <class U, class... Args>
        constexpr explicit expected(unexpect_t, initializer_list<U> il, Args&&... args)
            requires(std::is_constructible_v<E, initializer_list<U>&, Args...>)
            : _error(il, std::forward<Args>(args)...), _has_value(false) {}

        // Destructor
        constexpr ~expected() {
            if (!_has_value) {
                _error.~E();
            }
        }

#if KZ_P0848R3
        constexpr ~expected()
        requires(std::is_trivially_destructible_v<E>) = default;
#endif

        // Assignment
        constexpr expected& operator=(const expected& rhs) noexcept(
            std::is_nothrow_copy_assignable_v<E> &&
            std::is_nothrow_copy_constructible_v<E>)
        requires(
            std::is_copy_assignable_v<E> &&
            std::is_copy_constructible_v<E>) {
            if (_has_value) {
                if (!rhs._has_value) {
                    construct_error(rhs._error);
                }
            } else {
                if (rhs._has_value) {
                    std::destroy_at(std::addressof(_error));
                    construct_value();
                } else {
                    _error = rhs._error;
                }
            }
            return *this;
        }

        constexpr expected& operator=(const expected&) = delete;

        constexpr expected& operator=(expected&& rhs) noexcept(
            std::is_nothrow_move_assignable_v<E> &&
            std::is_nothrow_move_constructible_v<E>)
        requires(
            std::is_move_constructible_v<E> &&
            std::is_move_assignable_v<E>) {
            if (_has_value) {
                if (!rhs._has_value) {
                    construct_error(std::move(rhs._error));
                }
            } else {
                if (rhs._has_value) {
                    std::destroy_at(std::addressof(_error));
                    construct_value();
                } else {
                    _error = std::move(rhs._error);
                }
            }
            return *this;
        }

        template <class G>
        constexpr expected& operator=(const unexpected<G>& e)
        requires(
                std::is_constructible_v<E, const G&> &&
                std::is_assignable_v<E&, const G&>) {
            if (_has_value) {
                construct_error(std::forward<const G&>(e.value()));
            } else {
                _error = std::forward<const G&>(e.value());
            }
            return *this;
        }

        template <class G>
        constexpr expected& operator=(unexpected<G>&& e)
        requires(
            std::is_constructible_v<E, G> &&
            std::is_assignable_v<E&, G>) {
            if (_has_value) {
                construct_error(std::forward<G>(e.value()));
            } else {
                _error = std::forward<G>(e.value());
            }
            return *this;
        }

        // Modifiers
        constexpr void emplace() noexcept {
            if (!_has_value) {
                std::destroy_at(std::addressof(_error));
                _has_value = true;
            }
        }

        // Swap
        constexpr void swap(expected& rhs) noexcept(
            std::is_nothrow_move_constructible_v<E> &&
            std::is_nothrow_swappable_v<E>)
        requires(
            std::is_swappable_v<E> &&
            std::is_move_constructible_v<E>) {
            if (_has_value) {
                if (!rhs._has_value) {
                    construct_error(std::move(rhs._error));
                    std::destroy_at(std::addressof(rhs._error));
                    rhs._has_value = true;
                }
            } else {
                if (rhs._has_value) {
                    rhs.swap(*this);
                } else {
                    using std::swap;
                    swap(_error, rhs._error);
                }
            }
        }

        friend constexpr void swap(expected& x, expected& y) noexcept(noexcept(x.swap(y))) {
            x.swap(y);
        }

        // Observers
        constexpr explicit operator bool() const noexcept { return _has_value; }
        constexpr bool has_value() const noexcept         { return _has_value; }
        constexpr void operator*() const noexcept         {}

        constexpr void value() const& {
            if (!_has_value) KZ_THROW(bad_expected_access(_error));
        }

        constexpr void value() && {
            if (!_has_value) KZ_THROW(bad_expected_access(std::move(_error)));
        }

        constexpr const E&  error() const&  { return _error; }
        constexpr E&        error() &       { return _error; }
        constexpr const E&& error() const&& { return std::move(_error); }
        constexpr E&&       error() &&      { return std::move(_error); }

        // Equality operators
        template <class T2, class E2>
        requires(std::is_void_v<T2>)
        friend constexpr bool operator==(const expected& x, const expected<T2, E2>& y) {
            if (x.has_value())
                return y.has_value();
            else
                return !y.has_value() &&
                       static_cast<bool>(x.error() == y.error());
        }

        template <class E2>
        friend constexpr bool operator==(const expected& x, const unexpected<E2>& e) {
            return !x.has_value() && static_cast<bool>(x.error() == e.value());
        }

    private:
        constexpr void construct_value() {
            _has_value = true;
        }

        template <class... Args>
        constexpr void construct_error(Args&&... args) {
            std::construct_at(
                std::addressof(_error), std::forward<Args>(args)...);
            _has_value = false;
        }

        union {
            E _error;
        };
        bool _has_value;
    };

} // namespace kz
