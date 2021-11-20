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

#include <initializer_list>
#include <type_traits>
#include <utility>

namespace mtl {

    template <typename E> class unexpected {
    public:
        static_assert(!std::is_same_v<E, void>, "E must not be void");

        constexpr unexpected(const unexpected&) = default;
        constexpr unexpected(unexpected&&) = default;

        template <
            typename... Args,
            std::enable_if_t<std::is_constructible_v<E, Args...>>* = nullptr>
        constexpr explicit unexpected(std::in_place_t, Args&&... args)
            : _value(std::forward<Args>(args)...) {}

        template <typename U, typename... Args,
                  std::enable_if_t<std::is_constructible_v<
                      E, std::initializer_list<U>&, Args...>>* = nullptr>
        constexpr explicit unexpected(std::in_place_t,
                                      std::initializer_list<U> list,
                                      Args&&... args)
            : _value(list, std::forward<Args>(args)...) {}

        template <
            typename U,
            std::enable_if_t<
                std::is_constructible_v<E, U&&> &&
                !std::is_same_v<std::remove_cvref_t<U>, std::in_place_t> &&
                !std::is_same_v<std::remove_cvref_t<U>, unexpected>>* = nullptr>
        constexpr explicit unexpected(U&& e) : _value(std::forward<U>(e)) {}

        template <
            typename U,
            std::enable_if_t<
                std::is_constructible_v<E, const U&> &&
                !std::is_constructible_v<E, unexpected<U>&> &&
                !std::is_constructible_v<E, const unexpected<U>&> &&
                !std::is_constructible_v<E, const unexpected<U>> &&
                !std::is_convertible_v<unexpected<U>&, E> &&
                !std::is_convertible_v<unexpected<U>, E> &&
                !std::is_convertible_v<const unexpected<U>&, E> &&
                !std::is_convertible_v<const unexpected<U>, E>>* = nullptr>
        constexpr explicit(!std::is_convertible_v<const U&, E>)
            unexpected(const unexpected<U>& e)
            : _value(e.value()) {}

        template <
            typename U,
            std::enable_if_t<
                std::is_constructible_v<E, U> &&
                !std::is_constructible_v<E, unexpected<U>&> &&
                !std::is_constructible_v<E, unexpected<U>> &&
                !std::is_constructible_v<E, const unexpected<U>&> &&
                !std::is_constructible_v<E, const unexpected<U>> &&
                !std::is_convertible_v<unexpected<U>&, E> &&
                !std::is_convertible_v<unexpected<U>, E> &&
                !std::is_convertible_v<const unexpected<U>&, E> &&
                !std::is_convertible_v<const unexpected<U>, E>>* = nullptr>
        constexpr explicit(!std::is_convertible_v<U, E>)
            unexpected(unexpected<U>&& e)
            : _value(std::move(e.value())) {}

        constexpr unexpected& operator=(const unexpected&) = default;
        constexpr unexpected& operator=(unexpected&&) = default;

        template <
            typename U = E,
            std::enable_if_t<std::is_assignable_v<E, const U&>>* = nullptr>
        constexpr unexpected& operator=(const unexpected<U>& other) {
            _value = other.value();
            return *this;
        }

        template <typename U = E,
                  std::enable_if_t<std::is_assignable_v<E, U>>* = nullptr>
        constexpr unexpected& operator=(unexpected<U>&& other) {
            _value = std::move(other.value());
            return *this;
        }

        constexpr const E& value() const& noexcept { return _value; }
        constexpr E& value() & noexcept { return _value; }
        constexpr const E&& value() const&& noexcept {
            return std::move(_value);
        }
        constexpr E&& value() && noexcept { return std::move(_value); }

        void swap(unexpected& other,
                  std::enable_if_t<std::is_swappable_v<E>>* =
                      nullptr) noexcept(std::is_nothrow_swappable_v<E>) {
            using std::swap;
            swap(_value, other._value);
        }

        template <typename E1, typename E2>
        friend constexpr bool operator==(const unexpected<E1>& x,
                                         const unexpected<E2>& y);

        template <typename E1, typename E2>
        friend constexpr bool operator!=(const unexpected<E1>& x,
                                         const unexpected<E2>& y);

    private:
        E _value;
    };

    template <typename E1, typename E2>
    constexpr bool operator==(const unexpected<E1>& x,
                              const unexpected<E2>& y) {
        return x._value == y._value;
    }

    template <typename E1, typename E2>
    constexpr bool operator!=(const unexpected<E1>& x,
                              const unexpected<E2>& y) {
        return !(x == y);
    }

    template <typename E, std::enable_if_t<std::is_swappable_v<E>>* = nullptr>
    void swap(unexpected<E>& x,
              unexpected<E>& y) noexcept(noexcept(x.swap(y))) {
        x.swap(y);
    }

    template <typename E> unexpected(E) -> unexpected<E>;

} // namespace mtl
