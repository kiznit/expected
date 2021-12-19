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

#include <kz/expected_bits/unexpected.hpp>

namespace kz {

    // �.�.8 unexpect tag [expected.unexpect]
    struct unexpect_t {};
    inline constexpr unexpect_t unexpect;

    template <typename T, typename E>
    requires(!std::is_same_v<T, unexpected<E>>) class expected;

    namespace detail {

        struct no_init_t {};
        inline constexpr no_init_t no_init;

        // expected_base
        template <typename T, typename E,
            bool value_is_trivially_destructible =
                std::is_trivially_destructible_v<T>,
            bool error_is_trivially_destructible =
                std::is_trivially_destructible_v<E>>
        struct expected_base {

            constexpr expected_base(no_init_t)
                : _has_value(false) {} // OK, E is trivially destructible

            template <typename = void>
            requires(
                std::is_default_constructible_v<T>) constexpr expected_base()
                : _value(), _has_value(true) {}

            template <typename... Args>
            constexpr expected_base(in_place_t, Args&&... args)
                : _value(std::forward<Args>(args)...), _has_value(true) {}

            template <typename... Args>
            constexpr expected_base(unexpect_t, Args&&... args)
                : _error(std::forward<Args>(args)...), _has_value(false) {}

            ~expected_base() = default;

            union {
                T _value;
                unexpected<E> _error;
            };
            bool _has_value;
        };

        template <typename T, typename E>
        struct expected_base<T, E, false, true> {

            constexpr expected_base(no_init_t)
                : _has_value(false) {} // OK, E is trivially destructible

            template <typename = void>
            requires(
                std::is_default_constructible_v<T>) constexpr expected_base()
                : _value(), _has_value(true) {}

            template <typename... Args>
            constexpr expected_base(in_place_t, Args&&... args)
                : _value(std::forward<Args>(args)...), _has_value(true) {}

            template <typename... Args>
            constexpr expected_base(unexpect_t, Args&&... args)
                : _error(std::forward<Args>(args)...), _has_value(false) {}

            ~expected_base() {
                if (_has_value) {
                    _value.~T();
                }
            }

            union {
                T _value;
                unexpected<E> _error;
            };
            bool _has_value;
        };

        template <typename T, typename E>
        struct expected_base<T, E, true, false> {

            constexpr expected_base(no_init_t)
                : _has_value(true) {} // OK, T is trivially destructible

            template <typename = void>
            requires(
                std::is_default_constructible_v<T>) constexpr expected_base()
                : _value(), _has_value(true) {}

            template <typename... Args>
            constexpr expected_base(in_place_t, Args&&... args)
                : _value(std::forward<Args>(args)...), _has_value(true) {}

            template <typename... Args>
            constexpr expected_base(unexpect_t, Args&&... args)
                : _error(std::forward<Args>(args)...), _has_value(false) {}

            ~expected_base() {
                if (!_has_value) {
                    _error.~unexpected<E>();
                }
            }

            union {
                T _value;
                unexpected<E> _error;
            };
            bool _has_value;
        };

        template <typename T, typename E>
        struct expected_base<T, E, false, false> {

            constexpr expected_base(no_init_t) : _is_init(false) {}

            template <typename = void>
            requires(
                std::is_default_constructible_v<T>) constexpr expected_base()
                : _value(), _has_value(true), _is_init(true) {}

            template <typename... Args>
            constexpr expected_base(in_place_t, Args&&... args)
                : _value(std::forward<Args>(args)...), _has_value(true),
                  _is_init(true) {}

            template <typename... Args>
            constexpr expected_base(unexpect_t, Args&&... args)
                : _error(std::forward<Args>(args)...), _has_value(false),
                  _is_init(true) {}

            ~expected_base() {
                if (_is_init) {
                    if (_has_value) {
                        _value.~T();
                    } else {
                        _error.~unexpected<E>();
                    }
                }
            }

            union {
                T _value;
                unexpected<E> _error;
            };
            bool _has_value;
            bool _is_init; // Do we want to merge _is_init with _has_value into
                           // a tri-state? Seems a bit complicated...
        };

        template <typename E>
        struct expected_base<void, E, false, true> {

            constexpr expected_base(no_init_t)
                : _has_value(true) {} // OK: void has no destructor

            constexpr expected_base() : _has_value(true) {}

            constexpr expected_base(in_place_t) : _has_value(true) {}

            template <typename... Args>
            constexpr expected_base(unexpect_t, Args&&... args)
                : _error(std::forward<Args>(args)...), _has_value(false) {}

            ~expected_base() = default;

