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

// We want to ensure that assertion checks are enabled when tests are run, for extra safety
#ifdef NDEBUG
# undef NDEBUG
#endif

// The library should be included first in order to ensure that all necessary headers are included in the library itself
#include <senoval/vector.hpp>

// Test-only dependencies
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <cmath>

// Note that we should NOT define CATCH_CONFIG_MAIN in the same translation unit which also contains tests;
// that slows things down.
// https://github.com/catchorg/Catch2/blob/master/docs/slow-compiles.md
#include "catch.hpp"


using namespace senoval;


TEST_CASE("Vector")
{
    Vector<std::int32_t, 10> vec;

    REQUIRE(sizeof(vec) == 40 + sizeof(std::size_t));
    REQUIRE(vec.empty());
    REQUIRE(vec.capacity() == 10);
    REQUIRE(vec.max_size() == 10);
    REQUIRE(vec.size() == 0);
    REQUIRE(vec.begin() == vec.end());

    vec.push_back(1);
    REQUIRE(!vec.empty());
    REQUIRE(vec.capacity() == 10);
    REQUIRE(vec.max_size() == 10);
    REQUIRE(vec.size() == 1);
    REQUIRE(vec.begin() != vec.end());
    REQUIRE(1 == *vec.begin());
    REQUIRE(1 == *(vec.end() - 1));
    REQUIRE(1 == vec.front());
    REQUIRE(1 == vec.back());

    vec.push_back(2);
    REQUIRE(!vec.empty());
    REQUIRE(vec.capacity() == 10);
    REQUIRE(vec.max_size() == 10);
    REQUIRE(vec.size() == 2);
    REQUIRE(vec.begin() != vec.end());
    REQUIRE(1 == *vec.begin());
    REQUIRE(2 == *(vec.end() - 1));
    REQUIRE(1 == vec.front());
    REQUIRE(2 == vec.back());

    vec.push_back(3);
    vec.push_back(4);
    vec.push_back(5);
    vec.push_back(6);
    vec.push_back(7);
    vec.push_back(8);
    vec.push_back(9);
    vec.push_back(10);
    REQUIRE(!vec.empty());
    REQUIRE(vec.size() == 10);
    REQUIRE(1 == *vec.begin());
    REQUIRE(10 == *(vec.end() - 1));
    REQUIRE(1 == vec.front());
    REQUIRE(10 == vec.back());

    vec.pop_back();
    vec.pop_back();
    REQUIRE(!vec.empty());
    REQUIRE(vec.size() == 8);
    REQUIRE(1 == *vec.begin());
    REQUIRE(8 == *(vec.end() - 1));
    REQUIRE(1 == vec.front());
    REQUIRE(8 == vec.back());

    vec.push_back(9);
    vec.push_back(10);

    const std::int8_t arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    const Vector<std::int8_t, 80> vec2(std::begin(arr), std::end(arr));
    REQUIRE(sizeof(vec2) == 80 + sizeof(std::size_t));
    REQUIRE(!vec2.empty());
    REQUIRE(vec2.capacity() == 80);
    REQUIRE(vec2.max_size() == 80);
    REQUIRE(vec2.size() == 10);
    REQUIRE(vec2.begin() != vec2.end());
    REQUIRE(1 == *vec2.begin());
    REQUIRE(10 == *(vec2.end() - 1));
    REQUIRE(1 == vec2.front());
    REQUIRE(10 == vec2.back());

    REQUIRE(vec == vec2);
    REQUIRE(vec2 == vec);
    REQUIRE((vec != vec2) == false);
    REQUIRE((vec2 != vec) == false);

    vec[3] = -3;

    REQUIRE(vec != vec2);
    REQUIRE(vec2 != vec);
    REQUIRE((vec == vec2) == false);
    REQUIRE((vec2 == vec) == false);

    const Vector<std::int32_t, 10> vec_copy = vec;
    REQUIRE(!vec_copy.empty());
    REQUIRE(vec_copy.capacity() == 10);
    REQUIRE(vec_copy.max_size() == 10);
    REQUIRE(vec_copy.size() == 10);
    REQUIRE(vec_copy.begin() != vec_copy.end());

    REQUIRE(vec_copy != vec2);
    REQUIRE(vec2 != vec_copy);
    REQUIRE(vec == vec_copy);
    REQUIRE(vec_copy == vec);
    REQUIRE((vec_copy == vec2) == false);
    REQUIRE((vec2 == vec_copy) == false);
    REQUIRE((vec != vec_copy) == false);
    REQUIRE((vec_copy != vec) == false);

    vec.clear();
    REQUIRE(vec.empty());
    REQUIRE(vec.capacity() == 10);
    REQUIRE(vec.max_size() == 10);
    REQUIRE(vec.size() == 0);
    REQUIRE(vec.begin() == vec.end());

    REQUIRE(vec != vec2);
    REQUIRE(vec2 != vec);
    REQUIRE(vec != vec_copy);
    REQUIRE(vec_copy != vec);
    REQUIRE((vec == vec2) == false);
    REQUIRE((vec2 == vec) == false);
    REQUIRE((vec == vec_copy) == false);
    REQUIRE((vec_copy == vec) == false);

    {
        Vector<std::int32_t, 6> vec3(5, 123);
        REQUIRE(!vec3.empty());
        REQUIRE(vec3.capacity() == 6);
        REQUIRE(vec3.max_size() == 6);
        REQUIRE(vec3.size() == 5);
        REQUIRE(vec3[0] == 123);
        REQUIRE(vec3[1] == 123);
        REQUIRE(vec3[2] == 123);
        REQUIRE(vec3[3] == 123);
        REQUIRE(vec3[4] == 123);
    }

    {
        Vector<std::int8_t, 7> vec4({1, 2, 3, 4});
        REQUIRE(!vec4.empty());
        REQUIRE(vec4.capacity() == 7);
        REQUIRE(vec4.max_size() == 7);
        REQUIRE(vec4.size() == 4);
        REQUIRE(vec4[0] == 1);
        REQUIRE(vec4[1] == 2);
        REQUIRE(vec4[2] == 3);
        REQUIRE(vec4[3] == 4);
    }
}
