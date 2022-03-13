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

#include <kz/expected_bits/unexpected.hpp>
#include <catch2/catch.hpp>
#include "value.hpp"

TEST_CASE("unexpected static assertions", "[unexpected]") {
    using T = kz::unexpected<int>;

    static_assert(!std::is_default_constructible_v<T>);
    static_assert(std::is_trivially_copyable_v<T>);
    static_assert(std::is_trivially_move_constructible_v<T>);
}

TEST_CASE("unexpected constructors", "[unexpected]") {
    SECTION("from value") {
        kz::unexpected<int> a{123};
        REQUIRE(a.value() == 123);
    }

    SECTION("in-place with value") {
        kz::unexpected<int> a{std::in_place, 123};
        REQUIRE(a.value() == 123);
    }

    SECTION("in-place with multiple parameters") {
        const CopyConstructible a{100};
        MoveConstructible b{200};
        kz::unexpected<ComplexThing> c{std::in_place, a, std::move(b)};

        REQUIRE(c.value().a.value == 100);
        REQUIRE(c.value().b.value == 200);
        REQUIRE(c.value().list.empty());
        REQUIRE(a.value == 100);
        REQUIRE(b.value == -1);
    }

    SECTION("in-place with initializer list") {
        kz::unexpected<std::vector<int>> a{std::in_place, {1, 2, 3}};
        REQUIRE(a.value().size() == 3);
    }

    SECTION("in-place with initializer list and extra parameters") {
        const CopyConstructible a{100};
        MoveConstructible b{200};
        kz::unexpected<ComplexThing> c{
            std::in_place, {1, 2, 3}, a, std::move(b)};
        REQUIRE(c.value().a.value == 100);
        REQUIRE(c.value().b.value == 200);
        REQUIRE(c.value().list.size() == 3);
        REQUIRE(a.value == 100);
        REQUIRE(b.value == -1);
    }

    SECTION("copy") {

        const kz::unexpected<int> a{123};
        kz::unexpected<int> b{a};
        REQUIRE(a.value() == 123);
        REQUIRE(b.value() == 123);
    }

    SECTION("move") {
        kz::unexpected<MoveConstructible> a{42};
        kz::unexpected<MoveConstructible> b{std::move(a)};
        REQUIRE(a.value().value == -1);
        REQUIRE(b.value().value == 42);
    }

    SECTION("template argument deduction guide") {
        kz::unexpected a{int{33}};
        REQUIRE(a.value() == 33);
    }
}

TEST_CASE("unexpected assignments", "[unexpected]") {

    SECTION("by value") {
        kz::unexpected<int> a{10};
        kz::unexpected<int> b{20};
        a = b;
        REQUIRE(a.value() == 20);
        REQUIRE(b.value() == 20);
    }

    SECTION("by move") {
        kz::unexpected<MoveAssignable> a{10};
        kz::unexpected<MoveAssignable> b{20};
        a = std::move(b);
        REQUIRE(a.value().value == 20);
        REQUIRE(b.value().value == -1);
    }
}

TEST_CASE("unexpected accessors", "[unexpected]") {

    SECTION("value() - has value") {
        // &
        kz::unexpected<int> a{11};
        REQUIRE(a.value() == 11);

        // const&
        const kz::unexpected<int> b{22};
        REQUIRE(b.value() == 22);

        // &&
        kz::unexpected<int> c{33};
        REQUIRE(std::move(c).value() == 33);

        // const&&
        const kz::unexpected<int> d{44};
        REQUIRE(std::move(d).value() == 44);
    }
}

TEST_CASE("unexpected swap", "[unexpected]") {

    SECTION("member swap()") {
        kz::unexpected<int> a{3};
        kz::unexpected<int> b{7};

        a.swap(b);

        REQUIRE(a.value() == 7);
        REQUIRE(b.value() == 3);
    }

    SECTION("std::swap") {
        kz::unexpected<int> a{100};
        kz::unexpected<int> b{200};

        using std::swap;
        swap(a, b);

        REQUIRE(a.value() == 200);
        REQUIRE(b.value() == 100);
    }
}

TEST_CASE("unexpected comparisons", "[unexpected]") {

    SECTION("==") {
        kz::unexpected<int> a{44};
        kz::unexpected<int> b{44};
        kz::unexpected<int> c{55};

        REQUIRE(a == b);
        REQUIRE(!(a == c));
    }

    SECTION("!=") {
        kz::unexpected<int> a{66};
        kz::unexpected<int> b{66};
        kz::unexpected<int> c{77};

        REQUIRE(a != c);
        REQUIRE(!(a != b));
    }
}