            union {
                unexpected<E> _error;
            };
            bool _has_value;
        };

        template <typename E>
        struct expected_base<void, E, false, false> {

            constexpr expected_base(no_init_t)
                : _has_value(true) {} // OK: void has no destructor

            constexpr expected_base() : _has_value(true) {}

            constexpr expected_base(in_place_t) : _has_value(true) {}

            template <typename... Args>
            constexpr expected_base(unexpect_t, Args&&... args)
                : _error(std::forward<Args>(args)...), _has_value(false) {}

            ~expected_base() {
                if (!_has_value) {
                    _error.~unexpected<E>();
                }
            }

            union {
                unexpected<E> _error;
            };
            bool _has_value;
        };

        // expected_storage
        template <typename T, typename E>
        struct expected_storage : expected_base<T, E> {

            using expected_base<T, E>::expected_base;

            // TODO: noexcept() for copy/move constructor/assignments

            constexpr expected_storage(const expected_storage& rhs)
                : expected_base<T, E>(no_init) {
                if (rhs._has_value) {
                    construct_value(rhs._value);
                } else {
                    construct_error(rhs._error);
                }
            }

            // TODO: disable if not satisfied (?)
            // std::is_move_constructible_v<T>&&
            // std::is_move_constructible_v<E>)
            constexpr expected_storage(expected_storage&& rhs) noexcept(
                std::is_nothrow_move_constructible_v<T>&&
                    std::is_nothrow_move_constructible_v<E>)
                : expected_base<T, E>(no_init) {
                if (rhs._has_value) {
                    construct_value(std::move(rhs._value));
                } else {
                    construct_error(std::move(rhs._error));
                }
            }

            template <typename U, typename G>
            expected_storage(const expected<U, G>& rhs)
                : expected_base<T, E>(no_init) {
                if (bool(rhs)) {
                    construct_value(*rhs);
                } else {
                    construct_error(rhs.error());
                }
            }

            template <typename U, typename G>
            expected_storage(expected<U, G>&& rhs)
                : expected_base<T, E>(no_init) {
                if (bool(rhs)) {
                    construct_value(std::move(*rhs));
                } else {
                    construct_error(std::move(rhs.error()));
                }
            }

            expected_storage& operator=(const expected_storage& rhs)
            // TODO: noexcept(std::is_nothrow_copy_constructible_v<T>&&
            // std::is_nothrow_copy_constructible_v<E>)
            {
                if (this->_has_value) {
                    if (rhs._has_value) {
                        this->_value = rhs._value;
                    } else {
                        assign_error_to_value(rhs);
                    }
                } else {
                    if (rhs._has_value) {
                        assign_value_to_error(rhs);
                    } else {
                        this->_error = rhs._error;
                    }
                }
                return *this;
            }

            expected_storage& operator=(expected_storage&& rhs) {
                if (this->_has_value) {
                    if (rhs._has_value) {
                        this->_value = std::move(rhs._value);
                    } else {
                        move_error_to_value(std::move(rhs));
                    }
                } else {
                    if (rhs._has_value) {
                        move_value_to_error(std::move(rhs));
                    } else {
                        this->_error = std::move(rhs._error);
                    }
                }
                return *this;
            }

#if KZ_EXCEPTIONS
            template <typename U = T>
            requires(std::is_nothrow_constructible_v<T, U>) expected_storage&
            operator=(U&& rhs) {
                if (this->_has_value) {
                    this->_value = std::forward<U>(rhs);
                } else {
                    this->_error.~unexpected<E>();
                    construct_value(std::forward<U>(rhs));
                }
                return *this;
            }

            template <typename U = T>
            expected_storage& operator=(U&& rhs) {
                if (this->_has_value) {
                    this->_value = std::forward<U>(rhs);
                } else {
                    unexpected<E> temp(std::move(this->_error));
                    this->_error.~unexpected<E>();
                    try {
                        construct_value(std::forward<U>(rhs));
                    } catch (...) {
                        construct_error(std::move(temp));
                        throw;
                    }
                }
                return *this;
            }
#else
            template <typename U = T>
            expected_storage& operator=(U&& rhs) {
                if (this->_has_value) {
                    this->_value = std::forward<U>(rhs);
                } else {
                    this->_error.~unexpected<E>();
                    construct_value(std::forward<U>(rhs));
                }
                return *this;
            }

#endif

            template <class G = E>
            requires(std::is_nothrow_copy_constructible_v<E>&&
                    std::is_copy_assignable_v<E>) expected_storage&
            operator=(const unexpected<G>& e) {
                if (this->_has_value) {
                    this->_value.~T();
                    construct_error(unexpected(e.value()));

                } else {
                    this->_error = unexpected(e.value());
                }
                return *this;
            }

