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

#include <expected>
#include <catch2/catch.hpp>
#include "old.value.hpp"

enum class Error { FileNotFound, IOError, FlyingSquirrels };

TEST_CASE("expected constructors", "[expected]") {

    SECTION("default") {
        std::expected<DefaultInt, Error> a;
        REQUIRE(a.has_value());
        REQUIRE(*a == DefaultInt::DefaultValue);

        std::expected<void, Error> b;
        REQUIRE(b.has_value());
    }

    SECTION("from value") {
        std::expected<IntValue, Error> a{123};
        REQUIRE(a.has_value());
        REQUIRE(*a == 123);
    }

    SECTION("from complex value") {
        const IntValue a{100};
        IntMoveableValue b{200};
        std::expected<ComplexThing, Error> c{std::in_place, a, std::move(b)};
        REQUIRE(c->a == 100);
        REQUIRE(c->b == 200);
        REQUIRE(a == 100);
        REQUIRE(b == 0);
    }

    SECTION("from error") {
        std::unexpected error{Error::IOError};
        std::expected<IntValue, Error> a{error};
        REQUIRE(!a.has_value());
        REQUIRE(a.error() == Error::IOError);

        std::unexpected error2{Error::FlyingSquirrels};
        std::expected<void, Error> b{error2};
        REQUIRE(!b.has_value());
        REQUIRE(b.error() == Error::FlyingSquirrels);
    }

    SECTION("from complex error") {

        std::unexpected<ComplexThing> error{
            ComplexThing{IntValue{100}, IntMoveableValue{200}}};
        std::expected<int, ComplexThing> c{std::move(error)};
        REQUIRE(c.error().a == 100);
        REQUIRE(c.error().b == 200);
        REQUIRE(error.value().a == 100);
        REQUIRE(error.value().b == 0);
    }

    SECTION("in-place with value") {
        std::expected<IntValue, Error> a{std::in_place, 123};
        REQUIRE(*a == 123);
    }

    SECTION("in-place with multiple parameters") {
        const IntValue a{100};
        IntMoveableValue b{200};
        std::expected<ComplexThing, Error> c{std::in_place, a, std::move(b)};
        REQUIRE(c->a == 100);
        REQUIRE(c->b == 200);
        REQUIRE(a == 100);
        REQUIRE(b == 0);
    }

    SECTION("in-place with initializer list") {
        std::expected<std::vector<int>, Error> a{std::in_place, {1, 2, 3}};
        REQUIRE(a->size() == 3);
    }

    SECTION("in-place with initializer list and extra parameters") {
        const IntValue a(100);
        IntMoveableValue b(200);
        std::expected<ComplexThing, Error> c{
            std::in_place, {1, 2, 3}, a, std::move(b)};
        REQUIRE(c->a == 100);
        REQUIRE(c->b == 200);
        REQUIRE(c->list.size() == 3);
        REQUIRE(a == 100);
        REQUIRE(b == 0);
    }

    SECTION("unexpect_t with value") {
        std::expected<int, IntValue> a{std::unexpect, 123};

        REQUIRE(!a.has_value());
        REQUIRE(a.error() == 123);
    }

    SECTION("unexpect_t with initializer list") {
        std::expected<int, std::vector<int>> a{std::unexpect, {1, 2, 3}};
        REQUIRE(a.error().size() == 3);
    }

    SECTION("unexpect_t with multiple parameters") {
        const IntValue a{100};
        IntMoveableValue b{200};
        std::expected<int, ComplexThing> c{std::unexpect, a, std::move(b)};
        REQUIRE(c.error().a == 100);
        REQUIRE(c.error().b == 200);
        REQUIRE(a == 100);
        REQUIRE(b == 0);
    }

    SECTION("unexpect_t with initializer list and extra parameters") {
        const IntValue a(100);
        IntMoveableValue b(200);
        std::expected<int, ComplexThing> c{
            std::unexpect, {1, 2, 3}, a, std::move(b)};
        REQUIRE(c.error().a == 100);
        REQUIRE(c.error().b == 200);
        REQUIRE(c.error().list.size() == 3);
        REQUIRE(a == 100);
        REQUIRE(b == 0);
    }

    SECTION("copy - value") {
        const std::expected<IntValue, Error> a{3};
        std::expected<IntValue, Error> b{a};
        REQUIRE(a.has_value());
        REQUIRE(b.has_value());
        REQUIRE(*a == 3);
        REQUIRE(*b == 3);

        const std::expected<void, Error> c;
        std::expected<void, Error> d{c};
        REQUIRE(c.has_value());
        REQUIRE(d.has_value());
    }

    SECTION("copy - error") {
        const std::expected<IntValue, Error> a{
            std::unexpected(Error::FileNotFound)};
        std::expected<IntValue, Error> b{a};
        REQUIRE(!a.has_value());
        REQUIRE(!b.has_value());
        REQUIRE(a.error() == Error::FileNotFound);
        REQUIRE(b.error() == Error::FileNotFound);

        const std::expected<void, Error> c{std::unexpected(Error::IOError)};
        std::expected<void, Error> d{c};
        REQUIRE(!c.has_value());
        REQUIRE(!d.has_value());
        REQUIRE(c.error() == Error::IOError);
        REQUIRE(d.error() == Error::IOError);
    }

    SECTION("move - value") {
        std::expected<IntMoveableValue, Error> a{4};
        const std::expected<IntMoveableValue, Error> b{std::move(a)};
        REQUIRE(a.has_value());
        REQUIRE(b.has_value());
        REQUIRE(*a == 0);
        REQUIRE(*b == 4);

        std::expected<void, Error> c;
        const std::expected<void, Error> d{std::move(c)};
        REQUIRE(c.has_value());
        REQUIRE(d.has_value());
    }

    SECTION("move - error") {
        std::expected<IntValue, IntMoveableValue> a{
            std::unexpected<IntMoveableValue>{55}};
        const std::expected<IntValue, IntMoveableValue> b{std::move(a)};
        REQUIRE(!a.has_value());
        REQUIRE(!b.has_value());
        REQUIRE(a.error() == 0);
        REQUIRE(b.error() == 55);

        std::expected<void, IntMoveableValue> c{
            std::unexpected<IntMoveableValue>{66}};
        const std::expected<void, IntMoveableValue> d{std::move(c)};
        REQUIRE(!c.has_value());
        REQUIRE(!d.has_value());
        REQUIRE(c.error() == 0);
        REQUIRE(d.error() == 66);
    }

    SECTION("copy with conversion - value") {
        const std::expected<IntValue, Error> a{123};
        std::expected<LongValue, Error> b{a};
        REQUIRE(a.has_value());
        REQUIRE(b.has_value());
        REQUIRE(*a == 123);
        REQUIRE(*b == 123l);

        const std::expected<void, Error> c;
        std::expected<void, Error> d{c};
        REQUIRE(c.has_value());
        REQUIRE(d.has_value());
    }

    SECTION("copy with conversion - error") {
        const std::expected<IntValue, IntValue> a{std::unexpected(456)};
        std::expected<LongValue, LongValue> b{a};
        REQUIRE(!a.has_value());
        REQUIRE(!b.has_value());
        REQUIRE(a.error() == 456);
        REQUIRE(b.error() == 456l);

        const std::expected<void, IntValue> c{std::unexpected(789)};
        std::expected<void, LongValue> d{c};
        REQUIRE(!c.has_value());
        REQUIRE(!d.has_value());
        REQUIRE(c.error() == 789);
        REQUIRE(d.error() == 789l);
    }

    SECTION("move with conversion - value") {
        std::expected<IntMoveableValue, IntMoveableValue> a{69};
        std::expected<LongMoveableValue, LongMoveableValue> b{std::move(a)};
        REQUIRE(a.has_value());
        REQUIRE(b.has_value());
        REQUIRE(*a == 0);
        REQUIRE(*b == 69l);

        std::expected<void, IntMoveableValue> c;
        std::expected<void, LongMoveableValue> d{std::move(c)};
        REQUIRE(c.has_value());
        REQUIRE(d.has_value());
    }

    SECTION("move with conversion - error") {
        std::expected<IntMoveableValue, IntMoveableValue> a{
            std::unexpected(456)};
        std::expected<LongMoveableValue, LongMoveableValue> b{std::move(a)};
        REQUIRE(!a.has_value());
        REQUIRE(!b.has_value());
        REQUIRE(a.error() == 0);
        REQUIRE(b.error() == 456l);

        std::expected<void, IntMoveableValue> c{std::unexpected(789)};
        std::expected<void, LongMoveableValue> d{std::move(c)};
        REQUIRE(!c.has_value());
        REQUIRE(!d.has_value());
        REQUIRE(c.error() == 0);
        REQUIRE(d.error() == 789l);
    }
}

