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

#include <expected>
#include <catch2/catch.hpp>
#include "old.value.hpp"

enum class Error { FileNotFound, IOError, FlyingSquirrels };

TEST_CASE("expected constructors", "[expected]") {

    SECTION("default") {
        kz::expected<DefaultInt, Error> a;
        REQUIRE(a.has_value());
        REQUIRE(*a == DefaultInt::DefaultValue);

        kz::expected<void, Error> b;
        REQUIRE(b.has_value());
    }

    SECTION("from value") {
        kz::expected<IntValue, Error> a{123};
        REQUIRE(a.has_value());
        REQUIRE(*a == 123);
    }

    SECTION("from complex value") {
        const IntValue a{100};
        IntMoveableValue b{200};
        kz::expected<ComplexThing, Error> c{std::in_place, a, std::move(b)};
        REQUIRE(c->a == 100);
        REQUIRE(c->b == 200);
        REQUIRE(a == 100);
        REQUIRE(b == 0);
    }

    SECTION("from error") {
        kz::unexpected error{Error::IOError};
        kz::expected<IntValue, Error> a{error};
        REQUIRE(!a.has_value());
        REQUIRE(a.error() == Error::IOError);

        kz::unexpected error2{Error::FlyingSquirrels};
        kz::expected<void, Error> b{error2};
        REQUIRE(!b.has_value());
        REQUIRE(b.error() == Error::FlyingSquirrels);
    }

    SECTION("from complex error") {

        kz::unexpected<ComplexThing> error{
            ComplexThing{IntValue{100}, IntMoveableValue{200}}};
        kz::expected<int, ComplexThing> c{std::move(error)};
        REQUIRE(c.error().a == 100);
        REQUIRE(c.error().b == 200);
        REQUIRE(error.value().a == 100);
        REQUIRE(error.value().b == 0);
    }

    SECTION("in-place with value") {
        kz::expected<IntValue, Error> a{std::in_place, 123};
        REQUIRE(*a == 123);
    }

    SECTION("in-place with multiple parameters") {
        const IntValue a{100};
        IntMoveableValue b{200};
        kz::expected<ComplexThing, Error> c{std::in_place, a, std::move(b)};
        REQUIRE(c->a == 100);
        REQUIRE(c->b == 200);
        REQUIRE(a == 100);
        REQUIRE(b == 0);
    }

    SECTION("in-place with initializer list") {
        kz::expected<std::vector<int>, Error> a{std::in_place, {1, 2, 3}};
        REQUIRE(a->size() == 3);
    }

    SECTION("in-place with initializer list and extra parameters") {
        const IntValue a(100);
        IntMoveableValue b(200);
        kz::expected<ComplexThing, Error> c{
            std::in_place, {1, 2, 3}, a, std::move(b)};
        REQUIRE(c->a == 100);
        REQUIRE(c->b == 200);
        REQUIRE(c->list.size() == 3);
        REQUIRE(a == 100);
        REQUIRE(b == 0);
    }

    SECTION("unexpect_t with value") {
        kz::expected<int, IntValue> a{kz::unexpect, 123};

        REQUIRE(!a.has_value());
        REQUIRE(a.error() == 123);
    }

    SECTION("unexpect_t with initializer list") {
        kz::expected<int, std::vector<int>> a{kz::unexpect, {1, 2, 3}};
        REQUIRE(a.error().size() == 3);
    }

    SECTION("unexpect_t with multiple parameters") {
        const IntValue a{100};
        IntMoveableValue b{200};
        kz::expected<int, ComplexThing> c{kz::unexpect, a, std::move(b)};
        REQUIRE(c.error().a == 100);
        REQUIRE(c.error().b == 200);
        REQUIRE(a == 100);
        REQUIRE(b == 0);
    }

    SECTION("unexpect_t with initializer list and extra parameters") {
        const IntValue a(100);
        IntMoveableValue b(200);
        kz::expected<int, ComplexThing> c{
            kz::unexpect, {1, 2, 3}, a, std::move(b)};
        REQUIRE(c.error().a == 100);
        REQUIRE(c.error().b == 200);
        REQUIRE(c.error().list.size() == 3);
        REQUIRE(a == 100);
        REQUIRE(b == 0);
    }

    SECTION("copy - value") {
        const kz::expected<IntValue, Error> a{3};
        kz::expected<IntValue, Error> b{a};
        REQUIRE(a.has_value());
        REQUIRE(b.has_value());
        REQUIRE(*a == 3);
        REQUIRE(*b == 3);

        const kz::expected<void, Error> c;
        kz::expected<void, Error> d{c};
        REQUIRE(c.has_value());
        REQUIRE(d.has_value());
    }

    SECTION("copy - error") {
        const kz::expected<IntValue, Error> a{
            kz::unexpected(Error::FileNotFound)};
        kz::expected<IntValue, Error> b{a};
        REQUIRE(!a.has_value());
        REQUIRE(!b.has_value());
        REQUIRE(a.error() == Error::FileNotFound);
        REQUIRE(b.error() == Error::FileNotFound);

        const kz::expected<void, Error> c{kz::unexpected(Error::IOError)};
        kz::expected<void, Error> d{c};
        REQUIRE(!c.has_value());
        REQUIRE(!d.has_value());
        REQUIRE(c.error() == Error::IOError);
        REQUIRE(d.error() == Error::IOError);
    }

    SECTION("move - value") {
        kz::expected<IntMoveableValue, Error> a{4};
        const kz::expected<IntMoveableValue, Error> b{std::move(a)};
        REQUIRE(a.has_value());
        REQUIRE(b.has_value());
        REQUIRE(*a == 0);
        REQUIRE(*b == 4);

        kz::expected<void, Error> c;
        const kz::expected<void, Error> d{std::move(c)};
        REQUIRE(c.has_value());
        REQUIRE(d.has_value());
    }

    SECTION("move - error") {
        kz::expected<IntValue, IntMoveableValue> a{
            kz::unexpected<IntMoveableValue>{55}};
        const kz::expected<IntValue, IntMoveableValue> b{std::move(a)};
        REQUIRE(!a.has_value());
        REQUIRE(!b.has_value());
        REQUIRE(a.error() == 0);
        REQUIRE(b.error() == 55);

        kz::expected<void, IntMoveableValue> c{
            kz::unexpected<IntMoveableValue>{66}};
        const kz::expected<void, IntMoveableValue> d{std::move(c)};
        REQUIRE(!c.has_value());
        REQUIRE(!d.has_value());
        REQUIRE(c.error() == 0);
        REQUIRE(d.error() == 66);
    }

    SECTION("copy with conversion - value") {
        const kz::expected<IntValue, Error> a{123};
        kz::expected<LongValue, Error> b{a};
        REQUIRE(a.has_value());
        REQUIRE(b.has_value());
        REQUIRE(*a == 123);
        REQUIRE(*b == 123l);

        const kz::expected<void, Error> c;
        kz::expected<void, Error> d{c};
        REQUIRE(c.has_value());
        REQUIRE(d.has_value());
    }

    SECTION("copy with conversion - error") {
        const kz::expected<IntValue, IntValue> a{kz::unexpected(456)};
        kz::expected<LongValue, LongValue> b{a};
        REQUIRE(!a.has_value());
        REQUIRE(!b.has_value());
        REQUIRE(a.error() == 456);
        REQUIRE(b.error() == 456l);

        const kz::expected<void, IntValue> c{kz::unexpected(789)};
        kz::expected<void, LongValue> d{c};
        REQUIRE(!c.has_value());
        REQUIRE(!d.has_value());
        REQUIRE(c.error() == 789);
        REQUIRE(d.error() == 789l);
    }

    SECTION("move with conversion - value") {
        kz::expected<IntMoveableValue, IntMoveableValue> a{69};
        kz::expected<LongMoveableValue, LongMoveableValue> b{std::move(a)};
        REQUIRE(a.has_value());
        REQUIRE(b.has_value());
        REQUIRE(*a == 0);
        REQUIRE(*b == 69l);

        kz::expected<void, IntMoveableValue> c;
        kz::expected<void, LongMoveableValue> d{std::move(c)};
        REQUIRE(c.has_value());
        REQUIRE(d.has_value());
    }

    SECTION("move with conversion - error") {
        kz::expected<IntMoveableValue, IntMoveableValue> a{kz::unexpected(456)};
        kz::expected<LongMoveableValue, LongMoveableValue> b{std::move(a)};
        REQUIRE(!a.has_value());
        REQUIRE(!b.has_value());
        REQUIRE(a.error() == 0);
        REQUIRE(b.error() == 456l);

        kz::expected<void, IntMoveableValue> c{kz::unexpected(789)};
        kz::expected<void, LongMoveableValue> d{std::move(c)};
        REQUIRE(!c.has_value());
        REQUIRE(!d.has_value());
        REQUIRE(c.error() == 0);
        REQUIRE(d.error() == 789l);
    }
}