            template <class G = E>
            requires(std::is_nothrow_move_constructible_v<E>&&
                    std::is_move_assignable_v<E>) expected_storage&
            operator=(unexpected<G>&& e) {
                if (this->_has_value) {
                    this->_value.~T();
                    construct_error(unexpected(std::move(e.value())));

                } else {
                    this->_error = unexpected(std::move(e.value()));
                }
                return *this;
            }

#if KZ_EXCEPTIONS
            template <typename... Args>
            requires(std::is_nothrow_constructible_v<T, Args...>) T& emplace(
                Args&&... args) {
                if (this->_has_value) {
                    this->_value = T(std::forward<Args>(args)...);
                } else {
                    this->_error.~unexpected<E>();
                    construct_value(std::forward<Args>(args)...);
                }
                return this->_value;
            }

            template <typename... Args>
            requires(!std::is_nothrow_constructible_v<T, Args...> &&
                     std::is_nothrow_move_constructible_v<T>)
                T& emplace(Args&&... args) {
                if (this->_has_value) {
                    this->_value = T(std::forward<Args>(args)...);
                } else {
                    T temp(std::forward<Args>(args)...);
                    this->_error.~unexpected<E>();
                    construct_value(std::move(temp));
                }
                return this->_value;
            }

            template <typename... Args>
            T& emplace(Args&&... args) {
                if (this->_has_value) {
                    this->_value = T(std::forward<Args>(args)...);
                } else {
                    unexpected<E> temp(std::move(this->_error));
                    this->_error.~unexpected<E>();
                    try {
                        construct_value(std::forward<Args>(args)...);
                    } catch (...) {
                        construct_error(std::move(temp));
                        throw;
                    }
                }
                return this->_value;
            }
#else
            template <typename... Args>
            T& emplace(Args&&... args) {
                if (this->_has_value) {
                    this->_value = T(std::forward<Args>(args)...);
                } else {
                    this->_error.~unexpected<E>();
                    construct_value(std::forward<Args>(args)...);
                }
                return this->_value;
            }
#endif

            void swap(expected_storage& rhs) // TODO: noexcept()
            {
                if (this->_has_value) {
                    if (rhs._has_value) {
                        using std::swap;
                        swap(this->_value, rhs._value);
                    } else {
                        swap_value_with_error(rhs);
                    }
                } else {
                    if (rhs._has_value) {
                        rhs.swap(*this);
                    } else {
                        using std::swap;
                        swap(this->_error, rhs._error);
                    }
                }
            }

            template <typename = void>
            requires(std::is_nothrow_move_constructible_v<
                E>) void swap_value_with_error(expected_storage& rhs) {
                unexpected<E> temp(std::move(rhs._error));
                rhs._error.~unexpected<E>();

#if KZ_EXCEPTIONS
                try {
                    rhs.construct_value(std::move(this->_value));
                } catch (...) {
                    rhs.construct_error(std::move(temp));
                    throw;
                }
#else
                rhs.construct_value(std::move(this->_value));
#endif
                this->_value.~T();
                construct_error(std::move(temp));
            }

            template <typename = void>
            requires(!std::is_nothrow_move_constructible_v<E> &&
                     std::is_nothrow_move_constructible_v<
                         T>) void swap_value_with_error(expected_storage& rhs) {
                T temp(std::move(this->_value));
                this->_value.~T();
#if KZ_EXCEPTIONS
                try {
                    construct_error(std::move(rhs._error.value()));
                } catch (...) {
                    construct_value(std::move(temp));
                    throw;
                }
#else
                construct_error(std::move(rhs._error));
#endif

                rhs._error.~unexpected<E>();
                rhs.construct_value(std::move(temp));
            }

#if KZ_EXCEPTIONS
            template <typename = void>
            requires(std::is_nothrow_copy_constructible_v<
                E>) void assign_error_to_value(const expected_storage& rhs) {
                this->_value.~T();
                construct_error(rhs._error);
            }

            template <typename = void>
            requires(!std::is_nothrow_copy_constructible_v<E> &&
                     std::is_nothrow_move_constructible_v<
                         E>) void assign_error_to_value(const expected_storage&
                    rhs) {
                unexpected<E> temp(rhs._error);
                this->_value.~T();
                construct_error(std::move(temp));
            }

