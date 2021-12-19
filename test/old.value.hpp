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
#include <vector>

template <typename T>
struct Value {
    using value_type = T;

    Value(T x) noexcept : value(x) {}
    Value(std::initializer_list<T> list) noexcept : value(*list.begin()) {}
    template <typename U>
    Value(const Value<U>& x) noexcept : value(x.value) {}

    ~Value() { value = 0; }

    T value;

protected:
    Value() {}
};

template <typename T>
inline bool operator==(const Value<T>& x, const Value<T>& y) {
    return x.value == y;
}

template <typename T>
inline bool operator==(const Value<T>& x, T y) {
    return x.value == y;
}

template <typename T>
struct MoveableValue {
    using value_type = T;

    MoveableValue(T x) : value(x) {}
    MoveableValue(MoveableValue<T>&& x)
        : value(x.value), throwsOnMove(x.throwsOnMove) {
#if KZ_EXCEPTIONS
        if (throwsOnMove)
            throw std::exception();
#endif
        x.value = T(0);
    }
    template <typename U>
    MoveableValue(MoveableValue<U>&& x) : value(x.value) {
        x.value = T(0);
    }
    constexpr MoveableValue& operator=(MoveableValue&& x) {
        value = x.value;
        x.value = 0;
        return *this;
    }

    // Can't be copied
    MoveableValue(const MoveableValue&) = delete;
    MoveableValue& operator=(const MoveableValue&) = delete;

    ~MoveableValue() { value = 0; }

    T value;
    bool throwsOnMove{false};
};

template <typename T>
inline bool operator==(const MoveableValue<T>& x, T y) {
    return x.value == y;
}

template <typename T>
struct Default : Value<T> {
    constexpr static T DefaultValue = -100;

    Default() { this->value = DefaultValue; }
};

template <typename T>
inline bool operator==(const Default<T>& x, T y) {
    return x.value == y;
}

using IntValue = Value<int>;
using LongValue = Value<long>;
using IntMoveableValue = MoveableValue<int>;
using LongMoveableValue = MoveableValue<long>;
using DefaultInt = Default<int>;

struct ComplexThing {
    ComplexThing(const IntValue& a, IntMoveableValue&& b)
        : a(a), b(std::move(b)) {}

    ComplexThing(std::vector<int> list, const IntValue& a, IntMoveableValue&& b)
        : list(list), a(a), b(std::move(b)) {}

    std::vector<int> list;
    IntValue a;
    IntMoveableValue b;
};

struct AssignableComplexThing : ComplexThing {

    template <typename... Args>
    requires(sizeof...(Args) > 1) AssignableComplexThing(Args&&... args)
        : ComplexThing(std::forward<Args>(args)...) {}

    AssignableComplexThing(const AssignableComplexThing& other)
        : ComplexThing(other.list, other.a, IntMoveableValue(other.b.value)),
          throwsOnCopy(other.throwsOnCopy) {
#if KZ_EXCEPTIONS
        if (throwsOnCopy)
            throw std::exception();
#endif
    }

    AssignableComplexThing& operator=(const AssignableComplexThing& other) {
        list = other.list;
        a = other.a;
        b = other.b.value;
        return *this;
    }

    bool throwsOnCopy{false};
};

struct NotNoThrowConstructible {

    explicit NotNoThrowConstructible(int x) : value(x) {}
    explicit NotNoThrowConstructible(const NotNoThrowConstructible& other)
        : value(other.value) {}

    NotNoThrowConstructible& operator=(
        const NotNoThrowConstructible&) = default;

    int value;
};