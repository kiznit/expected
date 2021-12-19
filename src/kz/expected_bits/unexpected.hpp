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

namespace kz {

    using std::in_place_t;
    using std::initializer_list;

    // �.�.5.2 Class template unexpected [expected.unexpected.object]
    template <typename E>
    requires(!std::is_void_v<E>) class unexpected {
    public:
        // �.�.5.2.1 Constructors [expected.unexpected.ctor]
        constexpr unexpected(const unexpected&) = default;
        constexpr unexpected(unexpected&&) = default;

        template <typename Err>
        requires(std::is_constructible_v<E, Err> &&
                 !std::is_same_v<std::remove_cvref<Err>, in_place_t> &&
                 !std::is_same_v<std::remove_cvref<Err>,
                     unexpected>) constexpr explicit unexpected(Err&& e)
            : _value(std::forward<Err>(e)) {}

        template <typename... Args>
        requires(std::is_constructible_v<E,
            Args...>) constexpr explicit unexpected(in_place_t, Args&&... args)
            : _value(std::forward<Args>(args)...) {}

        template <typename U, typename... Args>
        requires(std::is_constructible_v<E, initializer_list<U>&,
            Args...>) constexpr explicit unexpected(in_place_t,
            initializer_list<U> il, Args&&... args)
            : _value(il, std::forward<Args>(args)...) {}

        template <typename Err>
        requires(
            std::is_constructible_v<E, const Err&> &&
            !std::is_constructible_v<E, unexpected<Err>&> &&
            !std::is_constructible_v<E, const unexpected<Err>&> &&
            !std::is_constructible_v<E, const unexpected<Err>> &&
            !std::is_convertible_v<unexpected<Err>&, E> &&
            !std::is_convertible_v<unexpected<Err>, E> &&
            !std::is_convertible_v<const unexpected<Err>&, E> &&
            !std::is_convertible_v<const unexpected<Err>,
                E>) constexpr explicit(!std::is_convertible_v<const Err&, E>)
            unexpected(const unexpected<Err>& e)
            : _value(e.value()) {}

        template <typename Err>
        requires(std::is_constructible_v<E, Err> &&
                 !std::is_constructible_v<E, unexpected<Err>&> &&
                 !std::is_constructible_v<E, unexpected<Err>> &&
                 !std::is_constructible_v<E, const unexpected<Err>&> &&
                 !std::is_constructible_v<E, const unexpected<Err>> &&
                 !std::is_convertible_v<unexpected<Err>&, E> &&
                 !std::is_convertible_v<unexpected<Err>, E> &&
                 !std::is_convertible_v<const unexpected<Err>&, E> &&
                 !std::is_convertible_v<const unexpected<Err>,
                     E>) constexpr explicit(!std::is_convertible_v<Err, E>)
            unexpected(unexpected<Err>&& e)
            : _value(std::move(e.value())) {}

        // �.�.5.2.2 Assignment [expected.unexpected.assign]
        constexpr unexpected& operator=(const unexpected&) = default;
        constexpr unexpected& operator=(unexpected&&) = default;

        template <class Err = E>
        requires std::is_assignable_v<E, const Err&>
        constexpr unexpected& operator=(const unexpected<Err>& e) {
            _value = e.value();
            return *this;
        }

        template <class Err = E>
        requires std::is_assignable_v<E, const Err>
        constexpr unexpected& operator=(unexpected<Err>&& e) {
            _value = std::move(e.value());
            return *this;
        }

        // �.�.5.2.3 Observers [expected.unexpected.observe]
        constexpr const E& value() const& noexcept { return _value; }
        constexpr E& value() & noexcept { return _value; }
        constexpr const E&& value() const&& noexcept {
            return std::move(_value);
        }
        constexpr E&& value() && noexcept { return std::move(_value); }

        // �.�.5.2.4 Swap [expected.unexpected.ctor]
        template <typename = void>
        requires std::is_swappable_v<E>
        void swap(unexpected& other) noexcept(std::is_nothrow_swappable_v<E>) {
            using std::swap;
            swap(_value, other._value);
        }

        // �.�.5.2.5 Equality operators [expected.unexpected.equality_op]
        template <typename E1, typename E2>
        friend constexpr bool operator==(
            const unexpected<E1>& x, const unexpected<E2>& y);

        template <typename E1, typename E2>
        friend constexpr bool operator!=(
            const unexpected<E1>& x, const unexpected<E2>& y);

    private:
        E _value;
    };

    // �.�.5.2.5 Equality operators [expected.unexpected.equality_op]
    template <typename E1, typename E2>
    constexpr bool operator==(
        const unexpected<E1>& x, const unexpected<E2>& y) {
        return x.value() == y.value();
    }

    template <typename E1, typename E2>
    constexpr bool operator!=(
        const unexpected<E1>& x, const unexpected<E2>& y) {
        return x.value() != y.value();
    }

    // �.�.5.2.5 Specialized algorithms [expected.unexpected.specalg]
    template <typename E>
    requires std::is_swappable_v<E>
    void swap(unexpected<E>& x, unexpected<E>& y) noexcept(
        noexcept(x.swap(y))) {
        x.swap(y);
    }

    template <typename E>
    unexpected(E) -> unexpected<E>;

} // namespace kz
