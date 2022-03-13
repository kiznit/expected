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
#include "value.hpp"

enum class Error { FileNotFound, IOError, FlyingSquirrels };

TEST_CASE("static assertions", "[expected]") {
    using T = std::expected<short, bool>;
    static_assert(std::is_same_v<T::value_type, short>);
    static_assert(std::is_same_v<T::error_type, bool>);
    static_assert(std::is_same_v<T::unexpected_type, kz::unexpected<bool>>);

    using U = std::expected<void, bool>;
    static_assert(std::is_void_v<U::value_type>);
}

TEST_CASE("rebind<>", "[expected]") {
    using T = std::expected<short, bool>;
    using U = T::rebind<long>;
    static_assert(std::is_same_v<U::value_type, long>);
    static_assert(std::is_same_v<U::error_type, bool>);

    using V = std::expected<void, bool>;
    using W = T::rebind<long>;
    static_assert(std::is_same_v<V::value_type, void>);
    static_assert(std::is_same_v<W::error_type, bool>);
}

// TODO: how do we speciailize cv void? do we need to do anything?

TEST_CASE("Default constructor", "[expected]") {
    SECTION("T is default-constructible") {
        using Type = std::expected<DefaultConstructible, Error>;
        static_assert(std::is_default_constructible_v<Type>);

        Type a;

        REQUIRE(a);
        REQUIRE(a->value == DefaultConstructible::DefaultValue);
    }

    // SECTION("T is not default-constructible") {
    //     using Type = std::expected<NotDefaultConstructible, Error>;
    //     static_assert(!std::is_constructible_v<Type>);
    // }

    SECTION("T is void") {
        using Type = std::expected<void, Error>;
        static_assert(std::is_default_constructible_v<Type>);

        Type a;

        REQUIRE(a);
    }
}

TEST_CASE("Copy constructor") {
    SECTION("T and E are copy-constructible") {
        using Type = std::expected<CopyConstructible, CopyConstructible>;
        static_assert(std::is_copy_constructible_v<Type>);

        const Type a(42);
        const Type b(a);

        REQUIRE(b);
        REQUIRE(b->value == 42);
    }

    // TODO: test with error

    SECTION("T is void and E is copy-constructible") {
        using Type = std::expected<void, CopyConstructible>;
        static_assert(std::is_copy_constructible_v<Type>);

        const Type a;
        const Type b(a);

        REQUIRE(b);
    }

    // TODO: test with error

    SECTION("T or E is not copy-constructible") {
        using Type1 = std::expected<NotCopyConstructible, CopyConstructible>;
        using Type2 = std::expected<CopyConstructible, NotCopyConstructible>;
        using Type3 = std::expected<NotCopyConstructible, NotCopyConstructible>;
        using Type4 = std::expected<void, NotCopyConstructible>;
        static_assert(!std::is_copy_constructible_v<Type1>);
        static_assert(!std::is_copy_constructible_v<Type2>);
        static_assert(!std::is_copy_constructible_v<Type3>);
        static_assert(!std::is_copy_constructible_v<Type4>);
    }

#if KZ_P0848R3
    SECTION("T and E are trivially-copy-constructible") {
        using Type1 = std::expected<int, Error>;
        static_assert(std::is_copy_constructible_v<Type1>);
        static_assert(std::is_trivially_copy_constructible_v<Type1>);

        using Type2 = std::expected<void, Error>;
        static_assert(std::is_copy_constructible_v<Type2>);
        static_assert(std::is_trivially_copy_constructible_v<Type2>);
    }

    SECTION("T or E is not trivially-copy-constructible") {
        using Type1 = std::expected<std::vector<int>, Error>;
        static_assert(std::is_copy_constructible_v<Type1>);
        static_assert(!std::is_trivially_copy_constructible_v<Type1>);

        using Type2 = std::expected<int, std::vector<int>>;
        static_assert(std::is_copy_constructible_v<Type2>);
        static_assert(!std::is_trivially_copy_constructible_v<Type2>);

        using Type3 = std::expected<void, std::vector<int>>;
        static_assert(std::is_copy_constructible_v<Type3>);
        static_assert(!std::is_trivially_copy_constructible_v<Type3>);
    }
#endif
}

