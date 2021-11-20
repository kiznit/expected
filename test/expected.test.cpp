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
#include "value.hpp"

enum class Error { FileNotFound, IOError, FlyingSquirrels };

TEST_CASE("expected type aliases", "[expected]") {
    using T = mtl::expected<short, bool>;

    static_assert(std::is_same_v<T::value_type, short>);
    static_assert(std::is_same_v<T::error_type, bool>);
    static_assert(std::is_same_v<T::unexpected_type, mtl::unexpected<bool>>);

    using U = mtl::expected<void, bool>;
    static_assert(std::is_same_v<U::value_type, void>);
}

TEST_CASE("expected rebind", "[expected]") {
    using T = mtl::expected<short, bool>;
    using U = T::rebind<long>;

    static_assert(std::is_same_v<U::value_type, long>);
    static_assert(std::is_same_v<U::error_type, bool>);
}

TEST_CASE("expected constructors", "[expected]") {

    SECTION("default") {
        mtl::expected<DefaultInt, Error> a;
        REQUIRE(a.has_value());
        REQUIRE(*a == DefaultInt::DefaultValue);

        mtl::expected<void, Error> b;
        REQUIRE(b.has_value());
    }

    SECTION("from value") {
        mtl::expected<IntValue, Error> a{123};
        REQUIRE(a.has_value());
        REQUIRE(*a == 123);
    }

    SECTION("from complex value") {
        const IntValue a{100};
        IntMoveableValue b{200};
        mtl::expected<ComplexThing, Error> c{std::in_place, a, std::move(b)};
        REQUIRE(c->a == 100);
        REQUIRE(c->b == 200);
        REQUIRE(a == 100);
        REQUIRE(b == 0);
    }

    SECTION("from error") {
        mtl::unexpected error{Error::IOError};
        mtl::expected<IntValue, Error> a{error};
        REQUIRE(!a.has_value());
        REQUIRE(a.error() == Error::IOError);

        mtl::unexpected error2{Error::FlyingSquirrels};
        mtl::expected<void, Error> b{error2};
        REQUIRE(!b.has_value());
        REQUIRE(b.error() == Error::FlyingSquirrels);
    }

    SECTION("from complex error") {

        mtl::unexpected<ComplexThing> error{
            ComplexThing{IntValue{100}, IntMoveableValue{200}}};
        mtl::expected<int, ComplexThing> c{std::move(error)};
        REQUIRE(c.error().a == 100);
        REQUIRE(c.error().b == 200);
        REQUIRE(error.value().a == 100);
        REQUIRE(error.value().b == 0);
    }

    SECTION("in-place with value") {
        mtl::expected<IntValue, Error> a{std::in_place, 123};
        REQUIRE(*a == 123);
    }

    SECTION("in-place with multiple parameters") {
        const IntValue a{100};
        IntMoveableValue b{200};
        mtl::expected<ComplexThing, Error> c{std::in_place, a, std::move(b)};
        REQUIRE(c->a == 100);
        REQUIRE(c->b == 200);
        REQUIRE(a == 100);
        REQUIRE(b == 0);
    }

    SECTION("in-place with initializer list") {
        mtl::expected<std::vector<int>, Error> a{std::in_place, {1, 2, 3}};
        REQUIRE(a->size() == 3);
    }

    SECTION("in-place with initializer list and extra parameters") {
        const IntValue a(100);
        IntMoveableValue b(200);
        mtl::expected<ComplexThing, Error> c{
            std::in_place, {1, 2, 3}, a, std::move(b)};
        REQUIRE(c->a == 100);
        REQUIRE(c->b == 200);
        REQUIRE(c->list.size() == 3);
        REQUIRE(a == 100);
        REQUIRE(b == 0);
    }

    SECTION("unexpect_t with value") {
        mtl::expected<int, IntValue> a{mtl::unexpect, 123};

        REQUIRE(!a.has_value());
        REQUIRE(a.error() == 123);
    }

    SECTION("unexpect_t with initializer list") {
        mtl::expected<int, std::vector<int>> a{mtl::unexpect, {1, 2, 3}};
        REQUIRE(a.error().size() == 3);
    }

    SECTION("unexpect_t with multiple parameters") {
        const IntValue a{100};
        IntMoveableValue b{200};
        mtl::expected<int, ComplexThing> c{mtl::unexpect, a, std::move(b)};
        REQUIRE(c.error().a == 100);
        REQUIRE(c.error().b == 200);
        REQUIRE(a == 100);
        REQUIRE(b == 0);
    }

    SECTION("unexpect_t with initializer list and extra parameters") {
        const IntValue a(100);
        IntMoveableValue b(200);
        mtl::expected<int, ComplexThing> c{
            mtl::unexpect, {1, 2, 3}, a, std::move(b)};
        REQUIRE(c.error().a == 100);
        REQUIRE(c.error().b == 200);
        REQUIRE(c.error().list.size() == 3);
        REQUIRE(a == 100);
        REQUIRE(b == 0);
    }

    SECTION("copy - value") {
        const mtl::expected<IntValue, Error> a{3};
        mtl::expected<IntValue, Error> b{a};
        REQUIRE(a.has_value());
        REQUIRE(b.has_value());
        REQUIRE(*a == 3);
        REQUIRE(*b == 3);

        const mtl::expected<void, Error> c;
        mtl::expected<void, Error> d{c};
        REQUIRE(c.has_value());
        REQUIRE(d.has_value());
    }

    SECTION("copy - error") {
        const mtl::expected<IntValue, Error> a{
            mtl::unexpected(Error::FileNotFound)};
        mtl::expected<IntValue, Error> b{a};
        REQUIRE(!a.has_value());
        REQUIRE(!b.has_value());
        REQUIRE(a.error() == Error::FileNotFound);
        REQUIRE(b.error() == Error::FileNotFound);

        const mtl::expected<void, Error> c{mtl::unexpected(Error::IOError)};
        mtl::expected<void, Error> d{c};
        REQUIRE(!c.has_value());
        REQUIRE(!d.has_value());
        REQUIRE(c.error() == Error::IOError);
        REQUIRE(d.error() == Error::IOError);
    }

    SECTION("move - value") {
        mtl::expected<IntMoveableValue, Error> a{4};
        const mtl::expected<IntMoveableValue, Error> b{std::move(a)};
        REQUIRE(a.has_value());
        REQUIRE(b.has_value());
        REQUIRE(*a == 0);
        REQUIRE(*b == 4);

        mtl::expected<void, Error> c;
        const mtl::expected<void, Error> d{std::move(c)};
        REQUIRE(c.has_value());
        REQUIRE(d.has_value());
    }

    SECTION("move - error") {
        mtl::expected<IntValue, IntMoveableValue> a{
            mtl::unexpected<IntMoveableValue>{55}};
        const mtl::expected<IntValue, IntMoveableValue> b{std::move(a)};
        REQUIRE(!a.has_value());
        REQUIRE(!b.has_value());
        REQUIRE(a.error() == 0);
        REQUIRE(b.error() == 55);

        mtl::expected<void, IntMoveableValue> c{
            mtl::unexpected<IntMoveableValue>{66}};
        const mtl::expected<void, IntMoveableValue> d{std::move(c)};
        REQUIRE(!c.has_value());
        REQUIRE(!d.has_value());
        REQUIRE(c.error() == 0);
        REQUIRE(d.error() == 66);
    }

    SECTION("copy with conversion - value") {
        const mtl::expected<IntValue, Error> a{123};
        mtl::expected<LongValue, Error> b{a};
        REQUIRE(a.has_value());
        REQUIRE(b.has_value());
        REQUIRE(*a == 123);
        REQUIRE(*b == 123l);

        const mtl::expected<void, Error> c;
        mtl::expected<void, Error> d{c};
        REQUIRE(c.has_value());
        REQUIRE(d.has_value());
    }

    SECTION("copy with conversion - error") {
        const mtl::expected<IntValue, IntValue> a{mtl::unexpected(456)};
        mtl::expected<LongValue, LongValue> b{a};
        REQUIRE(!a.has_value());
        REQUIRE(!b.has_value());
        REQUIRE(a.error() == 456);
        REQUIRE(b.error() == 456l);

        const mtl::expected<void, IntValue> c{mtl::unexpected(789)};
        mtl::expected<void, LongValue> d{c};
        REQUIRE(!c.has_value());
        REQUIRE(!d.has_value());
        REQUIRE(c.error() == 789);
        REQUIRE(d.error() == 789l);
    }

    SECTION("move with conversion - value") {
        mtl::expected<IntMoveableValue, IntMoveableValue> a{69};
        mtl::expected<LongMoveableValue, LongMoveableValue> b{std::move(a)};
        REQUIRE(a.has_value());
        REQUIRE(b.has_value());
        REQUIRE(*a == 0);
        REQUIRE(*b == 69l);

        mtl::expected<void, IntMoveableValue> c;
        mtl::expected<void, LongMoveableValue> d{std::move(c)};
        REQUIRE(c.has_value());
        REQUIRE(d.has_value());
    }

    SECTION("move with conversion - error") {
        mtl::expected<IntMoveableValue, IntMoveableValue> a{
            mtl::unexpected(456)};
        mtl::expected<LongMoveableValue, LongMoveableValue> b{std::move(a)};
        REQUIRE(!a.has_value());
        REQUIRE(!b.has_value());
        REQUIRE(a.error() == 0);
        REQUIRE(b.error() == 456l);

        mtl::expected<void, IntMoveableValue> c{mtl::unexpected(789)};
        mtl::expected<void, LongMoveableValue> d{std::move(c)};
        REQUIRE(!c.has_value());
        REQUIRE(!d.has_value());
        REQUIRE(c.error() == 0);
        REQUIRE(d.error() == 789l);
    }
}

