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

#include <metal/expected_bits/exception.hpp>
#include <metal/expected_bits/expected_storage.hpp>

namespace mtl {

    template <typename T, typename E>
    class expected : private detail::expected_storage<T, E> {
        using base = detail::expected_storage<T, E>;

    public:
        // TODO: assert that E can be copy-constructed? If not, error() won't
        // compile and it's not obvious why

        using value_type = T;
        using error_type = E;
        using unexpected_type = unexpected<E>;

        template <typename U> using rebind = expected<U, error_type>;

        // �.�.4.1, constructors
        constexpr expected() = default;
        constexpr expected(const expected&) = default;
        constexpr expected(expected&&) = default;

        template <
            typename U, typename G,
            std::enable_if_t<
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
                !std::is_constructible_v<unexpected<E>,
                                         const expected<U, G>&> &&
                !std::is_constructible_v<unexpected<E>,
                                         const expected<U, G>&&> &&
                !std::is_convertible_v<expected<U, G>&, unexpected<E>> &&
                !std::is_convertible_v<expected<U, G>&&, unexpected<E>> &&
                !std::is_convertible_v<const expected<U, G>&, unexpected<E>> &&
                !std::is_convertible_v<const expected<U, G>&&,
                                       unexpected<E>>>* = nullptr>
        explicit(
            !std::is_convertible_v<const U&, T> ||
            !std::is_convertible_v<
                const G&, E>) constexpr expected(const expected<U, G>& other)
            : base(other) {}

        template <
            typename G,
            std::enable_if_t<
                std::is_constructible_v<E, const G&> &&
                !std::is_constructible_v<unexpected<E>, expected<void, G>&> &&
                !std::is_constructible_v<unexpected<E>, expected<void, G>&&> &&
                !std::is_constructible_v<unexpected<E>,
                                         const expected<void, G>&> &&
                !std::is_constructible_v<unexpected<E>,
                                         const expected<void, G>&&> &&
                !std::is_convertible_v<expected<void, G>&, unexpected<E>> &&
                !std::is_convertible_v<expected<void, G>&&, unexpected<E>> &&
                !std::is_convertible_v<const expected<void, G>&,
                                       unexpected<E>> &&
                !std::is_convertible_v<const expected<void, G>&&,
                                       unexpected<E>>>* = nullptr>
        explicit(!std::is_convertible_v<const G&, E>) constexpr expected(
            const expected<void, G>& other)
            : base(other) {}

        template <
            typename U, typename G,
            std::enable_if_t<
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
                !std::is_constructible_v<unexpected<E>,
                                         const expected<U, G>&> &&
                !std::is_constructible_v<unexpected<E>,
                                         const expected<U, G>&&> &&
                !std::is_convertible_v<expected<U, G>&, unexpected<E>> &&
                !std::is_convertible_v<expected<U, G>&&, unexpected<E>> &&
                !std::is_convertible_v<const expected<U, G>&, unexpected<E>> &&
                !std::is_convertible_v<const expected<U, G>&&,
                                       unexpected<E>>>* = nullptr>
        explicit(!std::is_convertible_v<U&&, T> ||
                 !std::is_convertible_v<
                     G&&, E>) constexpr expected(expected<U, G>&& other)
            : base(std::move(other)) {}

        template <
            typename G,
            std::enable_if_t<
                std::is_constructible_v<E, G&&> &&
                !std::is_constructible_v<unexpected<E>, expected<void, G>&> &&
                !std::is_constructible_v<unexpected<E>, expected<void, G>&&> &&
                !std::is_constructible_v<unexpected<E>,
                                         const expected<void, G>&> &&
                !std::is_constructible_v<unexpected<E>,
                                         const expected<void, G>&&> &&
                !std::is_convertible_v<expected<void, G>&, unexpected<E>> &&
                !std::is_convertible_v<expected<void, G>&&, unexpected<E>> &&
                !std::is_convertible_v<const expected<void, G>&,
                                       unexpected<E>> &&
                !std::is_convertible_v<const expected<void, G>&&,
                                       unexpected<E>>>* = nullptr>
        explicit(!std::is_convertible_v<G&&, E>) constexpr expected(
            expected<void, G>&& other)
            : base(std::move(other)) {}