TEST_CASE("expected assignments", "[expected]") {

    SECTION("assign value to value") {
        const kz::expected<IntValue, Error> a{111};
        kz::expected<IntValue, Error> b{222};

        b = a;

        REQUIRE(a.has_value());
        REQUIRE(*a == 111);
        REQUIRE(b.has_value());
        REQUIRE(*b == 111);
    }

    SECTION("assign error to error") {
        const kz::expected<IntValue, Error> a{
            kz::unexpected(Error::FileNotFound)};
        kz::expected<IntValue, Error> b{kz::unexpected(Error::IOError)};

        b = a;

        REQUIRE(!a.has_value());
        REQUIRE(a.error() == Error::FileNotFound);
        REQUIRE(!b.has_value());
        REQUIRE(b.error() == Error::FileNotFound);
    }

    SECTION("assign_value_to_error - is_nothrow_copy_constructible_v<>") {
        using type = kz::expected<IntValue, Error>;
        static_assert(std::is_nothrow_copy_constructible_v<type::value_type>);

        const type a{111};
        type b{kz::unexpected(Error::FlyingSquirrels)};

        b = a;

        REQUIRE(a.has_value());
        REQUIRE(*a == 111);
        REQUIRE(b.has_value());
        REQUIRE(*b == 111);
    }

    SECTION("assign_value_to_error - is_nothrow_move_constructible_v<>") {
        using type = kz::expected<std::vector<int>, Error>;

        static_assert(!std::is_nothrow_copy_constructible_v<type::value_type>);
        static_assert(std::is_nothrow_move_constructible_v<type::value_type>);

        const type a(std::in_place, {1, 2, 3});
        type b{kz::unexpected(Error::FlyingSquirrels)};

        b = a;

        REQUIRE(a.has_value());
        REQUIRE(a->size() == 3);
        REQUIRE(b.has_value());
        REQUIRE(b->size() == 3);
    }

    SECTION("assign_value_to_error - neither") {
        using type = kz::expected<AssignableComplexThing, Error>;

        static_assert(!std::is_nothrow_copy_constructible_v<type::value_type>);

        static_assert(!std::is_nothrow_move_constructible_v<type::value_type>);

        const type a{std::in_place, {1, 2, 3}, 45, 69};
        type b{kz::unexpected(Error::FlyingSquirrels)};

        b = a;

        REQUIRE(a.has_value());
        REQUIRE(a->a == 45);
        REQUIRE(a->b == 69);
        REQUIRE(a->list.size() == 3);
        REQUIRE(b.has_value());
        REQUIRE(b->a == 45);
        REQUIRE(b->b == 69);
        REQUIRE(b->list.size() == 3);
    }

#if KZ_EXCEPTIONS
    SECTION("assign_value_to_error - neither + throws exception on copy") {
        using type = kz::expected<AssignableComplexThing, Error>;

        static_assert(!std::is_nothrow_copy_constructible_v<type::value_type>);

        static_assert(!std::is_nothrow_move_constructible_v<type::value_type>);

        const type a{std::in_place, {1, 2, 3}, 45, 69};
        type b{kz::unexpected(Error::FlyingSquirrels)};

        const_cast<AssignableComplexThing&>(*a).throwsOnCopy = true;
        REQUIRE_THROWS_AS(b = a, std::exception);

        REQUIRE(a.has_value());
        REQUIRE(a->a == 45);
        REQUIRE(a->b == 69);
        REQUIRE(a->list.size() == 3);
        REQUIRE(!b.has_value());
        REQUIRE(b.error() == Error::FlyingSquirrels);
    }
#endif

    SECTION("assign_error_to_value - is_nothrow_copy_constructible_v<>") {
        using type = kz::expected<IntValue, Error>;
        static_assert(std::is_nothrow_copy_constructible_v<type::error_type>);

        const type a{kz::unexpected(Error::FlyingSquirrels)};
        type b{111};

        b = a;

        REQUIRE(!a.has_value());
        REQUIRE(a.error() == Error::FlyingSquirrels);
        REQUIRE(!b.has_value());
        REQUIRE(b.error() == Error::FlyingSquirrels);
    }

    SECTION("assign_error_to_value - is_nothrow_move_constructible_v<>") {
        using type = kz::expected<IntValue, std::vector<int>>;

        static_assert(!std::is_nothrow_copy_constructible_v<type::error_type>);
        static_assert(std::is_nothrow_move_constructible_v<type::error_type>);

        const type a{kz::unexpected(std::vector<int>{1, 2, 3})};
        type b{222};

        b = a;

        REQUIRE(!a.has_value());
        REQUIRE(a.error().size() == 3);
        REQUIRE(!b.has_value());
        REQUIRE(b.error().size() == 3);
    }

    SECTION("assign_error_to_value - neither") {
        using type = kz::expected<IntValue, AssignableComplexThing>;

        static_assert(!std::is_nothrow_copy_constructible_v<type::error_type>);

        static_assert(!std::is_nothrow_move_constructible_v<type::error_type>);

        const type a{kz::unexpected(
            AssignableComplexThing{std::vector<int>{1, 2, 3}, 45, 69})};
        type b{333};

        b = a;

        REQUIRE(!a.has_value());
        REQUIRE(a.error().a == 45);
        REQUIRE(a.error().b == 69);
        REQUIRE(a.error().list.size() == 3);
        REQUIRE(!b.has_value());
        REQUIRE(b.error().a == 45);
        REQUIRE(b.error().b == 69);
        REQUIRE(b.error().list.size() == 3);
    }

#if KZ_EXCEPTIONS
    SECTION("assign_error_to_value - neither + throws exception on copy") {
        using type = kz::expected<IntValue, AssignableComplexThing>;

        static_assert(!std::is_nothrow_copy_constructible_v<type::error_type>);

        static_assert(!std::is_nothrow_move_constructible_v<type::error_type>);

        const type a{kz::unexpected(
            AssignableComplexThing{std::vector<int>{1, 2, 3}, 45, 69})};
        type b{333};

        const_cast<AssignableComplexThing&>(a.error()).throwsOnCopy = true;
        REQUIRE_THROWS_AS(b = a, std::exception);

        REQUIRE(!a.has_value());
        REQUIRE(a.error().a == 45);
        REQUIRE(a.error().b == 69);
        REQUIRE(a.error().list.size() == 3);
        REQUIRE(b.has_value());
        REQUIRE(*b == 333);
    }
#endif

    SECTION("assign value to value - void specialization") {
        const kz::expected<void, Error> a;
        kz::expected<void, Error> b;

        b = a;

        REQUIRE(a.has_value());
        REQUIRE(b.has_value());
    }

    SECTION("assign error to error - void specialization") {
        const kz::expected<void, Error> a{kz::unexpected(Error::FileNotFound)};
        kz::expected<void, Error> b{kz::unexpected(Error::IOError)};

        b = a;

        REQUIRE(!a.has_value());
        REQUIRE(a.error() == Error::FileNotFound);
        REQUIRE(!b.has_value());
        REQUIRE(b.error() == Error::FileNotFound);
    }

    SECTION("assign value to error - void specialization") {
        using type = kz::expected<void, AssignableComplexThing>;
        const type a{};
        type b{kz::unexpect, {1, 2, 3}, 45, 69};

        b = a;

        REQUIRE(a.has_value());
        REQUIRE(b.has_value());
    }

    SECTION("error to value - void specialization") {
        using type = kz::expected<void, AssignableComplexThing>;
        const type a{kz::unexpect, {1, 2, 3}, 45, 69};
        type b{};

        b = a;

        REQUIRE(!a.has_value());
        REQUIRE(a.error().a == 45);
        REQUIRE(a.error().b == 69);
        REQUIRE(a.error().list.size() == 3);
        REQUIRE(!b.has_value());
        REQUIRE(b.error().a == 45);
        REQUIRE(b.error().b == 69);
        REQUIRE(b.error().list.size() == 3);
    }
}

