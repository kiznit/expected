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

#include <memory>
#include <metal/expected_bits/unexpected.hpp>

namespace mtl {

    template <typename T, typename E> class expected;

    struct unexpect_t {};
    inline constexpr unexpect_t unexpect;

    namespace detail {
        template <typename T, typename E> class expected_storage {
        public:
            constexpr expected_storage() : expected_storage(std::in_place) {}

            constexpr expected_storage(const expected_storage& other)
                : _has_value(other._has_value) {
                if (_has_value)
                    new (std::addressof(_value)) T(other._value);
                else
                    new (std::addressof(_error))
                        unexpected<E>(other._error.value());
            }

            constexpr expected_storage(expected_storage&& other)
                : _has_value(other._has_value) {
                if (_has_value)
                    new (std::addressof(_value)) T(std::move(other._value));
                else
                    new (std::addressof(_error))
                        unexpected<E>(std::move(other._error.value()));
            }

            template <typename U, typename G>
            constexpr expected_storage(const expected<U, G>& other)
                : _has_value(other.has_value()) {
                if (_has_value)
                    new (std::addressof(_value)) T(*other);
                else
                    new (std::addressof(_error)) unexpected<E>(other.error());
            }

            template <typename U, typename G>
            constexpr expected_storage(expected<U, G>&& other)
                : _has_value(other.has_value()) {
                if (_has_value)
                    new (std::addressof(_value)) T(std::move(*other));
                else
                    new (std::addressof(_error))
                        unexpected<E>(std::move(other.error()));
            }

            template <typename... Args>
            constexpr expected_storage(std::in_place_t, Args&&... args)
                : _has_value(true), _value(std::forward<Args>(args)...) {}

            template <class U, typename... Args>
            constexpr expected_storage(std::in_place_t,
                                       std::initializer_list<U> list,
                                       Args&&... args)
                : _has_value(true), _value(list, std::forward<Args>(args)...) {}

            template <typename... Args>
            constexpr expected_storage(unexpect_t, Args&&... args)
                : _has_value(false), _error(std::forward<Args>(args)...) {}

            expected_storage& operator=(const expected_storage& other) {
                if (_has_value == other._has_value) {
                    if (_has_value) {
                        _value = other._value;
                    } else {
                        _error = other._error;
                    }
                } else if (_has_value) {
                    assign_error_to_value(other);
                } else {
                    assign_value_to_error(other);
                }
                return *this;
            }

            expected_storage& operator=(expected_storage&& other) {
                if (_has_value == other._has_value) {
                    if (_has_value) {
                        _value = std::move(other._value);
                    } else {
                        _error = std::move(other._error);
                    }
                } else if (_has_value) {
                    move_error_to_value(std::move(other));
                } else {
                    move_value_to_error(std::move(other));
                }
                return *this;
            }

#if MTL_EXCEPTIONS
            template <typename U> expected_storage& operator=(U&& other) {
                if (_has_value) {
                    _value = std::forward<U>(other);
                } else if (std::is_nothrow_constructible_v<T, U>) {
                    _error.~unexpected<E>();
                    new (std::addressof(_value)) T(std::forward<U>(other));
                    _has_value = true;
                } else {
                    unexpected<E> temp(std::move(_error));
                    _error.~unexpected<E>();
                    try {
                        new (std::addressof(_value)) T(std::forward<U>(other));
                        _has_value = true;
                    } catch (...) {
                        new (std::addressof(_error))
                            unexpected<E>(std::move(temp));
                        throw;
                    }
                }
                return *this;
            }
#else
            template <typename U> expected_storage& operator=(U&& other) {
                if (_has_value) {
                    _value = std::forward<U>(other);
                } else {
                    _error.~unexpected<E>();
                    new (std::addressof(_value)) T(std::forward<U>(other));
                    _has_value = true;
                }
                return *this;
            }
#endif

            // P0323R10 has multiple errors for this operator, so we do what we
            // think is right
            template <typename G = E>
            expected_storage& operator=(const unexpected<G>& error) {
                if (_has_value) {
                    _value.~T();
                    new (std::addressof(_error)) unexpected(error.value());
                    _has_value = false;
                } else {
                    _error = unexpected(error.value());
                }
                return *this;
            }

            template <typename G = E>
            expected_storage& operator=(unexpected<G>&& error) {
                if (_has_value) {
                    _value.~T();
                    new (std::addressof(_error))
                        unexpected(std::move(error.value()));
                    _has_value = false;
                } else {
                    _error = unexpected(std::move(error.value()));
                }
                return *this;
            }

            ~expected_storage() {
                if (_has_value)
                    _value.~T();
                else
                    _error.~unexpected<E>();
            }

#if MTL_EXCEPTIONS
            template <typename U = T,
                      std::enable_if_t<
                          std::is_nothrow_copy_constructible_v<U>>* = nullptr>
            void assign_value_to_error(const expected_storage& other) noexcept {
                _error.~unexpected<E>();
                new (std::addressof(_value)) T(other._value);
                _has_value = true;
            }

            template <typename U = T,
                      std::enable_if_t<
                          !std::is_nothrow_copy_constructible_v<U> &&
                          std::is_nothrow_move_constructible_v<U>>* = nullptr>
            void assign_value_to_error(const expected_storage& other) noexcept {
                T temp(other._value);
                _error.~unexpected<E>();
                new (std::addressof(_value)) T(std::move(other._value));
                _has_value = true;
            }

            template <typename U = T,
                      std::enable_if_t<
                          !std::is_nothrow_copy_constructible_v<U> &&
                          !std::is_nothrow_move_constructible_v<U> &&
                          std::is_nothrow_move_constructible_v<E>>* = nullptr>
            void assign_value_to_error(const expected_storage& other) {
                unexpected<E> temp(std::move(_error));
                _error.~unexpected<E>();
                try {
                    new (std::addressof(_value)) T(other._value);
                    _has_value = true;
                } catch (...) {
                    new (std::addressof(_error)) unexpected<E>(std::move(temp));
                    throw;
                }
            }
#else
            void assign_value_to_error(const expected_storage& other) noexcept {
                _error.~unexpected<E>();
                new (std::addressof(_value)) T(other._value);
                _has_value = true;
            }
#endif

#if MTL_EXCEPTIONS
            template <typename G = E,
                      std::enable_if_t<
                          std::is_nothrow_copy_constructible_v<G>>* = nullptr>
            void assign_error_to_value(const expected_storage& other) noexcept {
                _value.~T();
                new (std::addressof(_error)) unexpected<E>(other._error);
                _has_value = false;
            }

            template <typename G = E,
                      std::enable_if_t<
                          !std::is_nothrow_copy_constructible_v<G> &&
                          std::is_nothrow_move_constructible_v<G>>* = nullptr>
            void assign_error_to_value(const expected_storage& other) noexcept {
                unexpected<E> temp(other._error);
                _value.~T();
                new (std::addressof(_error))
                    unexpected<E>(std::move(other._error));
                _has_value = false;
            }

            template <typename G = E,
                      std::enable_if_t<
                          !std::is_nothrow_copy_constructible_v<G> &&
                          !std::is_nothrow_move_constructible_v<G> &&
                          std::is_nothrow_move_constructible_v<T>>* = nullptr>
            void assign_error_to_value(const expected_storage& other) {
                T temp(std::move(_value));
                _value.~T();
                try {
                    new (std::addressof(_error)) unexpected<E>(other._error);
                    _has_value = false;
                } catch (...) {
                    new (std::addressof(_value)) T(std::move(temp));
                    throw;
                }
            }
#else
            void assign_error_to_value(const expected_storage& other) noexcept {
                _value.~T();
                new (std::addressof(_error)) unexpected<E>(other._error);
                _has_value = false;
            }
#endif

#if MTL_EXCEPTIONS
            template <typename U = T,
                      std::enable_if_t<
                          std::is_nothrow_move_constructible_v<U>>* = nullptr>
            void move_value_to_error(expected_storage&& other) noexcept {
                _error.~unexpected<E>();
                new (std::addressof(_value)) T(std::move(other._value));
                _has_value = true;
            }

            template <typename U = T,
                      std::enable_if_t<
                          !std::is_nothrow_move_constructible_v<U>>* = nullptr>
            void move_value_to_error(expected_storage&& other) {
                unexpected<E> temp{std::move(_error)};
                _error.~unexpected<E>();
                try {
                    new (std::addressof(_value)) T(std::move(other._value));
                    _has_value = true;
                } catch (...) {
                    new (std::addressof(_error)) unexpected<E>(std::move(temp));
                    throw;
                }
            }
#else
            void move_value_to_error(expected_storage&& other) noexcept {
                _error.~unexpected<E>();
                new (std::addressof(_value)) T(std::move(other._value));
                _has_value = true;
            }
#endif

#if MTL_EXCEPTIONS
            template <typename G = E,
                      std::enable_if_t<
                          std::is_nothrow_move_constructible_v<G>>* = nullptr>
            void move_error_to_value(expected_storage&& other) noexcept {
                _value.~T();
                new (std::addressof(_error))
                    unexpected<E>(std::move(other._error));
                _has_value = false;
            }

            template <typename G = E,
                      std::enable_if_t<
                          !std::is_nothrow_move_constructible_v<G>>* = nullptr>
            void move_error_to_value(expected_storage&& other) {
                T temp{std::move(_value)};
                _value.~T();
                try {
                    new (std::addressof(_error))
                        unexpected<E>(std::move(other._error));
                    _has_value = false;
                } catch (...) {
                    new (std::addressof(_value)) T(std::move(temp));
                    throw;
                }
            }
#else
            void move_error_to_value(expected_storage&& other) noexcept {
                _value.~T();
                new (std::addressof(_error))
                    unexpected<E>(std::move(other._error));
                _has_value = false;
            }
#endif

#if MTL_EXCEPTIONS
            template <typename... Args> T& emplace(Args&&... args) {
                if (_has_value) {
                    _value = T{std::forward<Args>(args)...};
                } else if (std::is_nothrow_constructible_v<T, Args...>) {
                    _error.~unexpected<E>();
                    new (std::addressof(_value)) T{std::forward<Args>(args)...};
                    _has_value = true;
                } else if (std::is_nothrow_move_constructible_v<T>) {
                    // TODO: using {} instead of () when constructing temp
                    // results in unit test failures. I suspect this happens
                    // when T = std::vector<>. but I haven't verified. It
                    // appears {} and () are not the same thing and we need to
                    // figure this out and review all the code to ensure we got
                    // this right.
                    T temp(std::forward<Args>(args)...);
                    _error.~unexpected<E>();
                    new (std::addressof(_value)) T{std::move(temp)};
                    _has_value = true;
                } else {
                    unexpected<E> temp(std::move(_error));
                    _error.~unexpected<E>();
                    try {
                        new (std::addressof(_value))
                            T{std::forward<Args>(args)...};
                        _has_value = true;
                    } catch (...) {
                        new (std::addressof(_error))
                            unexpected<E>(std::move(temp));
                        throw;
                    }
                }
                return _value;
            }

            template <typename U, typename... Args>
            T& emplace(std::initializer_list<U> list, Args&&... args) {
                if (_has_value) {
                    _value = T{list, std::forward<Args>(args)...};
                } else if (std::is_nothrow_constructible_v<
                               T, std::initializer_list<U>&, Args...>) {
                    _error.~unexpected<E>();
                    new (std::addressof(_value))
                        T{list, std::forward<Args>(args)...};
                    _has_value = true;
                } else if (std::is_nothrow_move_constructible_v<T>) {
                    T temp(list, std::forward<Args>(args)...);
                    _error.~unexpected<E>();
                    new (std::addressof(_value)) T{std::move(temp)};
                    _has_value = true;
                } else {
                    unexpected<E> temp(std::move(_error));
                    _error.~unexpected<E>();
                    try {
                        new (std::addressof(_value))
                            T{list, std::forward<Args>(args)...};
                        _has_value = true;
                    } catch (...) {
                        new (std::addressof(_error))
                            unexpected<E>(std::move(temp));
                        throw;
                    }
                }
                return _value;
            }
#else
            template <typename... Args> T& emplace(Args&&... args) {
                if (_has_value) {
                    _value = T{std::forward<Args>(args)...};
                } else if (std::is_nothrow_constructible_v<T, Args...>) {
                    _error.~unexpected<E>();
                    new (std::addressof(_value)) T{std::forward<Args>(args)...};
                    _has_value = true;
                } else if (std::is_nothrow_move_constructible_v<T>) {
                    T temp(std::forward<Args>(args)...);
                    _error.~unexpected<E>();
                    new (std::addressof(_value)) T{std::move(temp)};
                    _has_value = true;
                } else {
                    _error.~unexpected<E>();
                    new (std::addressof(_value)) T{std::forward<Args>(args)...};
                    _has_value = true;
                }
                return _value;
            }

            template <typename U, typename... Args>
            T& emplace(std::initializer_list<U> list, Args&&... args) {
                if (_has_value) {
                    _value = T{list, std::forward<Args>(args)...};
                } else if (std::is_nothrow_constructible_v<
                               T, std::initializer_list<U>&, Args...>) {
                    _error.~unexpected<E>();
                    new (std::addressof(_value))
                        T{list, std::forward<Args>(args)...};
                    _has_value = true;
                } else if (std::is_nothrow_move_constructible_v<T>) {
                    T temp(list, std::forward<Args>(args)...);
                    _error.~unexpected<E>();
                    new (std::addressof(_value)) T{std::move(temp)};
                    _has_value = true;
                } else {
                    _error.~unexpected<E>();
                    new (std::addressof(_value))
                        T{list, std::forward<Args>(args)...};
                    _has_value = true;
                }
                return _value;
            }
#endif

#if MTL_EXCEPTIONS
            void swap(expected_storage& other) {
                if (_has_value) {
                    if (other._has_value) {
                        using std::swap;
                        swap(_value, other._value);
                    } else {
                        if (std::is_nothrow_move_constructible_v<E>) {
                            unexpected<E> temp(std::move(other._error));
                            _error.~unexpected<E>();
                            new (std::addressof(other._value))
                                T(std::move(_value));
                            _value.~T();
                            new (std::addressof(_error))
                                unexpected<E>(std::move(temp));
                            _has_value = false;
                            other._has_value = true;
                        } else {
                            T temp(std::move(_value));
                            _value.~T();
                            new (std::addressof(_error))
                                unexpected<E>(std::move(other._error));
                            other._error.~unexpected<E>();
                            new (std::addressof(other._value))
                                T(std::move(temp));
                            _has_value = false;
                            other._has_value = true;
                        }
                    }
                } else {
                    if (other._has_value) {
                        other.swap(*this);
                    } else {
                        using std::swap;
                        swap(_error, other._error);
                    }
                }
            }
#else
            void swap(expected_storage& other) {
                if (_has_value) {
                    if (other._has_value) {
                        using std::swap;
                        swap(_value, other._value);
                    } else {
                        if (std::is_nothrow_move_constructible_v<E>) {
                            unexpected<E> temp(std::move(other._error));
                            _error.~unexpected<E>();
                            new (std::addressof(other._value))
                                T(std::move(_value));
                            _value.~T();
                            new (std::addressof(_error))
                                unexpected<E>(std::move(temp));
                            _has_value = false;
                            other._has_value = true;
                        } else {
                            T temp(std::move(_value));
                            _value.~T();
                            new (std::addressof(_error))
                                unexpected<E>(std::move(other._error));
                            other._error.~unexpected<E>();
                            new (std::addressof(other._value))
                                T(std::move(temp));
                            _has_value = false;
                            other._has_value = true;
                        }
                    }
                } else {
                    if (other._has_value) {
                        other.swap(*this);
                    } else {
                        using std::swap;
                        swap(_error, other._error);
                    }
                }
            }
#endif

            bool _has_value;
            union {
                T _value;
                unexpected<E> _error;
            };
        };