            void assign_error_to_value(const expected_storage& rhs) {
                T temp(this->_value);
                this->_value.~T();
                try {
                    construct_error(rhs._error);
                } catch (...) {
                    construct_value(std::move(temp));
                    throw;
                }
            }

            template <typename = void>
            requires(std::is_nothrow_copy_constructible_v<
                T>) void assign_value_to_error(const expected_storage& rhs) {
                this->_error.~unexpected<E>();
                construct_value(rhs._value);
            }

            template <typename = void>
            requires(!std::is_nothrow_copy_constructible_v<T> &&
                     std::is_nothrow_move_constructible_v<
                         T>) void assign_value_to_error(const expected_storage&
                    rhs) {
                T temp(rhs._value);
                this->_error.~unexpected<E>();
                construct_value(std::move(temp));
            }

            void assign_value_to_error(const expected_storage& rhs) {
                unexpected<E> temp(this->_error);
                this->_error.~unexpected<E>();
                try {
                    construct_value(rhs._value);
                } catch (...) {
                    construct_error(std::move(temp));
                    throw;
                }
            }

#else
            void assign_error_to_value(const expected_storage& rhs) {
                this->_value.~T();
                construct_error(rhs._error);
            }

            void assign_value_to_error(const expected_storage& rhs) {
                this->_error.~unexpected<E>();
                construct_value(rhs._value);
            }
#endif

#if KZ_EXCEPTIONS

            template <typename = void>
            requires(std::is_nothrow_move_constructible_v<
                E>) void move_error_to_value(expected_storage&& rhs) {
                this->_value.~T();
                construct_error(std::move(rhs._error));
            }

            template <typename = void>
            void move_error_to_value(expected_storage&& rhs) {
                T temp(std::move(this->_value));
                this->_value.~T();
                try {
                    construct_error(std::move(rhs._error));
                } catch (...) {
                    construct_value(std::move(temp));
                    throw;
                }
            }

            template <typename = void>
            requires(std::is_nothrow_move_constructible_v<
                T>) void move_value_to_error(expected_storage&& rhs) {
                this->_error.~unexpected<E>();
                construct_value(std::move(rhs._value));
            }

            template <typename = void>
            void move_value_to_error(expected_storage&& rhs) {
                unexpected<E> temp(std::move(this->_error));
                this->_error.~unexpected<E>();
                try {
                    construct_value(std::move(rhs._value));
                } catch (...) {
                    construct_error(std::move(temp));
                    throw;
                }
            }
#else

            template <typename = void>
            void move_error_to_value(expected_storage&& rhs) {
                this->_value.~T();
                construct_error(std::move(rhs._error));
            }

            template <typename = void>
            void move_value_to_error(expected_storage&& rhs) {
                this->_error.~unexpected<E>();
                construct_value(std::move(rhs._value));
            }
#endif

            template <typename... Args>
            constexpr void construct_value(Args&&... args) {
                new (std::addressof(this->_value))
                    T(std::forward<Args>(args)...);
                this->_has_value = true;
            }

            template <typename... Args>
            constexpr void construct_error(Args&&... args) {
                new (std::addressof(this->_error))
                    unexpected<E>(std::forward<Args>(args)...);
                this->_has_value = false;
            }
        };

        template <typename E>
        struct expected_storage<void, E> : expected_base<void, E> {

            using expected_base<void, E>::expected_base;

            // TODO: noexcept() for copy/move constructor/assignments

            constexpr expected_storage(const expected_storage& rhs)
                : expected_base<void, E>(no_init) {
                if (rhs._has_value) {
                    construct_value();
                } else {
                    construct_error(rhs._error);
                }
            }

            // TODO: disable if not satisfied (?)
            // std::is_move_constructible_v<T>&&
            // std::is_move_constructible_v<E>)
            constexpr expected_storage(expected_storage&& rhs) noexcept(
                std::is_nothrow_move_constructible_v<E>)
                : expected_base<void, E>(no_init) {
                if (rhs._has_value) {
                    construct_value();
                } else {
                    construct_error(std::move(rhs._error));
                }
            }

            template <typename G>
            expected_storage(const expected<void, G>& rhs)
                : expected_base<void, E>(no_init) {
                if (bool(rhs)) {
                    construct_value();
                } else {
                    construct_error(rhs.error());
                }
            }

            template <typename G>
            expected_storage(expected<void, G>&& rhs)
                : expected_base<void, E>(no_init) {
                if (bool(rhs)) {
                    construct_value();
                } else {
                    construct_error(std::move(rhs.error()));
                }
            }