TEST_CASE("expected move assignments", "[expected]") {

    SECTION("move value to value") {
        kz::expected<IntMoveableValue, Error> a{111};
        kz::expected<IntMoveableValue, Error> b{222};

        b = std::move(a);

        REQUIRE(a.has_value());
        REQUIRE(*a == 0);
        REQUIRE(b.has_value());
        REQUIRE(*b == 111);
    }

    SECTION("move error to error") {
        kz::expected<IntValue, IntMoveableValue> a{kz::unexpected(14)};
        kz::expected<IntValue, IntMoveableValue> b{kz::unexpected(17)};

        b = std::move(a);

        REQUIRE(!a.has_value());
        REQUIRE(a.error() == 0);
        REQUIRE(!b.has_value());
        REQUIRE(b.error() == 14);
    }

    SECTION("move_value_to_error - is_nothrow_move_constructible_v<>") {
        using type = kz::expected<std::vector<int>, Error>;
        static_assert(std::is_nothrow_move_constructible_v<type::value_type>);

        type a{std::in_place, {1, 2, 3}};
        type b{kz::unexpected(Error::FlyingSquirrels)};

        b = std::move(a);

        REQUIRE(a.has_value());
        REQUIRE(a->empty());
        REQUIRE(b.has_value());
        REQUIRE(b->size() == 3);
    }

    SECTION("move_value_to_error - fallback") {
        using type = kz::expected<IntMoveableValue, Error>;

        static_assert(!std::is_nothrow_move_constructible_v<type::value_type>);

        type a{143};
        type b{kz::unexpected(Error::FlyingSquirrels)};

        b = std::move(a);

        REQUIRE(a.has_value());
        REQUIRE(*a == 0);
        REQUIRE(b.has_value());
        REQUIRE(*b == 143);
    }

#if KZ_EXCEPTIONS
    SECTION("move_value_to_error - fallback - throws on move") {
        using type = kz::expected<IntMoveableValue, Error>;

        static_assert(!std::is_nothrow_move_constructible_v<type::value_type>);

        type a{143};
        type b{kz::unexpected(Error::FlyingSquirrels)};

        a->throwsOnMove = true;
        REQUIRE_THROWS_AS(b = std::move(a), std::exception);

        REQUIRE(a.has_value());
        REQUIRE(*a == 143);
        REQUIRE(!b.has_value());
        REQUIRE(b.error() == Error::FlyingSquirrels);
    }
#endif

    SECTION("move_error_to_value - is_nothrow_move_constructible_v<>") {
        using type = kz::expected<IntValue, std::vector<int>>;
        static_assert(std::is_nothrow_move_constructible_v<type::error_type>);

        type a{kz::unexpect, {1, 2, 3}};
        type b{123};

        b = std::move(a);

        REQUIRE(!a.has_value());
        REQUIRE(a.error().empty());
        REQUIRE(!b.has_value());
        REQUIRE(b.error().size() == 3);
    }

    SECTION("move_error_to_value - fallback - throws on move") {
        using type = kz::expected<IntValue, IntMoveableValue>;

        static_assert(!std::is_nothrow_move_constructible_v<type::error_type>);

        type a{kz::unexpect, 143};
        type b{732};

        b = std::move(a);

        REQUIRE(!a.has_value());
        REQUIRE(*a == 0);
        REQUIRE(!b.has_value());
        REQUIRE(*b == 143);
    }

#if KZ_EXCEPTIONS
    SECTION("move_error_to_value - fallback - throws on move") {
        using type = kz::expected<IntValue, IntMoveableValue>;

        static_assert(!std::is_nothrow_move_constructible_v<type::error_type>);

        type a{kz::unexpect, 143};
        type b{732};

        a.error().throwsOnMove = true;
        REQUIRE_THROWS_AS(b = std::move(a), std::exception);

        REQUIRE(!a.has_value());
        REQUIRE(a.error() == 143);
        REQUIRE(b.has_value());
        REQUIRE(*b == 732);
    }
#endif

    SECTION("move value to value - void specialization") {
        kz::expected<void, Error> a{};
        kz::expected<void, Error> b{};

        b = std::move(a);

        REQUIRE(a.has_value());
        REQUIRE(b.has_value());
    }

    SECTION("move error to error - void specialization") {
        kz::expected<void, IntMoveableValue> a{kz::unexpected(14)};
        kz::expected<void, IntMoveableValue> b{kz::unexpected(17)};

        b = std::move(a);

        REQUIRE(!a.has_value());
        REQUIRE(a.error() == 0);
        REQUIRE(!b.has_value());
        REQUIRE(b.error() == 14);
    }

    SECTION("move_value_to_error - void specialization") {
        using type = kz::expected<void, Error>;
        type a{};
        type b{kz::unexpected(Error::FlyingSquirrels)};

        b = std::move(a);

        REQUIRE(a.has_value());
        REQUIRE(b.has_value());
    }

    SECTION("move_error_to_value - void specialization") {
        using type = kz::expected<void, std::vector<int>>;
        type a{kz::unexpect, {1, 2, 3}};
        type b{};

        b = std::move(a);

        REQUIRE(!a.has_value());
        REQUIRE(a.error().empty());
        REQUIRE(!b.has_value());
        REQUIRE(b.error().size() == 3);
    }

#if KZ_EXCEPTIONS
    SECTION("move_error_to_value - void specialization - throws on move") {
        using type = kz::expected<void, IntMoveableValue>;

        type a{kz::unexpect, 143};
        type b{};

        a.error().throwsOnMove = true;
        REQUIRE_THROWS_AS(b = std::move(a), std::exception);

        REQUIRE(!a.has_value());
        REQUIRE(a.error() == 143);
        REQUIRE(b.has_value());
    }
#endif
}