TEST_CASE("Move constructor") {
    SECTION("T and E are move-constructible") {
        using Type = std::expected<MoveConstructible, MoveConstructible>;
        static_assert(std::is_move_constructible_v<Type>);
        Type a(42);
        const Type b(std::move(a));

        REQUIRE(b);
        REQUIRE(b->value == 42);
    }

    // TODO: test with error

    SECTION("T is void and E is move-constructible") {
        using Type = std::expected<void, MoveConstructible>;
        static_assert(std::is_move_constructible_v<Type>);
        Type a;
        const Type b(std::move(a));

        REQUIRE(b);
    }

    // TODO: test with error

    SECTION("T or E is not move-constructible") {
        using Type1 = std::expected<NotMoveConstructible, MoveConstructible>;
        using Type2 = std::expected<MoveConstructible, NotMoveConstructible>;
        using Type3 = std::expected<NotMoveConstructible, NotMoveConstructible>;
        using Type4 = std::expected<void, NotMoveConstructible>;
        static_assert(!std::is_move_constructible_v<Type1>);
        static_assert(!std::is_move_constructible_v<Type2>);
        static_assert(!std::is_move_constructible_v<Type3>);
        static_assert(!std::is_move_constructible_v<Type4>);
    }

#if KZ_P0848R3
    SECTION("T and E are trivially-move-constructible") {
        using Type1 = std::expected<int, Error>;
        static_assert(std::is_move_constructible_v<Type1>);
        static_assert(std::is_trivially_move_constructible_v<Type1>);

        using Type2 = std::expected<void, Error>;
        static_assert(std::is_move_constructible_v<Type2>);
        static_assert(std::is_trivially_move_constructible_v<Type2>);
    }

    SECTION("T or E is not trivially-move-constructible") {
        using Type1 = std::expected<std::vector<int>, Error>;
        static_assert(std::is_move_constructible_v<Type1>);
        static_assert(!std::is_trivially_move_constructible_v<Type1>);

        using Type2 = std::expected<int, std::vector<int>>;
        static_assert(std::is_move_constructible_v<Type2>);
        static_assert(!std::is_trivially_move_constructible_v<Type2>);

        using Type3 = std::expected<void, std::vector<int>>;
        static_assert(std::is_move_constructible_v<Type3>);
        static_assert(!std::is_trivially_move_constructible_v<Type3>);
    }
#endif
}

TEST_CASE("Conversion copy constructor") {
    SECTION("Convert U to T") {
        using Type1 = std::expected<int, Error>;
        using Type2 = std::expected<long, Error>;

        static_assert(std::is_constructible_v<Type2::value_type,
            const Type1::value_type&>);

        // TODO: need a custom type for test
        // static_assert(!std::is_constructible_v<Type2::value_type,
        //               const Type1::value_type&&>);

        const Type1 a(99);
        const Type2 b(a);

        REQUIRE(b);
        REQUIRE(*b == 99);
    }

    SECTION("Convert void to void") {
        // TODO: need custom copyable and not moveable type for this test
        using Type1 = std::expected<void, int>;
        using Type2 = std::expected<void, long>;

        const Type1 a;
        const Type2 b(a);

        REQUIRE(b);
    }

    // TODO: convert G to E

    // TODO: void convert G to E
}

TEST_CASE("Conversion move constructor") {
    SECTION("Move U to T") {
        using Type1 = std::expected<int, Error>;
        using Type2 = std::expected<long, Error>;

        // TODO: need a custom type for test
        // static_assert(!std::is_constructible_v<Type2::value_type,
        //               const Type1::value_type&>);

        static_assert(std::is_constructible_v<Type2::value_type,
            const Type1::value_type&&>);

        Type1 a(32);
        const Type2 b(std::move(a));

        // TODO: need to verify source was moved
        // REQUIRE(a);
        // REQUIRE(*a == 0);
        REQUIRE(b);
        REQUIRE(*b == 32);
    }

    SECTION("Move void to void") {
        // TODO: need custom moveable and not copyable type for this test
        using Type1 = std::expected<void, int>;
        using Type2 = std::expected<void, long>;

        Type1 a;
        const Type2 b(std::move(a));

        REQUIRE(b);
    }

    // TODO: move G to E

    // TODO: void move G to E
}

TEST_CASE("Construct from value") {
    SECTION("Copy a value, same type") {
        using Type = std::expected<std::vector<int>, Error>;
        static_assert(std::is_copy_constructible_v<Type::value_type>);
        static_assert(std::is_move_constructible_v<Type::value_type>);

        const std::vector value{1, 2, 3};
        const Type a(value);

        REQUIRE(a.value() == value);
    }

    SECTION("Copy a value, different type") {
        // TODO
    }

    SECTION("Move a value, same type") {
        using Type = std::expected<std::vector<int>, Error>;
        static_assert(std::is_copy_constructible_v<Type::value_type>);
        static_assert(std::is_move_constructible_v<Type::value_type>);

        std::vector value{1, 2, 3};
        const Type a(std::move(value));

        REQUIRE(value.empty());
        REQUIRE(a->size() == 3);
    }

    SECTION("Move a value, different type") {
        // TODO
    }
}

TEST_CASE("Construct using unexpected") {
    // TODO: tests
}