        template <typename E> class expected_storage<void, E> {
        public:
            constexpr expected_storage() : _has_value(true) {}

            constexpr expected_storage(const expected_storage& other)
                : _has_value(other._has_value) {
                if (!_has_value)
                    new (std::addressof(_error))
                        unexpected<E>(other._error.value());
            }

            constexpr expected_storage(expected_storage&& other)
                : _has_value(other._has_value) {
                if (!_has_value)
                    new (std::addressof(_error))
                        unexpected<E>(std::move(other._error.value()));
            }

            template <typename G>
            constexpr expected_storage(const expected<void, G>& other)
                : _has_value(other.has_value()) {
                if (!_has_value)
                    new (std::addressof(_error)) unexpected<E>(other.error());
            }

            template <typename G>
            constexpr expected_storage(expected<void, G>&& other)
                : _has_value(other.has_value()) {
                if (!_has_value)
                    new (std::addressof(_error))
                        unexpected<E>(std::move(other.error()));
            }

            constexpr expected_storage(std::in_place_t) : _has_value(true) {}

            template <typename... Args>
            constexpr expected_storage(unexpect_t, Args&&... args) noexcept {
                new (std::addressof(_error))
                    unexpected<E>(std::forward<Args>(args)...);
                _has_value = false;
            }

            expected_storage& operator=(const expected_storage& other) {
                if (_has_value == other._has_value) {
                    if (!_has_value) {
                        _error = other._error;
                    }
                } else if (_has_value) {
                    new (std::addressof(_error)) unexpected<E>(other._error);
                    _has_value = false;
                } else {
                    _error.~unexpected<E>();
                    _has_value = true;
                }
                return *this;
            }

            expected_storage& operator=(expected_storage&& other) {
                if (_has_value == other._has_value) {
                    if (!_has_value) {
                        _error = std::move(other._error);
                    }
                } else if (_has_value) {
                    new (std::addressof(_error))
                        unexpected<E>(std::move(other._error));
                    _has_value = false;
                } else {
                    _error.~unexpected<E>();
                    _has_value = true;
                }
                return *this;
            }

            // P0323R10 has multiple errors for this operator, so we do what
            // we think is right
            template <typename G = E>
            expected_storage& operator=(const unexpected<G>& error) {
                if (_has_value) {
                    new (std::addressof(_error)) unexpected(error.value());
                    _has_value = false;
                } else {
                    _error = unexpected(error.value());
                }
                return *this;
            }

            template <typename G = E>
            expected_storage& operator=(unexpected<G>&& error) {
                if (_has_value) {
                    new (std::addressof(_error))
                        unexpected(std::move(error.value()));
                    _has_value = false;
                } else {
                    _error = unexpected(std::move(error.value()));
                }
                return *this;
            }

            ~expected_storage() {
                if (!_has_value)
                    _error.~unexpected<E>();
            }

            void swap(expected_storage& other) {
                if (_has_value && !other._has_value) {
                    new (std::addressof(_error))
                        unexpected(std::move(other._error));
                    _has_value = false;
                    other._error.~unexpected<E>();
                    other._has_value = true;
                } else if (!_has_value) {
                    if (other._has_value) {
                        other.swap(*this);
                    } else {
                        using std::swap;
                        std::swap(_error, other._error);
                    }
                }
            }

            bool _has_value;
            union {
                unexpected<E> _error;
            };
        };
    } // namespace detail
} // namespace mtl
