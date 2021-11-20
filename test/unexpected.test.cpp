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

TEST_CASE("unexpected constructors", "[unexpected]") {

    SECTION("from value") {
        mtl::unexpected<IntValue> a{123};
        REQUIRE(a.value() == 123);
    }

    SECTION("in-place with value") {
        mtl::unexpected<IntValue> a{std::in_place, 123};
        REQUIRE(a.value() == 123);
    }

    SECTION("in-place with multiple parameters") {
        const IntValue a{100};
        IntMoveableValue b{200};
        mtl::unexpected<ComplexThing> c{std::in_place, a, std::move(b)};

        REQUIRE(c.value().a == 100);
        REQUIRE(c.value().b == 200);
        REQUIRE(a == 100);
        REQUIRE(b == 0);
    }

    SECTION("in-place with initializer list") {
        mtl::unexpected<std::vector<int>> a{std::in_place, {1, 2, 3}};
        REQUIRE(a.value().size() == 3);
    }

    SECTION("in-place with initializer list and extra parameters") {
        const IntValue a{100};
        IntMoveableValue b{200};
        mtl::unexpected<ComplexThing> c{
            std::in_place, {1, 2, 3}, a, std::move(b)};
        REQUIRE(c.value().a == 100);
        REQUIRE(c.value().b == 200);
        REQUIRE(c.value().list.size() == 3);
        REQUIRE(a == 100);
        REQUIRE(b == 0);
    }

    SECTION("copy") {

        const mtl::unexpected<IntValue> a{123};
        mtl::unexpected<IntValue> b{a};
        REQUIRE(a.value() == 123);
        REQUIRE(b.value() == 123);
    }

    SECTION("move") {
        mtl::unexpected<IntMoveableValue> a{42};
        mtl::unexpected<IntMoveableValue> b{std::move(a)};
        REQUIRE(a.value() == 0);
        REQUIRE(b.value() == 42);
    }

    SECTION("copy with conversion") {
        const mtl::unexpected<IntValue> a{123};
        mtl::unexpected<LongValue> b{a};
        REQUIRE(a.value() == 123);
        REQUIRE(b.value() == 123l);
    }

    SECTION("move with conversion") {
        mtl::unexpected<IntMoveableValue> a{69};
        mtl::unexpected<LongMoveableValue> b{std::move(a)};
        REQUIRE(a.value() == 0);
        REQUIRE(b.value() == 69l);
    }

    SECTION("template argument deduction guide") {
        mtl::unexpected a{IntValue{33}};
        REQUIRE(a.value() == 33);
    }
}

TEST_CASE("unexpected assignments", "[unexpected]") {

    SECTION("by value") {
        mtl::unexpected<IntValue> a{10};
        mtl::unexpected<IntValue> b{20};
        a = b;
        REQUIRE(a.value() == 20);
        REQUIRE(b.value() == 20);
    }

    SECTION("by move") {
        mtl::unexpected<IntMoveableValue> a{10};
        mtl::unexpected<IntMoveableValue> b{20};
        a = std::move(b);
        REQUIRE(a.value() == 20);
        REQUIRE(b.value() == 0);
    }

    SECTION("by value with conversion") {
        mtl::unexpected<IntValue> a{11};
        mtl::unexpected<LongValue> b{22};
        a = b;
        REQUIRE(a.value() == 22);
        REQUIRE(b.value() == 22l);
    }

    SECTION("by move with conversion") {
        mtl::unexpected<IntMoveableValue> a{33};
        mtl::unexpected<LongMoveableValue> b{44};
        a = std::move(b);
        REQUIRE(a.value() == 44);
        REQUIRE(b.value() == 0l);
    }
}

TEST_CASE("unexpected accessors", "[unexpected]") {

    SECTION("value() - has value") {
        // &
        mtl::unexpected<IntValue> a{11};
        REQUIRE(a.value() == 11);

        // const&
        const mtl::unexpected<IntValue> b{22};
        REQUIRE(b.value() == 22);

        // &&
        mtl::unexpected<IntValue> c{33};
        REQUIRE(std::move(c).value() == 33);

        // const&&
        const mtl::unexpected<IntValue> d{44};
        REQUIRE(std::move(d).value() == 44);
    }
}

TEST_CASE("unexpected swap", "[unexpected]") {

    SECTION("member swap()") {
        mtl::unexpected<IntValue> a{3};
        mtl::unexpected<IntValue> b{7};

        a.swap(b);

        REQUIRE(a.value() == 7);
        REQUIRE(b.value() == 3);
    }

    SECTION("std::swap") {
        mtl::unexpected<IntValue> a{100};
        mtl::unexpected<IntValue> b{200};

        using std::swap;
        swap(a, b);

        REQUIRE(a.value() == 200);
        REQUIRE(b.value() == 100);
    }
}

TEST_CASE("unexpected comparisons", "[unexpected]") {

    SECTION("==") {
        mtl::unexpected<IntValue> a{44};
        mtl::unexpected<IntValue> b{44};
        mtl::unexpected<IntValue> c{55};

        REQUIRE(a == b);
        REQUIRE(!(a == c));
    }

    SECTION("!=") {
        mtl::unexpected<IntValue> a{66};
        mtl::unexpected<IntValue> b{66};
        mtl::unexpected<IntValue> c{77};

        REQUIRE(a != c);
        REQUIRE(!(a != b));
    }
}