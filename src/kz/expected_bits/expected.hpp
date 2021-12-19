/*
    Copyright (c) 2021, Thierry Tremblay
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

#include <kz/expected_bits/exception.hpp>
#include <kz/expected_bits/expected_storage.hpp>

namespace kz {

    using std::in_place;

    // �.�.4 Class template expected [expected.expected]
    template <typename T, typename E>
    requires(!std::is_same_v<T, unexpected<E>>) class expected
        : private detail::expected_storage<T, E>,
          private detail::expected_default_constructors<T, E> {
    public:
        using value_type = T;
        using error_type = E;
        using unexpected_type = unexpected<E>;

        template <typename U>
        using rebind = expected<U, error_type>;

        // �.�.4.1 Constructors [expected.object.ctor]
        constexpr expected() = default;
        constexpr expected(const expected& rhs) = default;
        constexpr expected(expected&& rhs) = default;

        template <typename U, typename G>
        requires(
            std::is_constructible_v<T, const U&> &&
            !std::is_constructible_v<T, expected<U, G>&> &&
            !std::is_constructible_v<T, expected<U, G>&&> &&
            !std::is_constructible_v<T, const expected<U, G>&> &&
            !std::is_constructible_v<T, const expected<U, G>&&> &&
            !std::is_convertible_v<expected<U, G>&, T> &&
            !std::is_convertible_v<expected<U, G>&&, T> &&
            !std::is_convertible_v<const expected<U, G>&, T> &&
            !std::is_convertible_v<const expected<U, G>&&, T> &&
            std::is_constructible_v<E, const G&> &&
            !std::is_constructible_v<unexpected<E>, expected<U, G>&> &&
            !std::is_constructible_v<unexpected<E>, expected<U, G>&&> &&
            !std::is_constructible_v<unexpected<E>, const expected<U, G>&> &&
            !std::is_constructible_v<unexpected<E>, const expected<U, G>&&> &&
            !std::is_convertible_v<expected<U, G>&, unexpected<E>> &&
            !std::is_convertible_v<expected<U, G>&&, unexpected<E>> &&
            !std::is_convertible_v<const expected<U, G>&, unexpected<E>> &&
            !std::is_convertible_v<const expected<U, G>&&,
                unexpected<
                    E>>) explicit((!std::is_void_v<T> && !std::is_void_v<U> &&
                                      !std::is_convertible_v<const U&, T>) ||
                                  !std::is_convertible_v<const G&,
                                      E>) constexpr expected(const expected<U,
            G>& rhs)
            : detail::expected_storage<T, E>(rhs) {}

        template <typename G>
        requires(
            std::is_void_v<T>&& std::is_constructible_v<E, const G&> &&
            !std::is_constructible_v<unexpected<E>, expected<void, G>&> &&
            !std::is_constructible_v<unexpected<E>, expected<void, G>&&> &&
            !std::is_constructible_v<unexpected<E>, const expected<void, G>&> &&
            !std::is_constructible_v<unexpected<E>,
                const expected<void, G>&&> &&
            !std::is_convertible_v<expected<void, G>&, unexpected<E>> &&
            !std::is_convertible_v<expected<void, G>&&, unexpected<E>> &&
            !std::is_convertible_v<const expected<void, G>&, unexpected<E>> &&
            !std::is_convertible_v<const expected<void, G>&&,
                unexpected<
                    E>>) explicit(!std::is_convertible_v<const G&,
                                  E>) constexpr expected(const expected<void,
            G>& rhs)
            : detail::expected_storage<void, E>(rhs) {}

        template <typename U, typename G>
        requires(
            std::is_constructible_v<T, U&&> &&
            !std::is_constructible_v<T, expected<U, G>&> &&
            !std::is_constructible_v<T, expected<U, G>&&> &&
            !std::is_constructible_v<T, const expected<U, G>&> &&
            !std::is_constructible_v<T, const expected<U, G>&&> &&
            !std::is_convertible_v<expected<U, G>&, T> &&
            !std::is_convertible_v<expected<U, G>&&, T> &&
            !std::is_convertible_v<const expected<U, G>&, T> &&
            !std::is_convertible_v<const expected<U, G>&&, T> &&
            std::is_constructible_v<E, G&&> &&
            !std::is_constructible_v<unexpected<E>, expected<U, G>&> &&
            !std::is_constructible_v<unexpected<E>, expected<U, G>&&> &&
            !std::is_constructible_v<unexpected<E>, const expected<U, G>&> &&
            !std::is_constructible_v<unexpected<E>, const expected<U, G>&&> &&
            !std::is_convertible_v<expected<U, G>&, unexpected<E>> &&
            !std::is_convertible_v<expected<U, G>&&, unexpected<E>> &&
            !std::is_convertible_v<const expected<U, G>&, unexpected<E>> &&
            !std::is_convertible_v<const expected<U, G>&&, unexpected<E>>)

            explicit((!std::is_void_v<T> && !std::is_void_v<U> &&
                         !std::is_convertible_v<U&&, T>) ||
                     !std::is_convertible_v<G&&,
                         E>) constexpr expected(expected<U, G>&& rhs)
            : detail::expected_storage<T, E>(std::move(rhs)) {}

        template <typename G>
        requires(
            std::is_void_v<T>&& std::is_constructible_v<E, G&&> &&
            !std::is_constructible_v<unexpected<E>, expected<void, G>&> &&
            !std::is_constructible_v<unexpected<E>, expected<void, G>&&> &&
            !std::is_constructible_v<unexpected<E>, const expected<void, G>&> &&
            !std::is_constructible_v<unexpected<E>,
                const expected<void, G>&&> &&
            !std::is_convertible_v<expected<void, G>&, unexpected<E>> &&
            !std::is_convertible_v<expected<void, G>&&, unexpected<E>> &&
            !std::is_convertible_v<const expected<void, G>&, unexpected<E>> &&
            !std::is_convertible_v<const expected<void, G>&&,
                unexpected<E>>) explicit(!std::is_convertible_v<G&&,
                                         E>) constexpr expected(expected<void,
            G>&& rhs)
            : detail::expected_storage<T, E>(std::move(rhs)) {}

        template <typename U = T>
        requires(std::is_constructible_v<T, U&&> &&
                 !std::is_same_v<std::remove_cvref_t<U>, in_place_t> &&
                 !std::is_same_v<std::remove_cvref_t<U>, expected<T, E>> &&
                 !std::is_same_v<std::remove_cvref_t<U>,
                     unexpected<E>>) explicit(!std::is_convertible_v<U&&,
                                              T>) constexpr expected(U&& v)
            : detail::expected_storage<T, E>(in_place, std::forward<U>(v)) {}

        template <typename G = E>
        requires(std::is_constructible_v<E, const G&>) explicit(
            !std::is_convertible_v<const G&,
                E>) constexpr expected(const unexpected<G>& e)
            : detail::expected_storage<T, E>(unexpect, e) {}

        template <typename G = E>
        requires(std::is_constructible_v<E, G&&>) explicit(
            !std::is_convertible_v<G&&, E>) constexpr expected(unexpected<G>&&
                e) noexcept(std::is_nothrow_constructible_v<E, G&&>)
            : detail::expected_storage<T, E>(unexpect, std::move(e)) {}

        template <typename... Args>
        requires((std::is_void_v<T> && sizeof...(Args) == 0) ||
                 (!std::is_void_v<T> &&
                     std::is_constructible_v<T,
                         Args...>)) constexpr explicit expected(in_place_t,
            Args&&... args)
            : detail::expected_storage<T, E>(
                  in_place, std::forward<Args>(args)...) {}

        template <typename U, typename... Args>
        requires(!std::is_void_v<T> &&
                 std::is_constructible_v<T, initializer_list<U>&,
                     Args...>) constexpr explicit expected(in_place_t,
            initializer_list<U> il, Args&&... args)
            : detail::expected_storage<T, E>(
                  in_place, il, std::forward<Args>(args)...) {}

        template <typename... Args>
        requires(std::is_constructible_v<E,
            Args...>) constexpr explicit expected(unexpect_t, Args&&... args)
            : detail::expected_storage<T, E>(
                  unexpect, in_place, std::forward<Args>(args)...) {}

        template <typename U, typename... Args>
        requires(std::is_constructible_v<E, initializer_list<U>&,
            Args...>) constexpr explicit expected(unexpect_t,
            initializer_list<U> il, Args&&... args)
            : detail::expected_storage<T, E>(
                  unexpect, in_place, il, std::forward<Args>(args)...) {}

        // �.�.4.2 Destructor [expected.object.dtor]
        ~expected() = default;

        // �.�.4.3 Assignment [expected.object.assign]
        expected& operator=(const expected&) = default;
        expected& operator=(expected&&) = default;

        template <typename U = T, typename V = T>
        requires(!std::is_void_v<T> &&
                 !std::is_same_v<expected<T, E>, std::remove_cvref_t<U>> &&
                 !std::conjunction_v<std::is_scalar<T>,
                     std::is_same<T, std::decay_t<U>>> &&
                 std::is_constructible_v<T, U> && std::is_assignable_v<V&, U> &&
                 std::is_same_v<T, V> &&
                 std::is_nothrow_move_constructible_v<E>) expected&
        operator=(U&& rhs) {
            detail::expected_storage<T, E>::operator=(std::forward<U>(rhs));
            return *this;
        }

        template <typename G = E>
        requires(std::is_nothrow_copy_constructible_v<E>&&
                std::is_copy_assignable_v<E>) expected&
        operator=(const unexpected<G>& rhs) {
            detail::expected_storage<T, E>::operator=(rhs);
            return *this;
        }

        template <typename G = E>
        requires(std::is_nothrow_move_constructible_v<E>&&
                std::is_move_assignable_v<E>) expected&
        operator=(unexpected<G>&& rhs) {
            detail::expected_storage<T, E>::operator=(std::move(rhs));
            return *this;
        }

        // �.�.4.4 Modifiers [expected.object.modify]
        template <typename = void>
        requires((std::is_void_v<T> ||
                     std::is_swappable_v<T>)&&std::is_swappable_v<E> &&
                 (std::is_void_v<T> || std::is_move_constructible_v<T> ||
                     std::is_move_constructible_v<E>)) void swap(expected&
                rhs) noexcept(std::is_nothrow_move_constructible_v<T>&&
                std::is_nothrow_swappable_v<T>&&
                    std::is_nothrow_move_constructible_v<E>&&
                        std::is_nothrow_swappable_v<E>) {
            detail::expected_storage<T, E>::swap(rhs);
        }

        template <typename = void>
        requires(std::is_void_v<T>) void emplace() {
            detail::expected_storage<T, E>::emplace();
        }

        template <typename... Args>
        requires(!std::is_void_v<T>) auto& emplace(Args&&... args) {
            return detail::expected_storage<T, E>::emplace(
                std::forward<Args>(args)...);
        }

        template <typename U, typename... Args>
        requires(!std::is_void_v<T>) auto& emplace(
            initializer_list<U> il, Args&&... args) {
            return detail::expected_storage<T, E>::emplace(
                il, std::forward<Args>(args)...);
        }

        // �.�.4.5 Observers [expected.object.observe]
        constexpr const T* operator->() const {
            return std::addressof(this->_value);
        }

        template <typename U = T>
        requires(!std::is_void_v<T> && std::is_same_v<T, U>) constexpr T*
        operator->() {
            return std::addressof(this->_value);
        }

        template <typename U = T>
        requires(!std::is_void_v<T> && std::is_same_v<T, U>) constexpr const U&
        operator*() const& {
            return this->_value;
        }

        template <typename U = T>
        requires(!std::is_void_v<T> && std::is_same_v<T, U>) constexpr U&
        operator*() & {
            return this->_value;
        }

        template <typename U = T>
        requires(!std::is_void_v<T> && std::is_same_v<T, U>) constexpr const U&&
        operator*() const&& {
            return std::move(this->_value);
        }

        template <typename U = T>
        requires(!std::is_void_v<T> && std::is_same_v<T, U>) constexpr U&&
        operator*() && {
            return std::move(this->_value);
        }

        constexpr explicit operator bool() const noexcept {
            return this->_has_value;
        }

        constexpr bool has_value() const noexcept { return this->_has_value; }

        template <typename U = T>
        requires(!std::is_void_v<T> &&
                 std::is_same_v<T, U>) constexpr const U& value() const& {
#if KZ_EXCEPTIONS
            if (!*this)
                throw bad_expected_access(error());
#endif
            return this->_value;
        }

        template <typename U = T>
        requires(
            !std::is_void_v<T> && std::is_same_v<T, U>) constexpr U& value() & {
#if KZ_EXCEPTIONS
            if (!*this)
                throw bad_expected_access(error());
#endif
            return this->_value;
        }

        template <typename U = T>
        requires(!std::is_void_v<T> &&
                 std::is_same_v<T, U>) constexpr const U&& value() const&& {
#if KZ_EXCEPTIONS
            if (!*this)
                throw bad_expected_access(error());
#endif
            return std::move(this->_value);
        }

        template <typename U = T>
        requires(!std::is_void_v<T> &&
                 std::is_same_v<T, U>) constexpr U&& value() && {
#if KZ_EXCEPTIONS
            if (!*this)
                throw bad_expected_access(error());
#endif
            return std::move(this->_value);
        }

        constexpr const E& error() const& { return this->_error.value(); }

        constexpr E& error() & { return this->_error.value(); }

        constexpr const E&& error() const&& {
            return std::move(this->_error.value());
        }

        constexpr E&& error() && { return std::move(this->_error.value()); }

        template <typename U>
        requires(std::is_copy_constructible_v<T>&&
                std::is_convertible_v<U&&, T>) constexpr T
            value_or(U&& value) const& {
            return bool(*this) ? **this
                               : static_cast<T>(std::forward<U>(value));
        }

        template <typename U>
        requires(std::is_move_constructible_v<T>&&
                std::is_convertible_v<U&&, T>) constexpr T
            value_or(U&& value) && {
            return bool(*this) ? std::move(**this)
                               : static_cast<T>(std::forward<U>(value));
        }

        // �.�.4.6 Expected Equality operators [expected.equality_op]
        template <typename T1, typename E1, typename T2, typename E2>
        friend constexpr bool operator==(
            const expected<T1, E1>& x, const expected<T2, E2>& y);
        template <typename T1, typename E1, typename T2, typename E2>
        friend constexpr bool operator!=(
            const expected<T1, E1>& x, const expected<T2, E2>& y);

        // �.�.4.7 Comparison with T [expected.comparison_T]
        template <typename T1, typename E1, typename T2>
        friend constexpr bool operator==(const expected<T1, E1>&, const T2&);
        template <typename T1, typename E1, typename T2>
        friend constexpr bool operator==(const T2&, const expected<T1, E1>&);
        template <typename T1, typename E1, typename T2>
        friend constexpr bool operator!=(const expected<T1, E1>&, const T2&);
        template <typename T1, typename E1, typename T2>
        friend constexpr bool operator!=(const T2&, const expected<T1, E1>&);

        // �.�.4.8 Comparison with unexpected<E>
        // [expected.comparison_unexpected_E]
        template <typename T1, typename E1, typename E2>
        friend constexpr bool operator==(
            const expected<T1, E1>&, const unexpected<E2>&);
        template <typename T1, typename E1, typename E2>
        friend constexpr bool operator==(
            const unexpected<E2>&, const expected<T1, E1>&);
        template <typename T1, typename E1, typename E2>
        friend constexpr bool operator!=(
            const expected<T1, E1>&, const unexpected<E2>&);
        template <typename T1, typename E1, typename E2>
        friend constexpr bool operator!=(
            const unexpected<E2>&, const expected<T1, E1>&);

        // �.�.4.9 Specialized algorithms [expected.specalg]
        template <typename T1, typename E1>
        friend void swap(expected<T1, E1>& x, expected<T1, E1>& y) noexcept(
            noexcept(x.swap(y)));
    };

    template <typename T1, typename E1, typename T2, typename E2>
    constexpr bool operator==(
        const expected<T1, E1>& x, const expected<T2, E2>& y) {
        if (bool(x))
            return bool(y) && *x == *y;
        else
            return !bool(y) && x.error() == y.error();
    }

    template <typename E1, typename T2, typename E2>
    constexpr bool operator==(
        const expected<void, E1>& x, const expected<T2, E2>& y) {
        if (bool(x))
            return false;
        else
            return !bool(y) && x.error() == y.error();
    }

    template <typename T1, typename E1, typename E2>
    constexpr bool operator==(
        const expected<T1, E1>& x, const expected<void, E2>& y) {
        return y == x;
    }

    template <typename E1, typename E2>
    constexpr bool operator==(
        const expected<void, E1>& x, const expected<void, E2>& y) {
        if (bool(x))
            return bool(y);
        else

            return !bool(y) && x.error() == y.error();
    }

    template <typename T1, typename E1, typename T2, typename E2>
    constexpr bool operator!=(
        const expected<T1, E1>& x, const expected<T2, E2>& y) {
        return !(x == y);
    }

    template <typename T1, typename E1, typename T2>
    constexpr bool operator==(const expected<T1, E1>& x, const T2& y) {
        return bool(x) ? *x == y : false;
    }

    template <typename T1, typename E1, typename T2>
    constexpr bool operator==(const T2& x, const expected<T1, E1>& y) {
        return y == x;
    }

    template <typename T1, typename E1, typename T2>
    constexpr bool operator!=(const expected<T1, E1>& x, const T2& y) {
        return !(x == y);
    }

    template <typename T1, typename E1, typename T2>
    constexpr bool operator!=(const T2& x, const expected<T1, E1>& y) {
        return !(x == y);
    }

    template <typename T1, typename E1, typename E2>
    constexpr bool operator==(
        const expected<T1, E1>& x, const unexpected<E2>& y) {
        return bool(x) ? false : unexpected(x.error()) == y;
    }

    template <typename T1, typename E1, typename E2>
    constexpr bool operator==(
        const unexpected<E2>& x, const expected<T1, E1>& y) {
        return y == x;
    }

    template <typename T1, typename E1, typename E2>
    constexpr bool operator!=(
        const expected<T1, E1>& x, const unexpected<E2>& y) {
        return !(x == y);
    }

    template <typename T1, typename E1, typename E2>
    constexpr bool operator!=(
        const unexpected<E2>& x, const expected<T1, E1>& y) {
        return !(x == y);
    }

    template <typename T1, typename E1>
    requires((std::is_void_v<std::remove_cvref_t<T1>> ||
              std::is_move_constructible_v<T1>)&&std::is_swappable_v<T1>&&
            std::is_move_constructible_v<E1>&&
                std::is_swappable_v<E1>) void swap(expected<T1, E1>& x,
        expected<T1, E1>& y) noexcept(noexcept(x.swap(y))) {
        x.swap(y);
    }

} // namespace kz