TEST_CASE("expected other assignments", "[expected]") {
    SECTION("value") {
        kz::expected<IntValue, Error> a{123};
        a = 456;
        REQUIRE(a.has_value());
        REQUIRE(*a == 456);

        kz::expected<IntValue, Error> b{kz::unexpected(Error::FlyingSquirrels)};
        b = 789;
        REQUIRE(b.has_value());
        REQUIRE(*b == 789);

        //         using type = kz::expected<NotNoThrowConstructible,
        //         IntMoveableValue>;
        //         static_assert(!std::is_nothrow_constructible_v<type::value_type,
        //                       NotNoThrowConstructible>);
        //         type c{kz::unexpected(777)};
        //         c = NotNoThrowConstructible{44};
        //         REQUIRE(c.has_value());
        //         REQUIRE(c->value == 44);

        // #if KZ_EXCEPTIONS
        //         // TODO: fix this test, we need a value_type that can throw
        //         on
        //         // construction
        //         type d{kz::unexpected(666)};
        //         d.error().throwsOnMove = true;
        //         REQUIRE_THROWS_AS(d = NotNoThrowConstructible{55},
        //         std::exception); REQUIRE(!d.has_value()); REQUIRE(d.error()
        //         == 666);
        // #endif
    }

    SECTION("unexpected - const&") {
        kz::expected<IntValue, Error> a{123};
        auto a2 = kz::unexpected(Error::FileNotFound);
        a = a2;
        REQUIRE(!a.has_value());
        REQUIRE(a.error() == Error::FileNotFound);

        kz::expected<IntValue, Error> b{kz::unexpect, Error::IOError};
        auto b2 = kz::unexpected(Error::FlyingSquirrels);
        b = b2;
        REQUIRE(!b.has_value());
        REQUIRE(b.error() == Error::FlyingSquirrels);

        kz::expected<void, Error> c{};
        auto c2 = kz::unexpected(Error::FileNotFound);
        c = c2;
        REQUIRE(!c.has_value());
        REQUIRE(c.error() == Error::FileNotFound);

        kz::expected<void, Error> d{kz::unexpect, Error::IOError};
        auto d2 = kz::unexpected(Error::FlyingSquirrels);
        d = d2;
        REQUIRE(!d.has_value());
        REQUIRE(d.error() == Error::FlyingSquirrels);
    }

    SECTION("unexpected - &&") {
        kz::expected<IntValue, IntMoveableValue> a{123};
        auto a2 = kz::unexpected(IntMoveableValue{1});
        // TODO: this appears to be optimized to use a constructor:
        // expected(unexpected<E>&&) and do "the right thing", except it's not
        // calling operator=()..., same for all 4 cases below. We might need to
        // define a custom type that is not move-constructible but is
        // move-assignable.
        a = std::move(a2);
        REQUIRE(!a.has_value());
        REQUIRE(a.error() == 1);
        REQUIRE(a2.value() == 0);

        kz::expected<IntValue, IntMoveableValue> b{kz::unexpect, -1};
        auto b2 = kz::unexpected(IntMoveableValue{2});
        b = std::move(b2);
        REQUIRE(!b.has_value());
        REQUIRE(b.error() == 2);
        REQUIRE(b2.value() == 0);

        kz::expected<void, IntMoveableValue> c{};
        auto c2 = kz::unexpected(IntMoveableValue{3});
        c = std::move(c2);
        REQUIRE(!c.has_value());
        REQUIRE(c.error() == 3);
        REQUIRE(c2.value() == 0);

        kz::expected<void, IntMoveableValue> d{kz::unexpect, -2};
        auto d2 = kz::unexpected(IntMoveableValue{4});
        d = std::move(d2);
        REQUIRE(!d.has_value());
        REQUIRE(d.error() == 4);
        REQUIRE(d2.value() == 0);
    }
}

