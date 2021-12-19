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

TEST_CASE("unexpected constructors", "[unexpected]") {

    static_assert(!std::is_default_constructible_v<kz::unexpected<int>>);

    SECTION("from value") {
        kz::unexpected<IntValue> a{123};
        REQUIRE(a.value() == 123);
    }

    SECTION("in-place with value") {
        kz::unexpected<IntValue> a{std::in_place, 123};
        REQUIRE(a.value() == 123);
    }

    SECTION("in-place with multiple parameters") {
        const IntValue a{100};
        IntMoveableValue b{200};
        kz::unexpected<ComplexThing> c{std::in_place, a, std::move(b)};

        REQUIRE(c.value().a == 100);
        REQUIRE(c.value().b == 200);
        REQUIRE(a == 100);
        REQUIRE(b == 0);
    }

    SECTION("in-place with initializer list") {
        kz::unexpected<std::vector<int>> a{std::in_place, {1, 2, 3}};
        REQUIRE(a.value().size() == 3);
    }

    SECTION("in-place with initializer list and extra parameters") {
        const IntValue a{100};
        IntMoveableValue b{200};
        kz::unexpected<ComplexThing> c{
            std::in_place, {1, 2, 3}, a, std::move(b)};
        REQUIRE(c.value().a == 100);
        REQUIRE(c.value().b == 200);
        REQUIRE(c.value().list.size() == 3);
        REQUIRE(a == 100);
        REQUIRE(b == 0);
    }

    SECTION("copy") {

        const kz::unexpected<IntValue> a{123};
        kz::unexpected<IntValue> b{a};
        REQUIRE(a.value() == 123);
        REQUIRE(b.value() == 123);
    }

    SECTION("move") {
        kz::unexpected<IntMoveableValue> a{42};
        kz::unexpected<IntMoveableValue> b{std::move(a)};
        REQUIRE(a.value() == 0);
        REQUIRE(b.value() == 42);
    }

    SECTION("copy with conversion") {
        const kz::unexpected<IntValue> a{123};
        kz::unexpected<LongValue> b{a};
        REQUIRE(a.value() == 123);
        REQUIRE(b.value() == 123l);
    }

    SECTION("move with conversion") {
        kz::unexpected<IntMoveableValue> a{69};
        kz::unexpected<LongMoveableValue> b{std::move(a)};
        REQUIRE(a.value() == 0);
        REQUIRE(b.value() == 69l);
    }

    SECTION("template argument deduction guide") {
        kz::unexpected a{IntValue{33}};
        REQUIRE(a.value() == 33);
    }
}

TEST_CASE("unexpected assignments", "[unexpected]") {

    SECTION("by value") {
        kz::unexpected<IntValue> a{10};
        kz::unexpected<IntValue> b{20};
        a = b;
        REQUIRE(a.value() == 20);
        REQUIRE(b.value() == 20);
    }

    SECTION("by move") {
        kz::unexpected<IntMoveableValue> a{10};
        kz::unexpected<IntMoveableValue> b{20};
        a = std::move(b);
        REQUIRE(a.value() == 20);
        REQUIRE(b.value() == 0);
    }

    SECTION("by value with conversion") {
        kz::unexpected<IntValue> a{11};
        kz::unexpected<LongValue> b{22};
        a = b;
        REQUIRE(a.value() == 22);
        REQUIRE(b.value() == 22l);
    }

    SECTION("by move with conversion") {
        kz::unexpected<IntMoveableValue> a{33};
        kz::unexpected<LongMoveableValue> b{44};
        a = std::move(b);
        REQUIRE(a.value() == 44);
        REQUIRE(b.value() == 0l);
    }
}

TEST_CASE("unexpected accessors", "[unexpected]") {

    SECTION("value() - has value") {
        // &
        kz::unexpected<IntValue> a{11};
        REQUIRE(a.value() == 11);

        // const&
        const kz::unexpected<IntValue> b{22};
        REQUIRE(b.value() == 22);

        // &&
        kz::unexpected<IntValue> c{33};
        REQUIRE(std::move(c).value() == 33);

        // const&&
        const kz::unexpected<IntValue> d{44};
        REQUIRE(std::move(d).value() == 44);
    }
}

TEST_CASE("unexpected swap", "[unexpected]") {

    SECTION("member swap()") {
        kz::unexpected<IntValue> a{3};
        kz::unexpected<IntValue> b{7};

        a.swap(b);

        REQUIRE(a.value() == 7);
        REQUIRE(b.value() == 3);
    }

    SECTION("std::swap") {
        kz::unexpected<IntValue> a{100};
        kz::unexpected<IntValue> b{200};

        using std::swap;
        swap(a, b);

        REQUIRE(a.value() == 200);
        REQUIRE(b.value() == 100);
    }
}

TEST_CASE("unexpected comparisons", "[unexpected]") {

    SECTION("==") {
        kz::unexpected<IntValue> a{44};
        kz::unexpected<IntValue> b{44};
        kz::unexpected<IntValue> c{55};

        REQUIRE(a == b);
        REQUIRE(!(a == c));
    }

    SECTION("!=") {
        kz::unexpected<IntValue> a{66};
        kz::unexpected<IntValue> b{66};
        kz::unexpected<IntValue> c{77};

        REQUIRE(a != c);
        REQUIRE(!(a != b));
    }
}