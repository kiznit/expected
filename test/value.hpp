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

struct DefaultConstructible {
    constexpr static int DefaultValue = -100;
    DefaultConstructible() : value(DefaultValue) {}
    int value;
};
static_assert(std::is_default_constructible_v<DefaultConstructible>);

struct NotDefaultConstructible {
    NotDefaultConstructible(int x) : value(x) {}
    int value;
};
static_assert(!std::is_default_constructible_v<NotDefaultConstructible>);

struct CopyConstructible {
    CopyConstructible(int x) : value(x) {}
    int value;
};
static_assert(std::is_copy_constructible_v<CopyConstructible>);

struct NotCopyConstructible {
    NotCopyConstructible(int x) : value(x) {}
    NotCopyConstructible(const NotCopyConstructible&) = delete;
    int value;
};
static_assert(!std::is_copy_constructible_v<NotCopyConstructible>);

struct MoveConstructible {
    MoveConstructible(int x) : value(x) {}
    MoveConstructible(MoveConstructible&& rhs) {
        value = rhs.value;
        rhs.value = -1;
    }
    int value;
};
static_assert(std::is_move_constructible_v<MoveConstructible>);

struct NotMoveConstructible {
    NotMoveConstructible(int x) : value(x) {}
    NotMoveConstructible(NotMoveConstructible&& rhs) = delete;
    int value;
};
static_assert(!std::is_move_constructible_v<NotMoveConstructible>);
