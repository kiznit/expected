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

#if !defined(KZ_EXCEPTIONS)
// Determine if exceptions are enabled
#if __cpp_exceptions >= 199711l
#define KZ_EXCEPTIONS 1
#elif __EXCEPTIONS
#define KZ_EXCEPTIONS 1
#elif _CPPUNWIND
#define KZ_EXCEPTIONS 1
#endif
#endif

#if KZ_EXCEPTIONS

#include <exception>

namespace kz {

    template <class E>
    class bad_expected_access;

    // �.�.7 Class bad_expected_access<void> [expected.bad_expected_access_base]
    template <>
    class bad_expected_access<void> : public std::exception {
    public:
        explicit bad_expected_access() {}
        const char* what() const noexcept override {
            return "kz::bad_expected_access<>";
        }
    };

    // �.�.6 Template Class bad_expected_access [expected.bad_expected_access]
    template <class E>
    class bad_expected_access : public bad_expected_access<void> {
    public:
        explicit bad_expected_access(E e) : _error(e) {}
        E& error() & { return _error; }
        const E& error() const& { return _error; }
        E&& error() && { return std::move(_error); }
        const E&& error() const&& { return std::move(_error); }

    private:
        E _error;
    };

} // namespace kz

#endif
