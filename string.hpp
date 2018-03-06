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
#include <cctype>


namespace senoval
{
/**
 * Converts any signed or unsigned integer or boolean to string and returns it by value.
 * The argument must be of integral type, otherwise the call will be rejected by SFINAE.
 * Usage examples:
 *      intToString(var)
 *      intToString<16>(var)
 *      intToString<2>(var).c_str()
 * It is safe to obtain a reference to the returned string and pass it to another function as an argument,
 * which enables use cases like this (this example is somewhat made up, but it conveys the idea nicely):
 *      print("%s", intToString(123).c_str());
 * The returned value can be also converted to String<>.
 * More info on rvalue references:
 *      https://herbsutter.com/2008/01/01/gotw-88-a-candidate-for-the-most-important-const/
 *      http://stackoverflow.com/questions/584824/guaranteed-lifetime-of-temporary-in-c
 */
template <
    std::uint8_t Radix = 10,
    typename T,
    typename = std::enable_if_t<std::is_integral<T>::value>>
inline constexpr auto convertIntToString(T number)
{
    static_assert(Radix >= 1, "Radix must be positive");

    // Plus 1 to round up, see the standard for details.
    constexpr std::uint16_t MaxChars =
        std::uint16_t(((Radix >= 10) ? std::numeric_limits<T>::digits10 : std::numeric_limits<T>::digits) +
                      1 + (std::is_signed<T>::value ? 1 : 0));

    class Container
    {
        std::uint16_t offset_;
        char storage_[MaxChars + 1]{};   // Plus 1 because of zero termination.

    public:
        explicit constexpr Container(T x) :
            offset_(MaxChars)          // Field initialization is not working in GCC in this context, not sure why.
        {
            constexpr char Alphabet[] = "0123456789abcdefghijklmnopqrstuvwxyz";
            static_assert(Radix <= (sizeof(Alphabet) / sizeof(Alphabet[0])), "Radix is too large");

            bool negative = false;
            if constexpr (std::is_signed<T>::value)
            {
                // Usage of constexpr if allows us to silence the comparison warning for unsigned types
                negative = (x < 0);
            }

            do
            {
                assert(offset_ > 0);

                if constexpr (std::is_signed<T>::value)
                {
                    // We can't just do "x = -x", because it would break on int8_t(-128), int16_t(-32768), etc.
                    std::int8_t residual = std::int8_t(x % Radix);
                    if (residual < 0)
                    {
                        // Should never happen - since C++11, neg % pos --> pos
                        residual = std::int8_t(-residual);
                    }

                    storage_[--offset_] = Alphabet[residual];

                    // Signed integers are really tricky sometimes.
                    // We must not mix negative with positive to avoid implementation-defined behaviors.
                    x = T((x < 0) ? -(x / -Radix) : (x / Radix));
                }
                else
                {
                    // Fast branch for unsigned arguments.
                    storage_[--offset_] = Alphabet[x % Radix];
                    x = T(x / Radix);
                }
            }
            while (x != 0);

            if (negative)
            {
                assert(offset_ > 0);
                storage_[--offset_] = '-';
            }

            assert(offset_ < MaxChars);                 // Making sure there was no overflow.
        }

        const char* c_str() const { return &storage_[offset_]; }

        operator const char* () const { return this->c_str(); }

        std::uint16_t length() const { return std::uint16_t(MaxChars - offset_); }
        std::uint16_t size() const { return length(); }

        constexpr std::uint16_t capacity() const { return MaxChars; }
        constexpr std::uint16_t max_size() const { return MaxChars; }
    };

    return Container(number);
}

/**
 * A string with fixed storage, API like std::string.
 */
template <std::size_t Capacity_>
class String
{
public:
    static constexpr std::size_t Capacity = Capacity_;

    static_assert(Capacity > 0, "Capacity must be positive");

private:
    template <std::size_t C>
    friend class String;

    std::size_t len_ = 0;
    char buf_[Capacity + 1];

public:
    String() // NOLINT
    {
        buf_[len_] = '\0';
    }

    /// Implicit on purpose
    String(const char* const initializer) // NOLINT
    {
        (*this) += initializer;
    }

    template <typename InputIterator>
    String(InputIterator begin, const InputIterator end) // NOLINT
    {
        while ((begin != end) && (len_ < Capacity))
        {
            buf_[len_] = static_cast<char>(*begin);
            ++len_;
            ++begin;
        }
        buf_[len_] = '\0';
    }

    template <std::size_t C>
    String(const String<C>& initializer) // NOLINT
    {
        (*this) += initializer;
    }