        template <
            typename U = T,
            std::enable_if_t<
                !std::is_void_v<std::remove_cvref_t<U>> &&
                std::is_constructible_v<T, U&&> &&
                !std::is_same_v<std::remove_cvref_t<U>, std::in_place_t> &&
                !std::is_same_v<expected<T, E>, std::remove_cvref_t<U>> &&
                !std::is_same_v<unexpected<E>, std::remove_cvref_t<U>>>* =
                nullptr>
        explicit(!std::is_convertible_v<U&&, T>) constexpr expected(U&& v)
            : base(std::in_place, std::forward<U>(v)) {}

        template <
            typename G = E,
            std::enable_if_t<std::is_constructible_v<E, const G&>>* = nullptr>
        explicit(!std::is_convertible_v<const G&, E>) constexpr expected(
            const unexpected<G>& e)
            : base(unexpect, e) {}

        template <typename G = E,
                  std::enable_if_t<std::is_constructible_v<E, G&&>>* = nullptr>
        explicit(!std::is_convertible_v<G&&, E>) constexpr expected(
            unexpected<G>&& e)
            : base(unexpect, std::move(e)) {}

        template <
            typename... Args,
            std::enable_if_t<std::is_void_v<std::remove_cvref_t<T>> ||
                             std::is_constructible_v<T, Args...>>* = nullptr>
        constexpr explicit expected(std::in_place_t, Args&&... args)
            : base(std::in_place, std::forward<Args>(args)...) {}

#if defined(_MSC_VER)
        // I don't know why MSVC doesn't accept the constraints below, but
        // it doesn't. Removing them seems to make it happy with the unit
        // tests.
        template <typename U, typename... Args>
#else
        template <typename U, typename... Args,
                  std::enable_if_t<
                      !std::is_void_v<std::remove_cvref_t<T>> &&
                      std::is_constructible_v<T, std::initializer_list<U>&,
                                              Args...>>* = nullptr>
#endif
        constexpr explicit expected(std::in_place_t,
                                    std::initializer_list<U> list,
                                    Args&&... args)
            : base(std::in_place, list, std::forward<Args>(args)...) {
        }

        template <
            typename... Args,
            std::enable_if_t<std::is_constructible_v<E, Args...>>* = nullptr>
        constexpr explicit expected(unexpect_t, Args&&... args)
            : base(unexpect, std::in_place, std::forward<Args>(args)...) {}

        template <typename U, typename... Args,
                  std::enable_if_t<std::is_constructible_v<
                      E, std::initializer_list<U>&, Args...>>* = nullptr>
        constexpr explicit expected(unexpect_t, std::initializer_list<U> list,
                                    Args&&... args)
            : base(unexpect, std::in_place, list, std::forward<Args>(args)...) {
        }

        // �.�.4.2, destructor
        ~expected() = default;

        // �.�.4.3, assignment
        expected& operator=(const expected&) =
            default; // TODO: noexcept clause, not explitcly specified in R10

        expected& operator=(expected&&) = default;
        /*TODO noexcept(
            std::is_nothrow_move_assignable_v<T>&&
                std::is_nothrow_move_constructible_v<T>);*/

        template <typename U = T,
                  std::enable_if_t<
                      !std::is_void_v<U> &&
                      !std::is_same_v<expected<T, E>, std::remove_cvref_t<U>> &&
                      !std::conjunction_v<std::is_scalar<T>,
                                          std::is_same<T, std::decay_t<U>>> &&
                      std::is_constructible_v<T, U>
                      /*TODO: && std::is_assignable_v<T&, U>*/
                      /*TODO: && std::is_nothrow_move_constructible_v<E>*/>* =
                      nullptr>
        expected& operator=(U&& other) {
            base::operator=(std::forward<U>(other));
            return *this;
        }

        template <typename G = E
                  // TODO:,
                  // std::enable_if_t<std::is_nothrow_copy_constructible_v<E>
                  // && std::is_copy_assignable_v<E>>* =nullptr
                  >
        expected& operator=(const unexpected<G>& other) {
            base::operator=(other);
            return *this;
        }

        template <typename G = E
                  // TODO:,
                  // std::enable_if_t<std::is_nothrow_move_constructible_v<E>
                  // && std::is_move_assignable_v<E>>* =nullptr
                  >
        expected& operator=(unexpected<G>&& other) {
            base::operator=(std::forward<unexpected<G>>(other));
            return *this;
        }

        // �.�.4.4, modifiers

        template <typename U = T, typename... Args,
                  std::enable_if_t<!std::is_void_v<std::remove_cvref_t<U>>>* =
                      nullptr>
        U& emplace(Args&&... args) {
            return base::emplace(std::forward<Args>(args)...);
        }

