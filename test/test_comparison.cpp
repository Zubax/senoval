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
#include <senoval/comparison.hpp>

// Test-only dependencies
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <cmath>

// Note that we should NOT define CATCH_CONFIG_MAIN in the same translation unit which also contains tests;
// that slows things down.
// https://github.com/catchorg/Catch2/blob/master/docs/slow-compiles.md
#include "catch.hpp"


using namespace senoval::comparison;


TEST_CASE("Comparison")
{
    // Basic same type floats
    REQUIRE(close(0.1, 0.1));
    REQUIRE(close(0.1F, 0.1F));
    REQUIRE(close(0.1L, 0.1L));

    // Basic mixed type floats
    REQUIRE(close(0.1F, 0.1));
    REQUIRE(close(0.1,  0.1F));
    REQUIRE(close(0.1F, 0.1L));
    REQUIRE(close(0.1L, 0.1F));
    REQUIRE(close(0.1,  0.1L));
    REQUIRE(close(0.1L, 0.1));

    // Non-equality
    REQUIRE_FALSE(close(-0.1, 0.1));
    REQUIRE_FALSE(close(-0.1F, 0.0L));

    REQUIRE(positive(1e-4));
    REQUIRE(!negative(1e-4));
    REQUIRE(!positive(-1e-4));
    REQUIRE(negative(-1e-4));
    REQUIRE(!positive(0));
    REQUIRE(!negative(0));
}
