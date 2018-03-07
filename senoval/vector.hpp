/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Zubax Robotics
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Author: Pavel Kirienko <pavel.kirienko@zubax.com>
 */

#pragma once

#if __cplusplus < 201703
# error "This library requires C++17 or newer"
#endif

#include <algorithm>
#include <iterator>
#include <cstring>
#include <cassert>
#include <cstdint>


namespace senoval
{
/**
 * A vector with fixed storage, API like std::vector<>.
 * This implementation supports only trivial types, since that is sufficient for the needs of this library.
 * Support for non-trivial types may be added in the future in a fully backward-compatible way.
 */
template <typename T, std::size_t Capacity_>
class Vector
{
public:
    static constexpr std::size_t Capacity = Capacity_;

    static_assert(Capacity > 0, "Capacity must be positive");
    static_assert(std::is_trivial<T>::value, "This implementation supports only trivial types.");

private:
    std::size_t len_ = 0;
    T buf_[Capacity];       // Note that we don't zero-initialize, just like std containers

public:
    Vector() = default;

    // Implicit by design
    Vector(std::initializer_list<T> values)
    {
        for (const auto& v : values)
        {
            push_back(v);
        }
    }

    template <typename InputIterator, typename = std::enable_if_t<!std::is_integral_v<InputIterator>>>
    Vector(InputIterator begin, const InputIterator end) // NOLINT
    {
        while ((begin != end) && (len_ < Capacity))
        {
            buf_[len_] = T(*begin);
            ++len_;
            ++begin;
        }
    }

    Vector(std::size_t count, const T& value)
    {
        while (count --> 0)
        {
            push_back(value);
        }
    }

    template <typename Container, typename = decltype(std::begin(std::declval<Container>()))>
    void append(const Container& other)
    {
        std::copy(std::begin(other), std::end(other), std::back_inserter(*this));
    }

    /*
     * std::vector API
     */
    using value_type = T;
    using size_type = std::size_t;
    using iterator = T*;
    using const_iterator = const T*;

    [[nodiscard]] constexpr std::size_t capacity() const { return Capacity; }
    [[nodiscard]] constexpr std::size_t max_size() const { return Capacity; }

    [[nodiscard]] std::size_t size() const { return len_; }

    [[nodiscard]] // nodiscard prevents confusion with clear()
    bool empty() const { return len_ == 0; }

    void clear()
    {
        len_ = 0;
    }

    void resize(const std::size_t count, const T& fill_value = T{})
    {
        assert(count <= Capacity);
        if (count < len_)
        {
            len_ = count;
        }
        else
        {
            while (count > len_)
            {
                push_back(fill_value);
            }
        }
        assert(size() == count);    // Will fail if count > Capacity
    }

    void push_back(const T& c)
    {
        if (len_ < Capacity)
        {
            buf_[len_] = c;
            ++len_;
        }
        else
        {
            assert(false);
        }
    }

    void pop_back()
    {
        if (len_ > 0)
        {
            --len_;
        }
        else
        {
            assert(false);
        }
    }

    [[nodiscard]] T&       front()       { return operator[](0); }
    [[nodiscard]] const T& front() const { return operator[](0); }

    [[nodiscard]]
    T& back()
    {
        if (len_ > 0)
        {
            return buf_[len_ - 1U];
        }
        else
        {
            assert(false);
            return buf_[0];
        }
    }
    [[nodiscard]] const T& back() const { return const_cast<Vector*>(this)->back(); }

    [[nodiscard]] T* begin() { return &buf_[0]; }
    [[nodiscard]] T* end()   { return &buf_[len_]; }

    [[nodiscard]] const T* begin() const { return &buf_[0]; }
    [[nodiscard]] const T* end()   const { return &buf_[len_]; }

    [[nodiscard]] T* data() { return &buf_[0]; }
    [[nodiscard]] const T* data() const { return &buf_[0]; }

    /*
     * Operators
     */
    [[nodiscard]]
    T& operator[](std::size_t index)
    {
        if (index < len_)
        {
            return buf_[index];
        }
        else
        {
            assert(false);
            return back();
        }
    }
    [[nodiscard]]
    const T& operator[](std::size_t index) const
    {
        return const_cast<Vector*>(this)->operator[](index);
    }

    template <typename S, typename = decltype(std::declval<S>().begin())>
    [[nodiscard]]
    bool operator==(const S& s) const
    {
        return std::equal(begin(), end(), std::begin(s), std::end(s));
    }

    template <typename S>
    [[nodiscard]]
    bool operator!=(const S& s) const
    {
        return !operator==(s);
    }
};

}