            expected_storage& operator=(const expected_storage& rhs) {
                if (this->_has_value) {
                    if (!rhs._has_value) {
                        construct_error(rhs._error);
                    }
                } else {
                    if (rhs._has_value) {
                        this->_error.~unexpected<E>();
                        construct_value();
                    } else {
                        this->_error = rhs._error;
                    }
                }
                return *this;
            }

            expected_storage& operator=(expected_storage&& rhs) {
                if (this->_has_value) {
                    if (!rhs._has_value) {
                        construct_error(std::move(rhs._error));
                    }
                } else {
                    if (rhs._has_value) {
                        this->_error.~unexpected<E>();
                        construct_value();
                    } else {
                        this->_error = std::move(rhs._error);
                    }
                }
                return *this;
            }

            template <class G = E>
            requires(std::is_nothrow_copy_constructible_v<E>&&
                    std::is_copy_assignable_v<E>) expected_storage&
            operator=(const unexpected<G>& e) {
                if (this->_has_value) {
                    construct_error(unexpected(e.value()));

                } else {
                    this->_error = unexpected(e.value());
                }
                return *this;
            }

            template <class G = E>
            requires(std::is_nothrow_move_constructible_v<E>&&
                    std::is_move_assignable_v<E>) expected_storage&
            operator=(unexpected<G>&& e) {
                if (this->_has_value) {
                    construct_error(unexpected(std::move(e.value())));

                } else {
                    this->_error = unexpected(std::move(e.value()));
                }
                return *this;
            }

            void swap(expected_storage& rhs) // TODO: noexcept()
            {
                if (this->_has_value) {
                    if (!rhs._has_value) {
                        swap_value_with_error(rhs);
                    }
                } else {
                    if (rhs._has_value) {
                        rhs.swap(*this);
                    } else {
                        using std::swap;
                        swap(this->_error, rhs._error);
                    }
                }
            }

            void swap_value_with_error(expected_storage& rhs) {
                construct_error(std::move(rhs._error)); // Can throw
                rhs._error.~unexpected<E>();
                rhs.construct_value();
            }

            void emplace() {
                if (!this->_has_value) {
                    this->_error.~unexpected<E>();
                    this->_has_value = true;
                }
            }

            constexpr void construct_value() { this->_has_value = true; }

            template <typename... Args>
            constexpr void construct_error(Args&&... args) {
                new (std::addressof(this->_error))
                    unexpected<E>(std::forward<Args>(args)...);
                this->_has_value = false;
            }
        };

        // expected_default_constructors is used to delete the default copy
        // and move constructors when they are not available.
        template <typename T, typename E,
            bool enable_copy_constructor =
                (std::is_void_v<T> || std::is_copy_constructible_v<
                                          T>)&&std::is_copy_constructible_v<E>,
            bool enable_move_constructor =
                (std::is_void_v<T> || std::is_move_constructible_v<
                                          T>)&&std::is_move_constructible_v<E>>
        struct expected_default_constructors {
            expected_default_constructors() = default;
            expected_default_constructors(
                const expected_default_constructors&) = default;
            expected_default_constructors(
                expected_default_constructors&&) = default;
            expected_default_constructors& operator=(
                const expected_default_constructors&) = default;
            expected_default_constructors& operator=(
                expected_default_constructors&&) noexcept = default;
        };

        template <typename T, typename E>
        struct expected_default_constructors<T, E, false, true> {
            expected_default_constructors() = default;
            expected_default_constructors(
                const expected_default_constructors&) = delete;
            expected_default_constructors(
                expected_default_constructors&&) = default;
            expected_default_constructors& operator=(
                const expected_default_constructors&) = default;
            expected_default_constructors& operator=(
                expected_default_constructors&&) noexcept = default;
        };

        template <typename T, typename E>
        struct expected_default_constructors<T, E, true, false> {
            expected_default_constructors() = default;
            expected_default_constructors(
                const expected_default_constructors&) = default;
            expected_default_constructors(
                expected_default_constructors&&) = delete;
            expected_default_constructors& operator=(
                const expected_default_constructors&) = default;
            expected_default_constructors& operator=(
                expected_default_constructors&&) noexcept = default;
        };

        template <typename T, typename E>
        struct expected_default_constructors<T, E, false, false> {
            expected_default_constructors() = default;
            expected_default_constructors(
                const expected_default_constructors&) = delete;
            expected_default_constructors(
                expected_default_constructors&&) = delete;
            expected_default_constructors& operator=(
                const expected_default_constructors&) = default;
            expected_default_constructors& operator=(
                expected_default_constructors&&) noexcept = default;
        };

    } // namespace detail
} // namespace kz