TEST_CASE("Construct using in_place_t") {
    SECTION("simple value") {
        using Type = std::expected<int, Error>;
        Type a(std::in_place, 312);

        REQUIRE(a);
        REQUIRE(*a == 312);
    }

    SECTION("void value") {
        using Type = std::expected<void, Error>;
        Type a(std::in_place);

        REQUIRE(a);
    }

    // TODO: more tests
}

TEST_CASE("Constrcut using in_place_t and initializer_list") {
    SECTION("no extra params") {
        using Type = std::expected<std::vector<int>, Error>;
        Type a(std::in_place, {1, 2, 3, 4});

        REQUIRE(a);
        REQUIRE(*a == std::vector<int>{1, 2, 3, 4});
    }

    // TODO: more tests, including with extra params
}

TEST_CASE("Construct using unexpect_t") {
    SECTION("simple value") {
        using Type = std::expected<int, Error>;
        Type a(std::unexpect, Error::FlyingSquirrels);

        REQUIRE(!a);
        REQUIRE(a.error() == Error::FlyingSquirrels);
    }

    SECTION("void value") {
        using Type = std::expected<void, Error>;
        Type a(std::unexpect, Error::IOError);

        REQUIRE(!a);
        REQUIRE(a.error() == Error::IOError);
    }

    // TODO: more tests
}

TEST_CASE("Construct using unexpect_t and initializer_list") {
    SECTION("no extra params") {
        using Type = std::expected<int, std::vector<int>>;
        Type a(std::unexpect, {1, 2, 3, 4});

        REQUIRE(!a);
        REQUIRE(a.error() == std::vector<int>{1, 2, 3, 4});
    }

    // TODO: more tests, including with extra params
}

TEST_CASE("Destructor", "[expected]") {
    // TODO: verify destructors for T and E are called

#if KZ_P0848R3
    SECTION("trivial destructor") {
        using Type1 = std::expected<int, Error>;
        using Type2 = std::expected<void, Error>;
        using Type3 = std::expected<std::vector<int>, Error>;
        using Type4 = std::expected<int, std::vector<int>>;
        using Type5 = std::expected<void, std::vector<int>>;

        static_assert(std::is_trivially_destructible_v<Type1>);
        static_assert(std::is_trivially_destructible_v<Type2>);
        static_assert(!std::is_trivially_destructible_v<Type3>);
        static_assert(!std::is_trivially_destructible_v<Type4>);
        static_assert(!std::is_trivially_destructible_v<Type5>);
    }
#endif
}

TEST_CASE("observers", "[expected]") {

    SECTION("operator bool / has_value()") {
        kz::expected<int, Error> a{123};
        kz::expected<int, Error> b{kz::unexpected(Error::FlyingSquirrels)};
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
        kz::expected<CopyConstructible, Error> a{11};
        REQUIRE(a->value == 11);

        *a.operator->() = 22;
        REQUIRE(a->value == 22);

        const kz::expected<CopyConstructible, Error> b{33};
        REQUIRE(b->value == 33);
    }

    SECTION("operator*()") {
        // &
        kz::expected<int, Error> a{11};
        REQUIRE(*a == 11);

        // const&
        const kz::expected<int, Error> b{22};
        REQUIRE(*b == 22);

        // &&
        kz::expected<int, Error> c{33};
        REQUIRE(*std::move(c) == 33);

        // const&&
        const kz::expected<int, Error> d{44};
        REQUIRE(*std::move(d) == 44);
    }

    SECTION("value() - has value") {
        // &
        kz::expected<int, Error> a{11};
        REQUIRE(a.value() == 11);

        // const&
        const kz::expected<int, Error> b{22};
        REQUIRE(b.value() == 22);

        // &&
        kz::expected<int, Error> c{33};
        REQUIRE(std::move(c).value() == 33);

        // const&&
        const kz::expected<int, Error> d{44};
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
        kz::expected<int, int> a{kz::unexpected{11}};
        REQUIRE(a.error() == 11);

        // const&
        const kz::expected<int, int> b{kz::unexpected{22}};
        REQUIRE(b.error() == 22);

        // &&
        kz::expected<int, int> c{kz::unexpected{33}};
        REQUIRE(std::move(c).error() == 33);

        // const&&
        const kz::expected<int, int> d{kz::unexpected{44}};
        REQUIRE(std::move(d).error() == 44);
    }

    SECTION("value_or()") {
        // value - const&
        const kz::expected<int, Error> a{31};
        REQUIRE(a.value_or(42) == 31);

        // error - const&
        const kz::expected<int, Error> b{kz::unexpected(Error::IOError)};
        REQUIRE(b.value_or(42) == 42);

        // value - &&
        kz::expected<int, Error> c{69};
        REQUIRE(std::move(c).value_or(777) == 69);

        // error - &&
        kz::expected<int, Error> d{kz::unexpected(Error::FileNotFound)};
        REQUIRE(std::move(d).value_or(84) == 84);
    }
}