TEST_CASE("�.�.4.4, modifiers", "[expected]") {
    SECTION("emplace() - void - has value") {
        kz::expected<void, Error> a;

        a.emplace();

        REQUIRE(a);
    }

    SECTION("emplace() - void - has error") {
        kz::expected<void, IntValue> a(std::unexpect, 200);

        a.emplace();

        REQUIRE(a);
    }

    SECTION("emplace() - has value") {
        kz::expected<ComplexThing, Error> a{std::in_place, 3, 7};

        const auto& ref = a.emplace(6, 9);

        REQUIRE(std::addressof(ref) == std::addressof(*a));
        REQUIRE(a->a == 6);
        REQUIRE(a->b == 9);
    }

    SECTION("emplace() - has error - path 1") {
        kz::expected<IntValue, Error> a{kz::unexpect, Error::FileNotFound};
        static_assert(
            std::is_nothrow_constructible_v<decltype(a)::value_type, int>);

        const auto& ref = a.emplace(77);

        REQUIRE(std::addressof(ref) == std::addressof(*a));
        REQUIRE(a.value() == 77);
    }

    SECTION("emplace() - has error - path 2") {
        kz::expected<std::vector<int>, Error> a{
            kz::unexpect, Error::FileNotFound};
        static_assert(
            !std::is_nothrow_constructible_v<decltype(a)::value_type, int>);
        static_assert(
            std::is_nothrow_move_constructible_v<decltype(a)::value_type>);

        const auto& ref = a.emplace(5);

        REQUIRE(std::addressof(ref) == std::addressof(*a));
        REQUIRE(a.value().size() == 5);
    }

    SECTION("emplace() - has error - path 3") {
        kz::expected<NotNoThrowConstructible, Error> a{
            kz::unexpect, Error::FileNotFound};
        static_assert(
            !std::is_nothrow_constructible_v<decltype(a)::value_type, int>);
        static_assert(
            !std::is_nothrow_move_constructible_v<decltype(a)::value_type>);

        const auto& ref = a.emplace(99);

        REQUIRE(std::addressof(ref) == std::addressof(*a));
        REQUIRE(a.value().value == 99);
    }

    SECTION("emplace() - has error - path 3 - throws") {
        // TODO: same as path 3 above, but needs to throw on construction of
        // T(std::foward<Arg>(args)...)
    }

    SECTION("emplace(initializer_list) - has value") {
        kz::expected<ComplexThing, Error> a{std::in_place, 3, 7};

        const auto& ref = a.emplace({1, 2, 3}, 6, 9);

        REQUIRE(std::addressof(ref) == std::addressof(*a));
        REQUIRE(a->list.size() == 3);
        REQUIRE(a->a == 6);
        REQUIRE(a->b == 9);
    }

    // SECTION("emplace(initializer_list) - has error - path 1") {
    //     kz::expected<ComplexThing, Error> a{
    //         kz::unexpect, Error::FileNotFound};
    //     static_assert(std::is_nothrow_constructible_v<decltype(a)::value_type,
    //         std::initializer_list<int>, int, int>);

    //     a.emplace({1, 2, 3}, 6, 9);

    //     // REQUIRE(std::addressof(ref) == std::addressof(*a));
    //     REQUIRE(a->list.size() == 3);
    //     REQUIRE(a->a == 6);
    //     REQUIRE(a->b == 9);
    // }

    // SECTION("emplace(initializer_list) - has error - path 2") {
    //     kz::expected<ComplexThing, Error> a{
    //         kz::unexpect, Error::FileNotFound};
    //     static_assert(!std::is_nothrow_constructible_v<decltype(a)::value_type,
    //                   std::initializer_list<int>, int, int>);
    //     static_assert(
    //         std::is_nothrow_move_constructible_v<decltype(a)::value_type>);

    //     a.emplace({1, 2, 3}, 6, 9);

    //     // REQUIRE(std::addressof(ref) == std::addressof(*a));
    //     REQUIRE(a->list.size() == 3);
    //     REQUIRE(a->a == 6);
    //     REQUIRE(a->b == 9);
    // }

    SECTION("emplace(initializer_list) - has error - path 3") {
        kz::expected<ComplexThing, Error> a{kz::unexpect, Error::FileNotFound};

        static_assert(!std::is_nothrow_constructible_v<decltype(a)::value_type,
                      std::initializer_list<int>, int, int>);
        static_assert(
            !std::is_nothrow_move_constructible_v<decltype(a)::value_type>);
        a.emplace({1, 2, 3}, 6, 9);

        // REQUIRE(std::addressof(ref) == std::addressof(*a));
        REQUIRE(a->list.size() == 3);
        REQUIRE(a->a == 6);
        REQUIRE(a->b == 9);
    }
}