TEST_CASE("expected assignments", "[expected]") {

    SECTION("assign value to value") {
        const mtl::expected<IntValue, Error> a{111};
        mtl::expected<IntValue, Error> b{222};

        b = a;

        REQUIRE(a.has_value());
        REQUIRE(*a == 111);
        REQUIRE(b.has_value());
        REQUIRE(*b == 111);
    }

    SECTION("assign error to error") {
        const mtl::expected<IntValue, Error> a{
            mtl::unexpected(Error::FileNotFound)};
        mtl::expected<IntValue, Error> b{mtl::unexpected(Error::IOError)};

        b = a;

        REQUIRE(!a.has_value());
        REQUIRE(a.error() == Error::FileNotFound);
        REQUIRE(!b.has_value());
        REQUIRE(b.error() == Error::FileNotFound);
    }

    SECTION("assign_value_to_error - is_nothrow_copy_constructible_v<>") {
        using type = mtl::expected<IntValue, Error>;
        static_assert(std::is_nothrow_copy_constructible_v<type::value_type>);

        const type a{111};
        type b{mtl::unexpected(Error::FlyingSquirrels)};

        b = a;

        REQUIRE(a.has_value());
        REQUIRE(*a == 111);
        REQUIRE(b.has_value());
        REQUIRE(*b == 111);
    }

    SECTION("assign_value_to_error - is_nothrow_move_constructible_v<>") {
        using type = mtl::expected<std::vector<int>, Error>;
        static_assert(!std::is_nothrow_copy_constructible_v<type::value_type>);
        static_assert(std::is_nothrow_move_constructible_v<type::value_type>);

        const type a(std::in_place, {1, 2, 3});
        type b{mtl::unexpected(Error::FlyingSquirrels)};

        b = a;

        REQUIRE(a.has_value());
        REQUIRE(a->size() == 3);
        REQUIRE(b.has_value());
        REQUIRE(b->size() == 3);
    }

    SECTION("assign_value_to_error - neither") {
        using type = mtl::expected<AssignableComplexThing, Error>;
        static_assert(!std::is_nothrow_copy_constructible_v<type::value_type>);
        static_assert(!std::is_nothrow_move_constructible_v<type::value_type>);

        const type a{std::in_place, {1, 2, 3}, 45, 69};
        type b{mtl::unexpected(Error::FlyingSquirrels)};

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

#if MTL_EXCEPTIONS
    SECTION("assign_value_to_error - neither + throws exception on copy") {
        using type = mtl::expected<AssignableComplexThing, Error>;
        static_assert(!std::is_nothrow_copy_constructible_v<type::value_type>);
        static_assert(!std::is_nothrow_move_constructible_v<type::value_type>);

        const type a{std::in_place, {1, 2, 3}, 45, 69};
        type b{mtl::unexpected(Error::FlyingSquirrels)};

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
        using type = mtl::expected<IntValue, Error>;
        static_assert(std::is_nothrow_copy_constructible_v<type::error_type>);

        const type a{mtl::unexpected(Error::FlyingSquirrels)};
        type b{111};

        b = a;

        REQUIRE(!a.has_value());
        REQUIRE(a.error() == Error::FlyingSquirrels);
        REQUIRE(!b.has_value());
        REQUIRE(b.error() == Error::FlyingSquirrels);
    }

    SECTION("assign_error_to_value - is_nothrow_move_constructible_v<>") {
        using type = mtl::expected<IntValue, std::vector<int>>;
        static_assert(!std::is_nothrow_copy_constructible_v<type::error_type>);
        static_assert(std::is_nothrow_move_constructible_v<type::error_type>);

        const type a{mtl::unexpected(std::vector<int>{1, 2, 3})};
        type b{222};

        b = a;

        REQUIRE(!a.has_value());
        REQUIRE(a.error().size() == 3);
        REQUIRE(!b.has_value());
        REQUIRE(b.error().size() == 3);
    }

    SECTION("assign_error_to_value - neither") {
        using type = mtl::expected<IntValue, AssignableComplexThing>;
        static_assert(!std::is_nothrow_copy_constructible_v<type::error_type>);
        static_assert(!std::is_nothrow_move_constructible_v<type::error_type>);

        const type a{mtl::unexpected(
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

#if MTL_EXCEPTIONS
    SECTION("assign_error_to_value - neither + throws exception on copy") {
        using type = mtl::expected<IntValue, AssignableComplexThing>;
        static_assert(!std::is_nothrow_copy_constructible_v<type::error_type>);
        static_assert(!std::is_nothrow_move_constructible_v<type::error_type>);

        const type a{mtl::unexpected(
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
        const mtl::expected<void, Error> a;
        mtl::expected<void, Error> b;

        b = a;

        REQUIRE(a.has_value());
        REQUIRE(b.has_value());
    }

    SECTION("assign error to error - void specialization") {
        const mtl::expected<void, Error> a{
            mtl::unexpected(Error::FileNotFound)};
        mtl::expected<void, Error> b{mtl::unexpected(Error::IOError)};

        b = a;

        REQUIRE(!a.has_value());
        REQUIRE(a.error() == Error::FileNotFound);
        REQUIRE(!b.has_value());
        REQUIRE(b.error() == Error::FileNotFound);
    }

    SECTION("assign value to error - void specialization") {
        using type = mtl::expected<void, AssignableComplexThing>;
        const type a{};
        type b{mtl::unexpect, {1, 2, 3}, 45, 69};

        b = a;

        REQUIRE(a.has_value());
        REQUIRE(b.has_value());
    }

    SECTION("error to value - void specialization") {
        using type = mtl::expected<void, AssignableComplexThing>;
        const type a{mtl::unexpect, {1, 2, 3}, 45, 69};
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
        mtl::expected<IntMoveableValue, Error> a{111};
        mtl::expected<IntMoveableValue, Error> b{222};

        b = std::move(a);

        REQUIRE(a.has_value());
        REQUIRE(*a == 0);
        REQUIRE(b.has_value());
        REQUIRE(*b == 111);
    }

    SECTION("move error to error") {
        mtl::expected<IntValue, IntMoveableValue> a{mtl::unexpected(14)};
        mtl::expected<IntValue, IntMoveableValue> b{mtl::unexpected(17)};

        b = std::move(a);

        REQUIRE(!a.has_value());
        REQUIRE(a.error() == 0);
        REQUIRE(!b.has_value());
        REQUIRE(b.error() == 14);
    }

    SECTION("move_value_to_error - is_nothrow_move_constructible_v<>") {
        using type = mtl::expected<std::vector<int>, Error>;
        static_assert(std::is_nothrow_move_constructible_v<type::value_type>);

        type a{std::in_place, {1, 2, 3}};
        type b{mtl::unexpected(Error::FlyingSquirrels)};

        b = std::move(a);

        REQUIRE(a.has_value());
        REQUIRE(a->empty());
        REQUIRE(b.has_value());
        REQUIRE(b->size() == 3);
    }

    SECTION("move_value_to_error - fallback") {
        using type = mtl::expected<IntMoveableValue, Error>;
        static_assert(!std::is_nothrow_move_constructible_v<type::value_type>);

        type a{143};
        type b{mtl::unexpected(Error::FlyingSquirrels)};

        b = std::move(a);

        REQUIRE(a.has_value());
        REQUIRE(*a == 0);
        REQUIRE(b.has_value());
        REQUIRE(*b == 143);
    }

#if MTL_EXCEPTIONS
    SECTION("move_value_to_error - fallback - throws on move") {
        using type = mtl::expected<IntMoveableValue, Error>;
        static_assert(!std::is_nothrow_move_constructible_v<type::value_type>);

        type a{143};
        type b{mtl::unexpected(Error::FlyingSquirrels)};

        a->throwsOnMove = true;
        REQUIRE_THROWS_AS(b = std::move(a), std::exception);

        REQUIRE(a.has_value());
        REQUIRE(*a == 143);
        REQUIRE(!b.has_value());
        REQUIRE(b.error() == Error::FlyingSquirrels);
    }
#endif

    SECTION("move_error_to_value - is_nothrow_move_constructible_v<>") {
        using type = mtl::expected<IntValue, std::vector<int>>;
        static_assert(std::is_nothrow_move_constructible_v<type::error_type>);

        type a{mtl::unexpect, {1, 2, 3}};
        type b{123};

        b = std::move(a);

        REQUIRE(!a.has_value());
        REQUIRE(a.error().empty());
        REQUIRE(!b.has_value());
        REQUIRE(b.error().size() == 3);
    }

    SECTION("move_error_to_value - fallback - throws on move") {
        using type = mtl::expected<IntValue, IntMoveableValue>;
        static_assert(!std::is_nothrow_move_constructible_v<type::error_type>);

        type a{mtl::unexpect, 143};
        type b{732};

        b = std::move(a);

        REQUIRE(!a.has_value());
        REQUIRE(*a == 0);
        REQUIRE(!b.has_value());
        REQUIRE(*b == 143);
    }

#if MTL_EXCEPTIONS
    SECTION("move_error_to_value - fallback - throws on move") {
        using type = mtl::expected<IntValue, IntMoveableValue>;
        static_assert(!std::is_nothrow_move_constructible_v<type::error_type>);

        type a{mtl::unexpect, 143};
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
        mtl::expected<void, Error> a{};
        mtl::expected<void, Error> b{};

        b = std::move(a);

        REQUIRE(a.has_value());
        REQUIRE(b.has_value());
    }

    SECTION("move error to error - void specialization") {
        mtl::expected<void, IntMoveableValue> a{mtl::unexpected(14)};
        mtl::expected<void, IntMoveableValue> b{mtl::unexpected(17)};

        b = std::move(a);

        REQUIRE(!a.has_value());
        REQUIRE(a.error() == 0);
        REQUIRE(!b.has_value());
        REQUIRE(b.error() == 14);
    }

    SECTION("move_value_to_error - void specialization") {
        using type = mtl::expected<void, Error>;
        type a{};
        type b{mtl::unexpected(Error::FlyingSquirrels)};

        b = std::move(a);

        REQUIRE(a.has_value());
        REQUIRE(b.has_value());
    }

    SECTION("move_error_to_value - void specialization") {
        using type = mtl::expected<void, std::vector<int>>;
        type a{mtl::unexpect, {1, 2, 3}};
        type b{};

        b = std::move(a);

        REQUIRE(!a.has_value());
        REQUIRE(a.error().empty());
        REQUIRE(!b.has_value());
        REQUIRE(b.error().size() == 3);
    }

#if MTL_EXCEPTIONS
    SECTION("move_error_to_value - void specialization - throws on move") {
        using type = mtl::expected<void, IntMoveableValue>;

        type a{mtl::unexpect, 143};
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
        mtl::expected<IntValue, Error> a{123};
        a = 456;
        REQUIRE(a.has_value());
        REQUIRE(*a == 456);

        mtl::expected<IntValue, Error> b{
            mtl::unexpected(Error::FlyingSquirrels)};
        b = 789;
        REQUIRE(b.has_value());
        REQUIRE(*b == 789);

        using type = mtl::expected<NotNoThrowConstructible, IntMoveableValue>;
        static_assert(
            !std::is_nothrow_constructible_v<type::value_type,
                                             NotNoThrowConstructible>);
        type c{mtl::unexpected(777)};
        c = NotNoThrowConstructible{44};
        REQUIRE(c.has_value());
        REQUIRE(c->value == 44);

#if MTL_EXCEPTIONS
        // TODO: fix this test, we need a value_type that can throw on
        // construction
        type d{mtl::unexpected(666)};
        d.error().throwsOnMove = true;
        REQUIRE_THROWS_AS(d = NotNoThrowConstructible{55}, std::exception);
        REQUIRE(!d.has_value());
        REQUIRE(d.error() == 666);
#endif
    }

    SECTION("unexpected - const&") {
        mtl::expected<IntValue, Error> a{123};
        auto a2 = mtl::unexpected(Error::FileNotFound);
        a = a2;
        REQUIRE(!a.has_value());
        REQUIRE(a.error() == Error::FileNotFound);

        mtl::expected<IntValue, Error> b{mtl::unexpect, Error::IOError};
        auto b2 = mtl::unexpected(Error::FlyingSquirrels);
        b = b2;
        REQUIRE(!b.has_value());
        REQUIRE(b.error() == Error::FlyingSquirrels);

        mtl::expected<void, Error> c{};
        auto c2 = mtl::unexpected(Error::FileNotFound);
        c = c2;
        REQUIRE(!c.has_value());
        REQUIRE(c.error() == Error::FileNotFound);

        mtl::expected<void, Error> d{mtl::unexpect, Error::IOError};
        auto d2 = mtl::unexpected(Error::FlyingSquirrels);
        d = d2;
        REQUIRE(!d.has_value());
        REQUIRE(d.error() == Error::FlyingSquirrels);
    }

    SECTION("unexpected - &&") {
        mtl::expected<IntValue, IntMoveableValue> a{123};
        auto a2 = mtl::unexpected(IntMoveableValue{1});
        a = std::move(a2);
        REQUIRE(!a.has_value());
        REQUIRE(a.error() == 1);
        REQUIRE(a2.value() == 0);

        mtl::expected<IntValue, IntMoveableValue> b{mtl::unexpect, -1};
        auto b2 = mtl::unexpected(IntMoveableValue{2});
        b = std::move(b2);
        REQUIRE(!b.has_value());
        REQUIRE(b.error() == 2);
        REQUIRE(b2.value() == 0);

        mtl::expected<void, IntMoveableValue> c{};
        auto c2 = mtl::unexpected(IntMoveableValue{3});
        c = std::move(c2);
        REQUIRE(!c.has_value());
        REQUIRE(c.error() == 3);
        REQUIRE(c2.value() == 0);

        mtl::expected<void, IntMoveableValue> d{mtl::unexpect, -2};
        auto d2 = mtl::unexpected(IntMoveableValue{4});
        d = std::move(d2);
        REQUIRE(!d.has_value());
        REQUIRE(d.error() == 4);
        REQUIRE(d2.value() == 0);
    }
}

TEST_CASE("�.�.4.4, modifiers", "[expected]") {
    SECTION("emplace() - has value") {
        mtl::expected<ComplexThing, Error> a{std::in_place, 3, 7};

        const auto& ref = a.emplace(6, 9);

        REQUIRE(std::addressof(ref) == std::addressof(*a));
        REQUIRE(a->a == 6);
        REQUIRE(a->b == 9);
    }

    SECTION("emplace() - has error - path 1") {
        mtl::expected<IntValue, Error> a{mtl::unexpect, Error::FileNotFound};
        static_assert(
            std::is_nothrow_constructible_v<decltype(a)::value_type, int>);

        const auto& ref = a.emplace(77);

        REQUIRE(std::addressof(ref) == std::addressof(*a));
        REQUIRE(a.value() == 77);
    }

    SECTION("emplace() - has error - path 2") {
        mtl::expected<std::vector<int>, Error> a{mtl::unexpect,
                                                 Error::FileNotFound};
        static_assert(
            !std::is_nothrow_constructible_v<decltype(a)::value_type, int>);
        static_assert(
            std::is_nothrow_move_constructible_v<decltype(a)::value_type>);

        const auto& ref = a.emplace(5);

        REQUIRE(std::addressof(ref) == std::addressof(*a));
        REQUIRE(a.value().size() == 5);
    }

    SECTION("emplace() - has error - path 3") {
        mtl::expected<NotNoThrowConstructible, Error> a{mtl::unexpect,
                                                        Error::FileNotFound};
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
        mtl::expected<ComplexThing, Error> a{std::in_place, 3, 7};

        const auto& ref = a.emplace({1, 2, 3}, 6, 9);

        REQUIRE(std::addressof(ref) == std::addressof(*a));
        REQUIRE(a->list.size() == 3);
        REQUIRE(a->a == 6);
        REQUIRE(a->b == 9);
    }

    // TODO:
    // SECTION("emplace(initializer_list) - has error - path 1") {
    //     mtl::expected<ComplexThing, Error> a{mtl::unexpect,
    //                                           Error::FileNotFound};
    //     static_assert(
    //         std::is_nothrow_constructible_v<
    //             decltype(a)::value_type, std::initializer_list<int>, int,
    //             int>);

    //     a.emplace({1, 2, 3}, 6, 9);

    //     // REQUIRE(std::addressof(ref) == std::addressof(*a));
    //     REQUIRE(a->list.size() == 3);
    //     REQUIRE(a->a == 6);
    //     REQUIRE(a->b == 9);
    // }

    // TODO:
    // SECTION("emplace(initializer_list) - has error - path 2") {
    //     mtl::expected<ComplexThing, Error> a{mtl::unexpect,
    //                                           Error::FileNotFound};
    //     static_assert(
    //         !std::is_nothrow_constructible_v<
    //             decltype(a)::value_type, std::initializer_list<int>, int,
    //             int>);
    //     static_assert(
    //         std::is_nothrow_move_constructible_v<decltype(a)::value_type>);

    //     a.emplace({1, 2, 3}, 6, 9);

    //     // REQUIRE(std::addressof(ref) == std::addressof(*a));
    //     REQUIRE(a->list.size() == 3);
    //     REQUIRE(a->a == 6);
    //     REQUIRE(a->b == 9);
    // }

    SECTION("emplace(initializer_list) - has error - path 3") {
        mtl::expected<ComplexThing, Error> a{mtl::unexpect,
                                             Error::FileNotFound};
        static_assert(
            !std::is_nothrow_constructible_v<
                decltype(a)::value_type, std::initializer_list<int>, int, int>);
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
        mtl::expected<IntValue, Error> a{123};
        mtl::expected<IntValue, Error> b{456};

        a.swap(b);

        REQUIRE(a.value() == 456);
        REQUIRE(b.value() == 123);
    }

    SECTION("swap two errors") {
        mtl::expected<IntValue, Error> a{mtl::unexpect, Error::FileNotFound};
        mtl::expected<IntValue, Error> b{mtl::unexpect, Error::FlyingSquirrels};
        a.swap(b);

        REQUIRE(a.error() == Error::FlyingSquirrels);
        REQUIRE(b.error() == Error::FileNotFound);
    }

    SECTION("swap value and error - path 1 - nothrow") {
        using type = mtl::expected<IntValue, Error>;
        type a{123};
        type b{mtl::unexpect, Error::IOError};
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
        using type = mtl::expected<IntValue, IntMoveableValue>;
        type a{123};
        type b{mtl::unexpect, 456};
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
        mtl::expected<void, Error> a{};
        mtl::expected<void, Error> b{};

        a.swap(b);

        REQUIRE(a.has_value());
        REQUIRE(b.has_value());
    }

    SECTION("swap two errors - void specialization") {
        mtl::expected<void, Error> a{mtl::unexpect, Error::FileNotFound};
        mtl::expected<void, Error> b{mtl::unexpect, Error::IOError};

        a.swap(b);

        REQUIRE(!a.has_value());
        REQUIRE(a.error() == Error::IOError);
        REQUIRE(!b.has_value());
        REQUIRE(b.error() == Error::FileNotFound);
    }

    SECTION("swap value and error - void specialization") {
        mtl::expected<void, Error> a{};
        mtl::expected<void, Error> b{mtl::unexpect, Error::FlyingSquirrels};

        a.swap(b);

        REQUIRE(!a.has_value());
        REQUIRE(a.error() == Error::FlyingSquirrels);
        REQUIRE(b.has_value());

        a.swap(b);

        REQUIRE(a.has_value());
        REQUIRE(!b.has_value());
        REQUIRE(b.error() == Error::FlyingSquirrels);
    }
}

TEST_CASE("expected accessors", "[expected]") {

    SECTION("operator bool / has_value()") {
        mtl::expected<IntValue, Error> a{123};
        mtl::expected<IntValue, Error> b{
            mtl::unexpected(Error::FlyingSquirrels)};
        REQUIRE(a);
        REQUIRE(a.has_value());
        REQUIRE(!b);
        REQUIRE(!b.has_value());

        mtl::expected<void, Error> c;
        mtl::expected<void, Error> d{mtl::unexpected(Error::FlyingSquirrels)};
        REQUIRE(c);
        REQUIRE(c.has_value());
        REQUIRE(!d);
        REQUIRE(!d.has_value());
    }

    SECTION("operator->()") {
        mtl::expected<IntValue, Error> a{11};
        REQUIRE(a->value == 11);

        *a.operator->() = 22;
        REQUIRE(*a == 22);

        const mtl::expected<IntValue, Error> b{33};
        REQUIRE(b->value == 33);
    }

    SECTION("operator*()") {
        // &
        mtl::expected<IntValue, Error> a{11};
        REQUIRE(*a == 11);

        // const&
        const mtl::expected<IntValue, Error> b{22};
        REQUIRE(*b == 22);

        // &&
        mtl::expected<IntValue, Error> c{33};
        REQUIRE(*std::move(c) == 33);

        // const&&
        const mtl::expected<IntValue, Error> d{44};
        REQUIRE(*std::move(d) == 44);
    }

    SECTION("value() - has value") {
        // &
        mtl::expected<IntValue, Error> a{11};
        REQUIRE(a.value() == 11);

        // const&
        const mtl::expected<IntValue, Error> b{22};
        REQUIRE(b.value() == 22);

        // &&
        mtl::expected<IntValue, Error> c{33};
        REQUIRE(std::move(c).value() == 33);

        // const&&
        const mtl::expected<IntValue, Error> d{44};
        REQUIRE(std::move(d).value() == 44);
    }

#if MTL_EXCEPTIONS
    SECTION("value() - has error") {
        // &
        mtl::expected<int, Error> a{mtl::unexpected(Error::FileNotFound)};
        REQUIRE_THROWS_AS(a.value(), mtl::bad_expected_access<Error>);

        // const&
        const mtl::expected<int, Error> b{mtl::unexpected(Error::FileNotFound)};
        REQUIRE_THROWS_AS(b.value(), mtl::bad_expected_access<Error>);

        // &&
        mtl::expected<int, Error> c{mtl::unexpected(Error::FileNotFound)};
        REQUIRE_THROWS_AS(std::move(c).value(),
                          mtl::bad_expected_access<Error>);

        // const&&
        const mtl::expected<int, Error> d{mtl::unexpected(Error::FileNotFound)};
        REQUIRE_THROWS_AS(std::move(d).value(),
                          mtl::bad_expected_access<Error>);
    }
#endif

    SECTION("error()") {
        // &
        mtl::expected<int, IntValue> a{mtl::unexpected{11}};
        REQUIRE(a.error() == 11);

        // const&
        const mtl::expected<int, IntValue> b{mtl::unexpected{22}};
        REQUIRE(b.error() == 22);

        // &&
        mtl::expected<int, IntValue> c{mtl::unexpected{33}};
        REQUIRE(std::move(c).error() == 33);

        // const&&
        const mtl::expected<int, IntValue> d{mtl::unexpected{44}};
        REQUIRE(std::move(d).error() == 44);
    }

    SECTION("value_or()") {
        // value - const&
        const mtl::expected<int, Error> a{31};
        REQUIRE(a.value_or(42) == 31);

        // error - const&
        const mtl::expected<int, Error> b{mtl::unexpect, Error::IOError};
        REQUIRE(b.value_or(42) == 42);

        // value - &&
        mtl::expected<int, Error> c{69};
        REQUIRE(std::move(c).value_or(777) == 69);

        // error - &&
        mtl::expected<int, Error> d{mtl::unexpect, Error::FileNotFound};
        REQUIRE(std::move(d).value_or(84) == 84);
    }
}

TEST_CASE("Equality operators", "[expected]") {
    SECTION("expected and expected") {
        const mtl::expected<int, int> a{1};
        const mtl::expected<int, int> b{2};
        const mtl::expected<int, int> c{2};
        const mtl::expected<int, int> d{mtl::unexpect, 1};
        const mtl::expected<int, int> e{mtl::unexpect, 2};
        const mtl::expected<int, int> f{mtl::unexpect, 2};

        REQUIRE(a != b);
        REQUIRE(a != d);
        REQUIRE(a != e);
        REQUIRE(b == c);
        REQUIRE(b != e);
        REQUIRE(d != e);
        REQUIRE(e == f);
    }

    SECTION("expected and expected - void specialization") {
        const mtl::expected<void, int> a{};
        const mtl::expected<void, int> b{};
        const mtl::expected<void, int> d{mtl::unexpect, 1};
        const mtl::expected<void, int> e{mtl::unexpect, 2};
        const mtl::expected<void, int> f{mtl::unexpect, 2};

        REQUIRE(a == b);
        REQUIRE(a != e);
        REQUIRE(d != e);
        REQUIRE(e == f);
    }

    SECTION("expected and expected - mix and match void specialization") {
        const mtl::expected<int, int> a{1};
        const mtl::expected<void, int> b{};
        const mtl::expected<int, int> c{mtl::unexpect, 1};
        const mtl::expected<void, int> d{mtl::unexpect, 1};

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
        const mtl::expected<int, int> a{22};
        REQUIRE(a == 22);
        REQUIRE(22 == a);
        REQUIRE(a != 44);
        REQUIRE(44 != a);
    }

    SECTION("expected and unexpected") {
        const mtl::expected<int, int> a{mtl::unexpect, 100};
        REQUIRE(a == mtl::unexpected(100));
        REQUIRE(mtl::unexpected(100) == a);
        REQUIRE(a != mtl::unexpected(200));
        REQUIRE(mtl::unexpected(200) != a);
    }

    SECTION("swap()") {
        mtl::expected<IntValue, Error> a{123};
        mtl::expected<IntValue, Error> b{456};

        std::swap(a, b);

        REQUIRE(a == 456);
        REQUIRE(b == 123);
    }
}