    /*
     * std::string API
     */
    using value_type = char;
    using size_type = std::size_t;
    using iterator = const char*;           ///< Const is intentional
    using const_iterator = const char*;

    [[nodiscard]] constexpr std::size_t capacity() const { return Capacity; }
    [[nodiscard]] constexpr std::size_t max_size() const { return Capacity; }

    [[nodiscard]] std::size_t size()   const { return len_; }
    [[nodiscard]] std::size_t length() const { return len_; }

    [[nodiscard]] // nodiscard prevents confusion with clear()
    bool empty() const { return len_ == 0; }

    [[nodiscard]] const char* c_str() const { return &buf_[0]; }

    void clear()
    {
        len_ = 0;
        buf_[len_] = '\0';
    }

    void resize(std::size_t sz, char c = char())
    {
        while (len_ < sz)
        {
            push_back(c);
        }

        while (len_ > sz)
        {
            pop_back();
        }
    }

    void push_back(char c)
    {
        if (len_ < Capacity)
        {
            buf_[len_] = c;
            ++len_;
        }
        buf_[len_] = '\0';
    }

    void pop_back()
    {
        if (len_ > 0)
        {
            --len_;
        }
        buf_[len_] = '\0';
    }

    [[nodiscard]] char&       front()       { return operator[](0); }
    [[nodiscard]] const char& front() const { return operator[](0); }

    [[nodiscard]]
    char& back()
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
    [[nodiscard]] const char& back() const { return const_cast<String*>(this)->back(); }

    /*
     * Iterators - only const iterators are provided for safety reasons.
     * It is easy to accidentally bring the object into an invalid state by zeroing a char in the middle.
     */
    [[nodiscard]] const char* begin() const { return &buf_[0]; }
    [[nodiscard]] const char* end()   const { return &buf_[len_]; }

    /*
     * Operators
     */
    template <typename T>
    String& operator=(const T& s)
    {
        clear();
        (*this) += s;
        return *this;
    }

    template <typename T, typename = decltype(std::declval<T>().c_str())>
    String& operator+=(const T& s)
    {
        (*this) += s.c_str();
        return *this;
    }

    String& operator+=(const char* p)
    {
        while ((*p != '\0') && (len_ < Capacity))
        {
            buf_[len_] = *p++;
            ++len_;
        }
        buf_[len_] = '\0';
        return *this;
    }

    String& operator+=(char c)
    {
        push_back(c);
        return *this;
    }

    [[nodiscard]]
    char& operator[](std::size_t index)
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
    const char& operator[](std::size_t index) const
    {
        return const_cast<String*>(this)->operator[](index);
    }

    template <typename T, typename = decltype(std::declval<T>().begin())>
    [[nodiscard]]
    bool operator==(const T& s) const
    {
        return std::equal(begin(), end(), std::begin(s), std::end(s));
    }

    [[nodiscard]]
    bool operator==(const char* s) const
    {
        return 0 == std::strncmp(this->c_str(), s, sizeof(buf_));
    }

    template <typename T>
    [[nodiscard]]
    bool operator!=(const T& s) const
    {
        return !operator==(s);
    }

    /*
     * Helpers
     */
    [[nodiscard]]   // nodiscard is used to emphasize that the method is not mutating
    String<Capacity> toLowerCase() const
    {
        String<Capacity> out;
        std::transform(begin(), end(), std::back_inserter(out), [](char c) { return char(std::tolower(c)); });
        return out;
    }

    [[nodiscard]]   // nodiscard is used to emphasize that the method is not mutating
    String<Capacity> toUpperCase() const
    {
        String<Capacity> out;
        std::transform(begin(), end(), std::back_inserter(out), [](char c) { return char(std::toupper(c)); });
        return out;
    }
};

template <std::size_t LeftCapacity, std::size_t RightCapacity>
[[nodiscard]]
inline auto operator+(const String<LeftCapacity>& left, const String<RightCapacity>& right)
{
    String<LeftCapacity + RightCapacity> out(left);
    out += right;
    return out;
}

template <std::size_t Capacity>
[[nodiscard]]
inline auto operator+(const String<Capacity>& left, const char* right)
{
    String<Capacity> out(left);
    out += right;
    return out;
}

template <std::size_t Capacity>
[[nodiscard]]
inline auto operator+(const char* left, const String<Capacity>& right)
{
    String<Capacity> out(left);
    out += right;
    return out;
}

template <std::size_t Capacity>
[[nodiscard]]
inline bool operator==(const char* left, const String<Capacity>& right)
{
    return right == left;
}

template <std::size_t Capacity>
[[nodiscard]]
inline bool operator!=(const char* left, const String<Capacity>& right)
{
    return right != left;
}

}
