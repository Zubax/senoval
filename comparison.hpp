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

#include <cmath>
#include <limits>
#include <cstdlib>
#include <algorithm>
#include <type_traits>


namespace senoval
{
/**
 * The following functions defined in this namespace are of interest:
 *
 *  - close()           Compare two values. If the values are floats, perform fuzzy comparison,
 *                      otherwise perform exact comparison.
 *
 *  - closeToZero()     Like close(), where the second argument is set to zero of the appropriate type.
 *
 *  - equal()           Perform equality comparison in a way that doesn't trigger compiler warnings.
 *                      For floats, the method is (x <= y) && (x >= y); otherwise the plain operator == is used.
 */
namespace comparison
{
/**
 * Float comparison precision. The default should be safe in all meaningful use cases.
 * For details refer to:
 *  http://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
 *  https://code.google.com/p/googletest/source/browse/trunk/include/gtest/internal/gtest-internal.h
 */
#ifdef SENOVAL_DEFAULT_FLOAT_COMPARISON_EPSILON_MULT
static constexpr unsigned DefaultEpsilonMultiplier = DEFAULT_FLOAT_COMPARISON_EPSILON_MULT;
#else
static constexpr unsigned DefaultEpsilonMultiplier = 10;
#endif

/**
 * Exact comparison of two floats that suppresses compiler warnings.
 * Most of the time you DON'T want to use this function! Consider using close() instead.
 */
template <typename T>
inline bool equal(const T& left,
                  const T& right)
{
    if constexpr (std::is_floating_point_v<std::decay_t<T>>)
    {
        return (left <= right) && (left >= right);
    }
    else
    {
        return left == right;
    }
}

/**
 * This function performs fuzzy comparison of two floating point numbers.
 * Type of T can be either float, double or long double.
 * For details refer to http://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
 * See also: @ref DEFAULT_FLOAT_COMPARISON_EPSILON_MULT.
 */
template <typename T>
inline bool close(const T a,
                  const T b,
                  const T absolute_epsilon,
                  const T relative_epsilon)
{
    // NAN
    if (std::isnan(a) || std::isnan(b))
    {
        return false;
    }

    // Infinity
    if (std::isinf(a) || std::isinf(b))
    {
        return equal(a, b);
    }

    // Close numbers near zero
    if (std::abs(a - b) <= absolute_epsilon)
    {
        return true;
    }

    // General case
    return std::abs(a - b) <= (std::max(std::abs(a), std::abs(b)) * relative_epsilon);
}

/*
 * Float comparison specializations
 */
inline bool close(const float left,
                  const float right)
{
    return close(left, right,
                 std::numeric_limits<float>::epsilon(),
                 std::numeric_limits<float>::epsilon() * DefaultEpsilonMultiplier);
}

inline bool close(const double left,
                  const double right)
{
    return close(left, right,
                 std::numeric_limits<double>::epsilon(),
                 std::numeric_limits<double>::epsilon() * DefaultEpsilonMultiplier);
}

inline bool close(const long double left,
                  const long double right)
{
    return close(left, right,
                 std::numeric_limits<long double>::epsilon(),
                 std::numeric_limits<long double>::epsilon() * DefaultEpsilonMultiplier);
}

/*
 * Mixed floating type comparison - coercing larger type to smaller type
 */
inline bool close(const float left,
                  const double right)
{
    return close(left, static_cast<float>(right));
}

inline bool close(const double left,
                  const float right)
{
    return close(static_cast<float>(left), right);
}

inline bool close(const float left,
                  const long double right)
{
    return close(left, static_cast<float>(right));
}

inline bool close(const long double left,
                  const float right)
{
    return close(static_cast<float>(left), right);
}

inline bool close(const double left,
                  const long double right)
{
    return close(left, static_cast<double>(right));
}

inline bool close(const long double left,
                  const double right)
{
    return close(static_cast<double>(left), right);
}

/**
 * Comparison against zero.
 * Helps to compare a floating point number against zero if the exact type is unknown.
 * For non-floating point types performs exact comparison against an object of the same type constructed from
 * integer zero.
 */
template <typename T>
inline bool closeToZero(const T& x)
{
    if constexpr (std::is_floating_point_v<std::decay_t<T>>)
    {
        return close(x, T(0));
    }
    else
    {
        return x == T(0);
    }
}

template <typename T>
inline bool positive(const T& x)
{
    return (x > T(0)) && !closeToZero(x);
}

template <typename T>
inline bool negative(const T& x)
{
    return (x < T(0)) && !closeToZero(x);
}

}
}