TEST_CASE("�.�.4.5, swap", "[expected]") {
    SECTION("swap two values") {
        kz::expected<IntValue, Error> a{123};
        kz::expected<IntValue, Error> b{456};

        a.swap(b);

        REQUIRE(a.value() == 456);
        REQUIRE(b.value() == 123);
    }

    SECTION("swap two errors") {
        kz::expected<IntValue, Error> a{kz::unexpect, Error::FileNotFound};
        kz::expected<IntValue, Error> b{kz::unexpect, Error::FlyingSquirrels};
        a.swap(b);

        REQUIRE(a.error() == Error::FlyingSquirrels);
        REQUIRE(b.error() == Error::FileNotFound);
    }

    SECTION("swap value and error - path 1 - nothrow") {
        using type = kz::expected<IntValue, Error>;
        type a{123};
        type b{kz::unexpect, Error::IOError};
        static_assert(std::is_nothrow_move_constructible_v<type::error_type>);

        a.swap(b);

        REQUIRE(a.error() == Error::IOError);
        REQUIRE(b.value() == 123);

        a.swap(b);

        REQUIRE(a.value() == 123);
        REQUIRE(b.error() == Error::IOError);
    }

    // TODO: SECTION("swap value and error - path 1 - throw")

    SECTION("swap value and error - path 2 - nothrow") {
        using type = kz::expected<IntValue, IntMoveableValue>;
        type a{123};
        type b{kz::unexpect, 456};

        static_assert(!std::is_nothrow_move_constructible_v<type::error_type>);
        static_assert(std::is_nothrow_move_constructible_v<type::value_type>);

        a.swap(b);

        REQUIRE(!a.has_value());
        REQUIRE(a.error() == 456);
        REQUIRE(b.has_value());
        REQUIRE(*b == 123);

        a.swap(b);

        REQUIRE(a.has_value());
        REQUIRE(*a == 123);
        REQUIRE(!b.has_value());
        REQUIRE(b.error() == 456);
    }

    // TODO: SECTION("swap value and error - path 2 - throw")

    SECTION("swap two values - void specialization") {
        kz::expected<void, Error> a{};
        kz::expected<void, Error> b{};

        a.swap(b);

        REQUIRE(a.has_value());
        REQUIRE(b.has_value());
    }

    SECTION("swap two errors - void specialization") {
        kz::expected<void, Error> a{kz::unexpect, Error::FileNotFound};
        kz::expected<void, Error> b{kz::unexpect, Error::IOError};

        a.swap(b);

        REQUIRE(!a.has_value());
        REQUIRE(a.error() == Error::IOError);
        REQUIRE(!b.has_value());
        REQUIRE(b.error() == Error::FileNotFound);
    }

    SECTION("swap value and error - void specialization") {
        kz::expected<void, Error> a{};
        kz::expected<void, Error> b{kz::unexpect, Error::FlyingSquirrels};

        a.swap(b);

        REQUIRE(!a.has_value());
        REQUIRE(a.error() == Error::FlyingSquirrels);
        REQUIRE(b.has_value());

        a.swap(b);

        REQUIRE(a.has_value());
        REQUIRE(!b.has_value());
        REQUIRE(b.error() == Error::FlyingSquirrels);
    }

    SECTION("swap()") {
        kz::expected<IntValue, Error> a{123};
        kz::expected<IntValue, Error> b{456};

        std::swap(a, b);

        REQUIRE(a == 456);
        REQUIRE(b == 123);
    }
}