TEST_CASE("expected assignments", "[expected]") {

    SECTION("assign value to value") {
        const std::expected<IntValue, Error> a{111};
        std::expected<IntValue, Error> b{222};

        b = a;

        REQUIRE(a.has_value());
        REQUIRE(*a == 111);
        REQUIRE(b.has_value());
        REQUIRE(*b == 111);
    }

    SECTION("assign error to error") {
        const std::expected<IntValue, Error> a{
            std::unexpected(Error::FileNotFound)};
        std::expected<IntValue, Error> b{std::unexpected(Error::IOError)};

        b = a;

        REQUIRE(!a.has_value());
        REQUIRE(a.error() == Error::FileNotFound);
        REQUIRE(!b.has_value());
        REQUIRE(b.error() == Error::FileNotFound);
    }

    SECTION("assign value to error - is_nothrow_constructible_v<>") {
        using type = std::expected<int, Error>;
        static_assert(std::is_nothrow_constructible_v<type::value_type>);

        const type a{111};
        type b{std::unexpected(Error::FlyingSquirrels)};

        b = a;

        REQUIRE(a.has_value());
        REQUIRE(*a == 111);
        REQUIRE(b.has_value());
        REQUIRE(*b == 111);
    }

    SECTION("assign value to error - is_nothrow_move_constructible_v<>") {
        using type = std::expected<NotNoThrowConstructible, Error>;

        static_assert(!std::is_nothrow_constructible_v<type::value_type>);
        static_assert(std::is_nothrow_move_constructible_v<type::value_type>);

        const type a(std::in_place, 444);
        type b{std::unexpected(Error::FlyingSquirrels)};

        b = a;

        REQUIRE(a.has_value());
        REQUIRE(a->value == 444);
        REQUIRE(b.has_value());
        REQUIRE(b->value == 444);
    }

    SECTION("assign value to error - neither") {
        using type = std::expected<AssignableComplexThing, Error>;

        static_assert(!std::is_nothrow_constructible_v<type::value_type>);
        static_assert(!std::is_nothrow_move_constructible_v<type::value_type>);

        const type a{std::in_place, {1, 2, 3}, 45, 69};
        type b{std::unexpected(Error::FlyingSquirrels)};

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
    SECTION("assign value to error - neither + throws exception on copy") {
        using type = std::expected<AssignableComplexThing, Error>;

        static_assert(!std::is_nothrow_constructible_v<type::value_type>);
        static_assert(!std::is_nothrow_move_constructible_v<type::value_type>);

        const type a{std::in_place, {1, 2, 3}, 45, 69};
        type b{std::unexpected(Error::FlyingSquirrels)};

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

    SECTION("assign error to value - is_nothrow_constructible_v<>") {
        using type = std::expected<IntValue, Error>;
        static_assert(std::is_nothrow_constructible_v<type::error_type>);

        const type a{std::unexpected(Error::FlyingSquirrels)};
        type b{111};

        b = a;

        REQUIRE(!a.has_value());
        REQUIRE(a.error() == Error::FlyingSquirrels);
        REQUIRE(!b.has_value());
        REQUIRE(b.error() == Error::FlyingSquirrels);
    }

    SECTION("assign error to value - is_nothrow_move_constructible_v<>") {
        using type = std::expected<IntValue, NotNoThrowConstructible>;

        static_assert(!std::is_nothrow_constructible_v<type::error_type>);
        static_assert(std::is_nothrow_move_constructible_v<type::error_type>);

        const type a{std::unexpected(666)};
        type b{222};

        b = a;

        REQUIRE(!a.has_value());
        REQUIRE(a.error().value == 666);
        REQUIRE(!b.has_value());
        REQUIRE(b.error().value == 666);
    }

    SECTION("assign error to value - neither") {
        using type = std::expected<IntValue, AssignableComplexThing>;

        static_assert(!std::is_nothrow_constructible_v<type::error_type>);
        static_assert(!std::is_nothrow_move_constructible_v<type::error_type>);

        const type a{std::unexpected(
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
    SECTION("assign error to value - neither + throws exception on copy") {
        using type = std::expected<IntValue, AssignableComplexThing>;

        static_assert(!std::is_nothrow_constructible_v<type::error_type>);
        static_assert(!std::is_nothrow_move_constructible_v<type::error_type>);

        const type a{std::unexpected(
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
        const std::expected<void, Error> a;
        std::expected<void, Error> b;

        b = a;

        REQUIRE(a.has_value());
        REQUIRE(b.has_value());
    }

    SECTION("assign error to error - void specialization") {
        const std::expected<void, Error> a{
            std::unexpected(Error::FileNotFound)};
        std::expected<void, Error> b{std::unexpected(Error::IOError)};

        b = a;

        REQUIRE(!a.has_value());
        REQUIRE(a.error() == Error::FileNotFound);
        REQUIRE(!b.has_value());
        REQUIRE(b.error() == Error::FileNotFound);
    }

    SECTION("assign value to error - void specialization") {
        using type = std::expected<void, AssignableComplexThing>;
        const type a{};
        type b{std::unexpect, {1, 2, 3}, 45, 69};

        b = a;

        REQUIRE(a.has_value());
        REQUIRE(b.has_value());
    }

    SECTION("error to value - void specialization") {
        using type = std::expected<void, AssignableComplexThing>;
        const type a{std::unexpect, {1, 2, 3}, 45, 69};
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
        std::expected<IntMoveableValue, Error> a{111};
        std::expected<IntMoveableValue, Error> b{222};

        b = std::move(a);

        REQUIRE(a.has_value());
        REQUIRE(*a == 0);
        REQUIRE(b.has_value());
        REQUIRE(*b == 111);
    }

    SECTION("move error to error") {
        std::expected<IntValue, IntMoveableValue> a{std::unexpected(14)};
        std::expected<IntValue, IntMoveableValue> b{std::unexpected(17)};

        b = std::move(a);

        REQUIRE(!a.has_value());
        REQUIRE(a.error() == 0);
        REQUIRE(!b.has_value());
        REQUIRE(b.error() == 14);
    }

    SECTION("move_value_to_error - is_nothrow_move_constructible_v<>") {
        using type = std::expected<std::vector<int>, Error>;
        static_assert(std::is_nothrow_move_constructible_v<type::value_type>);

        type a{std::in_place, {1, 2, 3}};
        type b{std::unexpected(Error::FlyingSquirrels)};

        b = std::move(a);

        REQUIRE(a.has_value());
        REQUIRE(a->empty());
        REQUIRE(b.has_value());
        REQUIRE(b->size() == 3);
    }

    SECTION("move_value_to_error - fallback") {
        using type = std::expected<IntMoveableValue, Error>;

        static_assert(!std::is_nothrow_move_constructible_v<type::value_type>);

        type a{143};
        type b{std::unexpected(Error::FlyingSquirrels)};

        b = std::move(a);

        REQUIRE(a.has_value());
        REQUIRE(*a == 0);
        REQUIRE(b.has_value());
        REQUIRE(*b == 143);
    }

#if KZ_EXCEPTIONS
    SECTION("move_value_to_error - fallback - throws on move") {
        using type = std::expected<IntMoveableValue, Error>;

        static_assert(!std::is_nothrow_move_constructible_v<type::value_type>);

        type a{143};
        type b{std::unexpected(Error::FlyingSquirrels)};

        a->throwsOnMove = true;
        REQUIRE_THROWS_AS(b = std::move(a), std::exception);

        REQUIRE(a.has_value());
        REQUIRE(*a == 143);
        REQUIRE(!b.has_value());
        REQUIRE(b.error() == Error::FlyingSquirrels);
    }
#endif

    SECTION("move_error_to_value - is_nothrow_move_constructible_v<>") {
        using type = std::expected<IntValue, std::vector<int>>;
        static_assert(std::is_nothrow_move_constructible_v<type::error_type>);

        type a{std::unexpect, {1, 2, 3}};
        type b{123};

        b = std::move(a);

        REQUIRE(!a.has_value());
        REQUIRE(a.error().empty());
        REQUIRE(!b.has_value());
        REQUIRE(b.error().size() == 3);
    }

    SECTION("move_error_to_value - fallback - throws on move") {
        using type = std::expected<IntValue, IntMoveableValue>;

        static_assert(!std::is_nothrow_move_constructible_v<type::error_type>);

        type a{std::unexpect, 143};
        type b{732};

        b = std::move(a);

        REQUIRE(!a.has_value());
        REQUIRE(*a == 0);
        REQUIRE(!b.has_value());
        REQUIRE(*b == 143);
    }

#if KZ_EXCEPTIONS
    SECTION("move_error_to_value - fallback - throws on move") {
        using type = std::expected<IntValue, IntMoveableValue>;

        static_assert(!std::is_nothrow_move_constructible_v<type::error_type>);

        type a{std::unexpect, 143};
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
        std::expected<void, Error> a{};
        std::expected<void, Error> b{};

        b = std::move(a);

        REQUIRE(a.has_value());
        REQUIRE(b.has_value());
    }

    SECTION("move error to error - void specialization") {
        std::expected<void, IntMoveableValue> a{std::unexpected(14)};
        std::expected<void, IntMoveableValue> b{std::unexpected(17)};

        b = std::move(a);

        REQUIRE(!a.has_value());
        REQUIRE(a.error() == 0);
        REQUIRE(!b.has_value());
        REQUIRE(b.error() == 14);
    }

    SECTION("move_value_to_error - void specialization") {
        using type = std::expected<void, Error>;
        type a{};
        type b{std::unexpected(Error::FlyingSquirrels)};

        b = std::move(a);

        REQUIRE(a.has_value());
        REQUIRE(b.has_value());
    }

    SECTION("move_error_to_value - void specialization") {
        using type = std::expected<void, std::vector<int>>;
        type a{std::unexpect, {1, 2, 3}};
        type b{};

        b = std::move(a);

        REQUIRE(!a.has_value());
        REQUIRE(a.error().empty());
        REQUIRE(!b.has_value());
        REQUIRE(b.error().size() == 3);
    }

#if KZ_EXCEPTIONS
    SECTION("move_error_to_value - void specialization - throws on move") {
        using type = std::expected<void, IntMoveableValue>;

        type a{std::unexpect, 143};
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
        std::expected<IntValue, Error> a{123};
        a = 456;
        REQUIRE(a.has_value());
        REQUIRE(*a == 456);

        std::expected<IntValue, Error> b{
            std::unexpected(Error::FlyingSquirrels)};
        b = 789;
        REQUIRE(b.has_value());
        REQUIRE(*b == 789);
    }

    SECTION("unexpected - const&") {
        std::expected<IntValue, Error> a{123};
        auto a2 = std::unexpected(Error::FileNotFound);
        a = a2;
        REQUIRE(!a.has_value());
        REQUIRE(a.error() == Error::FileNotFound);

        std::expected<IntValue, Error> b{std::unexpect, Error::IOError};
        auto b2 = std::unexpected(Error::FlyingSquirrels);
        b = b2;
        REQUIRE(!b.has_value());
        REQUIRE(b.error() == Error::FlyingSquirrels);

        std::expected<void, Error> c{};
        auto c2 = std::unexpected(Error::FileNotFound);
        c = c2;
        REQUIRE(!c.has_value());
        REQUIRE(c.error() == Error::FileNotFound);

        std::expected<void, Error> d{std::unexpect, Error::IOError};
        auto d2 = std::unexpected(Error::FlyingSquirrels);
        d = d2;
        REQUIRE(!d.has_value());
        REQUIRE(d.error() == Error::FlyingSquirrels);
    }

    SECTION("unexpected - &&") {
        std::expected<IntValue, IntMoveableValue> a{123};
        auto a2 = std::unexpected(IntMoveableValue{1});
        // TODO: this appears to be optimized to use a constructor:
        // expected(unexpected<E>&&) and do "the right thing", except
        // it's not calling operator=()..., same for all 4 cases below. We
        // might need to define a custom type that is not move-constructible but
        // is move-assignable.
        a = std::move(a2);
        REQUIRE(!a.has_value());
        REQUIRE(a.error() == 1);
        REQUIRE(a2.value() == 0);

        std::expected<IntValue, IntMoveableValue> b{std::unexpect, -1};
        auto b2 = std::unexpected(IntMoveableValue{2});
        b = std::move(b2);
        REQUIRE(!b.has_value());
        REQUIRE(b.error() == 2);
        REQUIRE(b2.value() == 0);

        std::expected<void, IntMoveableValue> c{};
        auto c2 = std::unexpected(IntMoveableValue{3});
        c = std::move(c2);
        REQUIRE(!c.has_value());
        REQUIRE(c.error() == 3);
        REQUIRE(c2.value() == 0);

        std::expected<void, IntMoveableValue> d{std::unexpect, -2};
        auto d2 = std::unexpected(IntMoveableValue{4});
        d = std::move(d2);
        REQUIRE(!d.has_value());
        REQUIRE(d.error() == 4);
        REQUIRE(d2.value() == 0);
    }
}

TEST_CASE("�.�.4.4, modifiers", "[expected]") {
    SECTION("emplace() - void - has value") {
        std::expected<void, Error> a;

        a.emplace();

        REQUIRE(a);
    }

    SECTION("emplace() - void - has error") {
        std::expected<void, IntValue> a(std::unexpect, 200);

        a.emplace();

        REQUIRE(a);
    }

    SECTION("emplace() - has value") {
        std::expected<SimpleThing, Error> a{std::in_place, 3, 7};
        REQUIRE(a->x == 3);
        REQUIRE(a->y == 7);

        const auto& ref = a.emplace(6, 9);

        REQUIRE(std::addressof(ref) == std::addressof(*a));
        REQUIRE(a->x == 6);
        REQUIRE(a->y == 9);
    }

    SECTION("emplace() - has error") {
        std::expected<IntValue, Error> a{std::unexpect, Error::FileNotFound};
        static_assert(
            std::is_nothrow_constructible_v<decltype(a)::value_type, int>);

        const auto& ref = a.emplace(77);

        REQUIRE(std::addressof(ref) == std::addressof(*a));
        REQUIRE(a.value() == 77);
    }

    SECTION("emplace(initializer_list) - has value") {
        std::expected<SimpleThing, Error> a{std::in_place, 3, 7};

        REQUIRE(a->n == 0);
        REQUIRE(a->x == 3);
        REQUIRE(a->y == 7);

        const auto& ref = a.emplace({1, 2, 3}, 6, 9);

        REQUIRE(std::addressof(ref) == std::addressof(*a));

        REQUIRE(a->n == 3);
        REQUIRE(a->x == 6);
        REQUIRE(a->y == 9);
    }

    SECTION("emplace(initializer_list) - has error") {
        std::expected<SimpleThing, Error> a{std::unexpect, Error::FileNotFound};

        static_assert(std::is_nothrow_constructible_v<decltype(a)::value_type,
            std::initializer_list<int>, int, int>);

        const auto& ref = a.emplace({1, 2, 3}, 6, 9);

        REQUIRE(std::addressof(ref) == std::addressof(*a));

        REQUIRE(a->n == 3);
        REQUIRE(a->x == 6);
        REQUIRE(a->y == 9);
    }
}

TEST_CASE("�.�.4.5, swap", "[expected]") {
    SECTION("swap two values") {
        std::expected<IntValue, Error> a{123};
        std::expected<IntValue, Error> b{456};

        a.swap(b);

        REQUIRE(a.value() == 456);
        REQUIRE(b.value() == 123);
    }

    SECTION("swap two errors") {
        std::expected<IntValue, Error> a{std::unexpect, Error::FileNotFound};
        std::expected<IntValue, Error> b{std::unexpect, Error::FlyingSquirrels};
        a.swap(b);

        REQUIRE(a.error() == Error::FlyingSquirrels);
        REQUIRE(b.error() == Error::FileNotFound);
    }

    SECTION("swap value and error - path 1 - nothrow") {
        using type = std::expected<IntValue, Error>;
        type a{123};
        type b{std::unexpect, Error::IOError};
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
        using type = std::expected<IntValue, IntMoveableValue>;
        type a{123};
        type b{std::unexpect, 456};

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
        std::expected<void, Error> a{};
        std::expected<void, Error> b{};

        a.swap(b);

        REQUIRE(a.has_value());
        REQUIRE(b.has_value());
    }

    SECTION("swap two errors - void specialization") {
        std::expected<void, Error> a{std::unexpect, Error::FileNotFound};
        std::expected<void, Error> b{std::unexpect, Error::IOError};

        a.swap(b);

        REQUIRE(!a.has_value());
        REQUIRE(a.error() == Error::IOError);
        REQUIRE(!b.has_value());
        REQUIRE(b.error() == Error::FileNotFound);
    }

    SECTION("swap value and error - void specialization") {
        std::expected<void, Error> a{};
        std::expected<void, Error> b{std::unexpect, Error::FlyingSquirrels};

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
        std::expected<IntValue, Error> a{123};
        std::expected<IntValue, Error> b{456};

        std::swap(a, b);

        REQUIRE(a == 456);
        REQUIRE(b == 123);
    }
}

TEST_CASE("Equality operators", "[expected]") {
    SECTION("expected and expected") {
        const std::expected<int, int> a{1};
        const std::expected<int, int> b{2};
        const std::expected<int, int> c{2};
        const std::expected<int, int> d{std::unexpect, 1};
        const std::expected<int, int> e{std::unexpect, 2};
        const std::expected<int, int> f{std::unexpect, 2};

        REQUIRE(a != b);
        REQUIRE(a != d);
        REQUIRE(a != e);
        REQUIRE(b == c);
        REQUIRE(b != e);
        REQUIRE(d != e);
        REQUIRE(e == f);

        REQUIRE(a == 1);
        REQUIRE(b == 2);

        REQUIRE(d == std::unexpected<int>(1));
        REQUIRE(e == std::unexpected<int>(2));
    }

    SECTION("expected and expected - void specialization") {
        const std::expected<void, int> a{};
        const std::expected<void, int> b{};
        const std::expected<void, int> d{std::unexpect, 1};
        const std::expected<void, int> e{std::unexpect, 2};
        const std::expected<void, int> f{std::unexpect, 2};

        REQUIRE(a == b);
        REQUIRE(a != e);
        REQUIRE(d != e);
        REQUIRE(e == f);

        REQUIRE(d == std::unexpected<int>(1));
        REQUIRE(e == std::unexpected<int>(2));
    }

    SECTION("expected and expected - mix and match void specialization") {
        const std::expected<int, int> a{1};
        const std::expected<void, int> b{};
        const std::expected<int, int> c{std::unexpect, 1};
        const std::expected<void, int> d{std::unexpect, 1};

        REQUIRE(a != c);
        REQUIRE(b != d);

        REQUIRE(c != a);
        REQUIRE(d != b);
    }

    SECTION("expected and value") {
        const std::expected<int, int> a{22};
        REQUIRE(a == 22);
        REQUIRE(22 == a);
        REQUIRE(a != 44);
        REQUIRE(44 != a);
    }

    SECTION("expected and unexpected") {
        const std::expected<int, int> a{std::unexpect, 100};
        REQUIRE(a == std::unexpected(100));
        REQUIRE(std::unexpected(100) == a);
        REQUIRE(a != std::unexpected(200));
        REQUIRE(std::unexpected(200) != a);
    }
}