        template <typename U = T, typename V, typename... Args,
                  std::enable_if_t<!std::is_void_v<std::remove_cvref_t<U>>>* =
                      nullptr>
        U& emplace(std::initializer_list<V> list, Args&&... args) {
            return base::emplace(list, std::forward<Args>(args)...);
        }

        // �.�.4.5, swap
        template <typename U = T
                  /* TODO
                      typename G = E,
                            std::enable_if_t<
                                std::is_swappable_v<T> && // TODO: Lvalues of
                     type T are
                                                          // Swappable; and
                                    std::is_swappable_v<E> && // TODO: Lvalues
                     of type E
                                                              // are Swappable;
                     and std::is_void_v<std::remove_cvref_t<U>> ||
                                (std::is_move_constructible_v<U> ||
                                 std::is_move_constructible_v<G>)>* = nullptr*/
                  >
        void swap(expected& other) noexcept(
            std::is_nothrow_move_constructible_v<T>&&
                std::is_nothrow_swappable_v<T>&&
                    std::is_nothrow_move_constructible_v<E>&&
                        std::is_nothrow_swappable_v<E>) {
            base::swap(other);
        }

        // �.�.4.6, observers
        template <class U = T,
                  std::enable_if_t<!std::is_void_v<std::remove_cvref_t<U>>>* =
                      nullptr>
        constexpr const U* operator->() const {
            return std::addressof(base::_value);
        }

        template <class U = T,
                  std::enable_if_t<!std::is_void_v<std::remove_cvref_t<U>>>* =
                      nullptr>
        constexpr U* operator->() {
            return std::addressof(base::_value);
        }

        template <class U = T,
                  std::enable_if_t<!std::is_void_v<std::remove_cvref_t<U>>>* =
                      nullptr>
        constexpr const U& operator*() const& {
            return base::_value;
        }

        template <class U = T,
                  std::enable_if_t<!std::is_void_v<std::remove_cvref_t<U>>>* =
                      nullptr>
        constexpr U& operator*() & {
            return base::_value;
        }

        template <class U = T,
                  std::enable_if_t<!std::is_void_v<std::remove_cvref_t<U>>>* =
                      nullptr>
        constexpr const U&& operator*() const&& {
            return std::move(base::_value);
        }

        template <class U = T,
                  std::enable_if_t<!std::is_void_v<std::remove_cvref_t<U>>>* =
                      nullptr>
        constexpr U&& operator*() && {
            return std::move(base::_value);
        }

        constexpr explicit operator bool() const noexcept {
            return base::_has_value;
        }

        constexpr bool has_value() const noexcept { return base::_has_value; }

        template <class U = T,
                  std::enable_if_t<!std::is_void_v<std::remove_cvref_t<U>>>* =
                      nullptr>
        constexpr const U& value() const& {
#if MTL_EXCEPTIONS
            if (!*this)
                throw bad_expected_access(error());
#endif
            return base::_value;
        }

        template <class U = T,
                  std::enable_if_t<!std::is_void_v<std::remove_cvref_t<U>>>* =
                      nullptr>
        constexpr U& value() & {
#if MTL_EXCEPTIONS
            if (!*this)
                throw bad_expected_access(error());
#endif
            return base::_value;
        }

        template <class U = T,
                  std::enable_if_t<!std::is_void_v<std::remove_cvref_t<U>>>* =
                      nullptr>
        constexpr const U&& value() const&& {
#if MTL_EXCEPTIONS
            if (!*this)
                throw bad_expected_access(error());
#endif
            return std::move(base::_value);
        }

        template <class U = T,
                  std::enable_if_t<!std::is_void_v<std::remove_cvref_t<U>>>* =
                      nullptr>
        constexpr U&& value() && {
#if MTL_EXCEPTIONS
            if (!*this)
                throw bad_expected_access(error());
#endif
            return std::move(base::_value);
        }

        constexpr const E& error() const& { return base::_error.value(); }

        constexpr E& error() & { return base::_error.value(); }

        constexpr const E&& error() const&& {
            return std::move(base::_error.value());
        }

        constexpr E&& error() && { return std::move(base::_error.value()); }

        template <typename U> constexpr T value_or(U&& value) const& {
            static_assert(
                std::is_copy_constructible_v<T> &&
                    std::is_convertible_v<U&&, T>,
                "T must be copy-constructible and convertible from U&&");

            return bool(*this) ? **this
                               : static_cast<T>(std::forward<U>(value));
        }