TEST_CASE("expected accessors", "[expected]") {

    SECTION("operator bool / has_value()") {
        kz::expected<IntValue, Error> a{123};
        kz::expected<IntValue, Error> b{kz::unexpected(Error::FlyingSquirrels)};
        REQUIRE(a);
        REQUIRE(a.has_value());
        REQUIRE(!b);
        REQUIRE(!b.has_value());

        kz::expected<void, Error> c;
        kz::expected<void, Error> d{kz::unexpected(Error::FlyingSquirrels)};
        REQUIRE(c);
        REQUIRE(c.has_value());
        REQUIRE(!d);
        REQUIRE(!d.has_value());
    }

    SECTION("operator->()") {
        kz::expected<IntValue, Error> a{11};
        REQUIRE(a->value == 11);

        *a.operator->() = 22;
        REQUIRE(*a == 22);

        const kz::expected<IntValue, Error> b{33};
        REQUIRE(b->value == 33);
    }

    SECTION("operator*()") {
        // &
        kz::expected<IntValue, Error> a{11};
        REQUIRE(*a == 11);

        // const&
        const kz::expected<IntValue, Error> b{22};
        REQUIRE(*b == 22);

        // &&
        kz::expected<IntValue, Error> c{33};
        REQUIRE(*std::move(c) == 33);

        // const&&
        const kz::expected<IntValue, Error> d{44};
        REQUIRE(*std::move(d) == 44);
    }

    SECTION("value() - has value") {
        // &
        kz::expected<IntValue, Error> a{11};
        REQUIRE(a.value() == 11);

        // const&
        const kz::expected<IntValue, Error> b{22};
        REQUIRE(b.value() == 22);

        // &&
        kz::expected<IntValue, Error> c{33};
        REQUIRE(std::move(c).value() == 33);

        // const&&
        const kz::expected<IntValue, Error> d{44};
        REQUIRE(std::move(d).value() == 44);
    }

#if KZ_EXCEPTIONS
    SECTION("value() - has error") {
        // &
        kz::expected<int, Error> a{kz::unexpected(Error::FileNotFound)};
        REQUIRE_THROWS_AS(a.value(), kz::bad_expected_access<Error>);

        // const&
        const kz::expected<int, Error> b{kz::unexpected(Error::FileNotFound)};
        REQUIRE_THROWS_AS(b.value(), kz::bad_expected_access<Error>);

        // &&
        kz::expected<int, Error> c{kz::unexpected(Error::FileNotFound)};
        REQUIRE_THROWS_AS(std::move(c).value(), kz::bad_expected_access<Error>);

        // const&&
        const kz::expected<int, Error> d{kz::unexpected(Error::FileNotFound)};
        REQUIRE_THROWS_AS(std::move(d).value(), kz::bad_expected_access<Error>);
    }
#endif

    SECTION("error()") {
        // &
        kz::expected<int, IntValue> a{kz::unexpected{11}};
        REQUIRE(a.error() == 11);

        // const&
        const kz::expected<int, IntValue> b{kz::unexpected{22}};
        REQUIRE(b.error() == 22);

        // &&
        kz::expected<int, IntValue> c{kz::unexpected{33}};
        REQUIRE(std::move(c).error() == 33);

        // const&&
        const kz::expected<int, IntValue> d{kz::unexpected{44}};
        REQUIRE(std::move(d).error() == 44);
    }

    SECTION("value_or()") {
        // value - const&
        const kz::expected<int, Error> a{31};
        REQUIRE(a.value_or(42) == 31);

        // error - const&
        const kz::expected<int, Error> b{kz::unexpect, Error::IOError};
        REQUIRE(b.value_or(42) == 42);

        // value - &&
        kz::expected<int, Error> c{69};
        REQUIRE(std::move(c).value_or(777) == 69);

        // error - &&
        kz::expected<int, Error> d{kz::unexpect, Error::FileNotFound};
        REQUIRE(std::move(d).value_or(84) == 84);
    }
}

