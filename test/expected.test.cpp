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
    static_assert(std::is_same_v<T::unexpected_type, std::unexpected<bool>>);

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

TEST_CASE("is_specialization<>", "[expected]") {
    using T1 = int;
    using T2 = std::vector<int>;
    using T3 = std::unexpected<int>;

    static_assert(!kz::detail::is_specialization<T1, std::unexpected>::value);
    static_assert(!kz::detail::is_specialization<T2, std::unexpected>::value);
    static_assert(kz::detail::is_specialization<T3, std::unexpected>::value);
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

TEST_CASE("Construct using in_place_t and initializer_list") {
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
        std::expected<int, Error> a{123};
        std::expected<int, Error> b{std::unexpected(Error::FlyingSquirrels)};
        REQUIRE(a);
        REQUIRE(a.has_value());
        REQUIRE(!b);
        REQUIRE(!b.has_value());

        std::expected<void, Error> c;
        std::expected<void, Error> d{std::unexpected(Error::FlyingSquirrels)};
        REQUIRE(c);
        REQUIRE(c.has_value());
        REQUIRE(!d);
        REQUIRE(!d.has_value());
    }

    SECTION("operator->()") {
        std::expected<CopyConstructible, Error> a{11};
        REQUIRE(a->value == 11);

        *a.operator->() = 22;
        REQUIRE(a->value == 22);

        const std::expected<CopyConstructible, Error> b{33};
        REQUIRE(b->value == 33);
    }

    SECTION("operator*()") {
        // &
        std::expected<int, Error> a{11};
        REQUIRE(*a == 11);

        // const&
        const std::expected<int, Error> b{22};
        REQUIRE(*b == 22);

        // &&
        std::expected<int, Error> c{33};
        REQUIRE(*std::move(c) == 33);

        // const&&
        const std::expected<int, Error> d{44};
        REQUIRE(*std::move(d) == 44);
    }

    SECTION("value() - has value") {
        // &
        std::expected<int, Error> a{11};
        REQUIRE(a.value() == 11);

        // const&
        const std::expected<int, Error> b{22};
        REQUIRE(b.value() == 22);

        // &&
        std::expected<int, Error> c{33};
        REQUIRE(std::move(c).value() == 33);

        // const&&
        const std::expected<int, Error> d{44};
        REQUIRE(std::move(d).value() == 44);
    }

#if KZ_EXCEPTIONS
    SECTION("value() - has error") {
        // &
        std::expected<int, Error> a{std::unexpected(Error::FileNotFound)};
        REQUIRE_THROWS_AS(a.value(), std::bad_expected_access<Error>);

        // const&
        const std::expected<int, Error> b{std::unexpected(Error::FileNotFound)};
        REQUIRE_THROWS_AS(b.value(), std::bad_expected_access<Error>);

        // &&
        std::expected<int, Error> c{std::unexpected(Error::FileNotFound)};
        REQUIRE_THROWS_AS(
            std::move(c).value(), std::bad_expected_access<Error>);

        // const&&
        const std::expected<int, Error> d{std::unexpected(Error::FileNotFound)};
        REQUIRE_THROWS_AS(
            std::move(d).value(), std::bad_expected_access<Error>);
    }
#endif

    SECTION("error()") {
        // &
        std::expected<int, int> a{std::unexpected{11}};
        REQUIRE(a.error() == 11);

        // const&
        const std::expected<int, int> b{std::unexpected{22}};
        REQUIRE(b.error() == 22);

        // &&
        std::expected<int, int> c{std::unexpected{33}};
        REQUIRE(std::move(c).error() == 33);

        // const&&
        const std::expected<int, int> d{std::unexpected{44}};
        REQUIRE(std::move(d).error() == 44);
    }

    SECTION("value_or()") {
        // value - const&
        const std::expected<int, Error> a{31};
        REQUIRE(a.value_or(42) == 31);

        // error - const&
        const std::expected<int, Error> b{std::unexpected(Error::IOError)};
        REQUIRE(b.value_or(42) == 42);

        // value - &&
        std::expected<int, Error> c{69};
        REQUIRE(std::move(c).value_or(777) == 69);

        // error - &&
        std::expected<int, Error> d{std::unexpected(Error::FileNotFound)};
        REQUIRE(std::move(d).value_or(84) == 84);
    }

    SECTION("error_or()") {
        // value - const&
        const std::expected<int, Error> a{31};
        REQUIRE(a.error_or(Error::FlyingSquirrels) == Error::FlyingSquirrels);

        // error - const&
        const std::expected<int, Error> b{std::unexpected(Error::IOError)};
        REQUIRE(b.error_or(Error::FlyingSquirrels) == Error::IOError);

        // value - &&
        std::expected<int, Error> c{69};
        REQUIRE(std::move(c).error_or(Error::FlyingSquirrels) ==
                Error::FlyingSquirrels);

        // error - &&
        std::expected<int, Error> d{std::unexpected(Error::FileNotFound)};
        REQUIRE(std::move(d).error_or(Error::FlyingSquirrels) ==
                Error::FileNotFound);
    }

    SECTION("and_then()") {
        const std::expected<int, Error> a{12};
        const auto a2 = a.and_then([](int value) -> std::expected<bool, Error> {
            return value == 12;
        });
        REQUIRE(a2 == true);
    }

    SECTION("or_else()") {
        const std::expected<int, Error> a{std::unexpected(Error::IOError)};
        const auto a2 =
            a.or_else([](Error error) -> std::expected<bool, Error> {
                return std::unexpected(error);
            });
        REQUIRE(a2.error() == Error::IOError);
    }

    SECTION("transform()") {

        // Function with return value
        const std::expected<int, Error> a{10};
        const auto a2 = a.transform([](int x) -> int { return x + 1; });
        REQUIRE(a2 == 11);

        // void function
        bool called = false;
        a.transform([&called](int) -> void { called = true; });
        REQUIRE(called);

        // Error
        const std::expected<int, Error> b{std::unexpected(Error::IOError)};
        const auto b2 = b.transform([](int x) -> int { return x + 1; });
        REQUIRE(b2.error() == Error::IOError);
    }

    SECTION("transform_error()") {
        // Value
        const std::expected<int, Error> a{60};
        const auto a2 = a.transform_error(
            [](Error) -> Error { return Error::FlyingSquirrels; });
        REQUIRE(a2);

        // Error
        const std::expected<int, Error> b{std::unexpected(Error::IOError)};
        const auto b2 = b.transform_error(
            [](Error) -> Error { return Error::FlyingSquirrels; });
        REQUIRE(b2.error() == Error::FlyingSquirrels);
    }

    SECTION("transform() - void specialization") {

        // Function with return value
        const std::expected<void, Error> a{};
        const auto a2 = a.transform([]() {});
        REQUIRE(a2);

        // void function
        bool called = false;
        a.transform([&called]() -> void { called = true; });
        REQUIRE(called);

        // Error
        const std::expected<void, Error> b{std::unexpected(Error::IOError)};
        const auto b2 = b.transform([]() -> int { return 7; });
        REQUIRE(b2.error() == Error::IOError);
    }

    SECTION("transform_error() - void specialization") {
        // Value
        const std::expected<void, Error> a{};
        const auto a2 = a.transform_error(
            [](Error) -> Error { return Error::FlyingSquirrels; });
        REQUIRE(a2);

        // Error
        const std::expected<void, Error> b{std::unexpected(Error::IOError)};
        const auto b2 = b.transform_error(
            [](Error) -> Error { return Error::FlyingSquirrels; });
        REQUIRE(b2.error() == Error::FlyingSquirrels);
    }
}