        template <typename U> constexpr T value_or(U&& value) && {
            static_assert(
                std::is_move_constructible_v<T> &&
                    std::is_convertible_v<U&&, T>,
                "T must be move-constructible and convertible from U&&");

            return bool(*this) ? std::move(**this)
                               : static_cast<T>(std::forward<U>(value));
        }

        // �.�.4.7, Expected equality operators
        template <typename T1, typename E1, typename T2, typename E2>
        friend constexpr bool operator==(const expected<T1, E1>& x,
                                         const expected<T2, E2>& y);
        template <typename T1, typename E1, typename T2, typename E2>
        friend constexpr bool operator!=(const expected<T1, E1>& x,
                                         const expected<T2, E2>& y);

        // �.�.4.8, Comparison with T
        template <typename T1, typename E1, typename T2>
        friend constexpr bool operator==(const expected<T1, E1>&, const T2&);
        template <typename T1, typename E1, typename T2>
        friend constexpr bool operator==(const T2&, const expected<T1, E1>&);
        template <typename T1, typename E1, typename T2>
        friend constexpr bool operator!=(const expected<T1, E1>&, const T2&);
        template <typename T1, typename E1, typename T2>
        friend constexpr bool operator!=(const T2&, const expected<T1, E1>&);

        // �.�.4.9, Comparison with unexpected<E>
        template <typename T1, typename E1, typename E2>
        friend constexpr bool operator==(const expected<T1, E1>&,
                                         const unexpected<E2>&);
        template <typename T1, typename E1, typename E2>
        friend constexpr bool operator==(const unexpected<E2>&,
                                         const expected<T1, E1>&);
        template <typename T1, typename E1, typename E2>
        friend constexpr bool operator!=(const expected<T1, E1>&,
                                         const unexpected<E2>&);
        template <typename T1, typename E1, typename E2>
        friend constexpr bool operator!=(const unexpected<E2>&,
                                         const expected<T1, E1>&);

        // �.�.4.10, Specialized algorithms
        template <typename T1, typename E1>
        friend void swap(expected<T1, E1>& x,
                         expected<T1, E1>& y) noexcept(noexcept(x.swap(y)));
    };

    template <typename T1, typename E1, typename T2, typename E2>
    constexpr bool operator==(const expected<T1, E1>& x,
                              const expected<T2, E2>& y) {
        if (bool(x))
            return bool(y) && *x == *y;
        else
            return !bool(y) && x.error() == y.error();
    }

    template <typename E1, typename T2, typename E2>
    constexpr bool operator==(const expected<void, E1>& x,
                              const expected<T2, E2>& y) {
        if (bool(x))
            return false;
        else
            return !bool(y) && x.error() == y.error();
    }

    template <typename T1, typename E1, typename E2>
    constexpr bool operator==(const expected<T1, E1>& x,
                              const expected<void, E2>& y) {
        return y == x;
    }

    template <typename E1, typename E2>
    constexpr bool operator==(const expected<void, E1>& x,
                              const expected<void, E2>& y) {
        if (bool(x))
            return bool(y);
        else

            return !bool(y) && x.error() == y.error();
    }

    template <typename T1, typename E1, typename T2, typename E2>
    constexpr bool operator!=(const expected<T1, E1>& x,
                              const expected<T2, E2>& y) {
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
    constexpr bool operator==(const expected<T1, E1>& x,
                              const unexpected<E2>& y) {
        return bool(x) ? false : unexpected(x.error()) == y;
    }

    template <typename T1, typename E1, typename E2>
    constexpr bool operator==(const unexpected<E2>& x,
                              const expected<T1, E1>& y) {
        return y == x;
    }

    template <typename T1, typename E1, typename E2>
    constexpr bool operator!=(const expected<T1, E1>& x,
                              const unexpected<E2>& y) {
        return !(x == y);
    }

    template <typename T1, typename E1, typename E2>
    constexpr bool operator!=(const unexpected<E2>& x,
                              const expected<T1, E1>& y) {
        return !(x == y);
    }

    template <typename T1, typename E1,
              std::enable_if_t<
                  (std::is_void_v<std::remove_cvref_t<T1>> ||
                   std::is_move_constructible_v<T1>)&&std::is_swappable_v<T1> &&
                  std::is_move_constructible_v<E1> &&
                  std::is_swappable_v<E1>>* = nullptr>
    void swap(expected<T1, E1>& x,
              expected<T1, E1>& y) noexcept(noexcept(x.swap(y))) {
        x.swap(y);
    }

} // namespace mtl