TEST_CASE("Equality operators", "[expected]") {
    SECTION("expected and expected") {
        const kz::expected<int, int> a{1};
        const kz::expected<int, int> b{2};
        const kz::expected<int, int> c{2};
        const kz::expected<int, int> d{kz::unexpect, 1};
        const kz::expected<int, int> e{kz::unexpect, 2};
        const kz::expected<int, int> f{kz::unexpect, 2};

        REQUIRE(a != b);
        REQUIRE(a != d);
        REQUIRE(a != e);
        REQUIRE(b == c);
        REQUIRE(b != e);
        REQUIRE(d != e);
        REQUIRE(e == f);
    }

    SECTION("expected and expected - void specialization") {
        const kz::expected<void, int> a{};
        const kz::expected<void, int> b{};
        const kz::expected<void, int> d{kz::unexpect, 1};
        const kz::expected<void, int> e{kz::unexpect, 2};
        const kz::expected<void, int> f{kz::unexpect, 2};

        REQUIRE(a == b);
        REQUIRE(a != e);
        REQUIRE(d != e);
        REQUIRE(e == f);
    }

    SECTION("expected and expected - mix and match void specialization") {
        const kz::expected<int, int> a{1};
        const kz::expected<void, int> b{};
        const kz::expected<int, int> c{kz::unexpect, 1};
        const kz::expected<void, int> d{kz::unexpect, 1};

        REQUIRE(a != b);
        REQUIRE(a != c);
        REQUIRE(a != d);
        REQUIRE(b != c);
        REQUIRE(b != d);
        REQUIRE(c == d);

        REQUIRE(b != a);
        REQUIRE(c != a);
        REQUIRE(d != a);
        REQUIRE(c != b);
        REQUIRE(d != b);
        REQUIRE(d == c);
    }

    SECTION("expected and value") {
        const kz::expected<int, int> a{22};
        REQUIRE(a == 22);
        REQUIRE(22 == a);
        REQUIRE(a != 44);
        REQUIRE(44 != a);
    }

    SECTION("expected and unexpected") {
        const kz::expected<int, int> a{kz::unexpect, 100};
        REQUIRE(a == kz::unexpected(100));
        REQUIRE(kz::unexpected(100) == a);
        REQUIRE(a != kz::unexpected(200));
        REQUIRE(kz::unexpected(200) != a);
    }
}
