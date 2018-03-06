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
#include <senoval/string.hpp>

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


template <std::uint8_t Radix = 10, typename T>
constexpr inline auto intToStringHelper(T value)
{
    auto result = convertIntToString<Radix>(value);
    return String<result.capacity()>(result);
}


TEST_CASE("IntToString")
{
    static constexpr auto A = convertIntToString(123);
    REQUIRE(String<20>(A) == "123");

    REQUIRE(intToStringHelper(0) == "0");
    REQUIRE(intToStringHelper(1) == "1");
    REQUIRE(intToStringHelper(-1) == "-1");

    REQUIRE(intToStringHelper(123456) == "123456");
    REQUIRE(intToStringHelper(-123456) == "-123456");

    REQUIRE(intToStringHelper<16>(123456) == "1e240");
    REQUIRE(intToStringHelper<2>(123456) == "11110001001000000");

    REQUIRE(intToStringHelper<10, std::int8_t>(127)  ==  "127");
    REQUIRE(intToStringHelper<10, std::int8_t>(-128) == "-128");

    REQUIRE(intToStringHelper<10, std::int16_t>(32767)  ==  "32767");
    REQUIRE(intToStringHelper<10, std::int16_t>(-32768) == "-32768");

    REQUIRE(intToStringHelper<10, std::int32_t>(2147483647)  ==  "2147483647");
    REQUIRE(intToStringHelper<10, std::int32_t>(-2147483648) == "-2147483648");

    REQUIRE(intToStringHelper<10, std::int64_t>(9223372036854775807LL)  ==  "9223372036854775807");
    REQUIRE(intToStringHelper<10, std::int64_t>(-9223372036854775807LL) == "-9223372036854775807");
}


TEST_CASE("String")
{
    String<10> s;
    REQUIRE(s.empty());
    REQUIRE(std::string(s.c_str()) == "");  // NOLINT
    REQUIRE(s == "");  // NOLINT
    REQUIRE(s != " ");
    REQUIRE(s.capacity() == 10);
    REQUIRE(s.max_size() == 10);
    REQUIRE(s.length() == 0);
    REQUIRE(s.size() == 0);  // NOLINT

    s += "123";
    REQUIRE(!s.empty());
    REQUIRE(std::string(s.c_str()) == "123");
    REQUIRE("123" == s);
    REQUIRE(" " != s);

    s += String<10>("456");
    REQUIRE(!s.empty());
    REQUIRE(std::string(s.c_str()) == "123456");
    REQUIRE(s == "123456");
    REQUIRE(s != "123");
    REQUIRE(s.capacity() == 10);
    REQUIRE(s.max_size() == 10);
    REQUIRE(s.length() == 6);
    REQUIRE(s.size() == 6);

    s += "7890a";
    REQUIRE(!s.empty());
    REQUIRE(std::string(s.c_str()) == "1234567890");
    REQUIRE(s == "1234567890");
    REQUIRE(s != "1234567890a");

    s.clear();
    REQUIRE(s.empty());
    REQUIRE(std::string(s.c_str()) == "");  // NOLINT
    REQUIRE(s == "");  // NOLINT

    s = String<30>("qwertyuiopasdfghjklzxcvbnm");
    REQUIRE(std::string(s.c_str()) == "qwertyuiop");
    REQUIRE(s == "qwertyuiop");

    s = String<30>("123");
    s += 'a';
    s += 'b';
    s += 'c';
    REQUIRE(std::string(s.c_str()) == "123abc");
    REQUIRE(s == "123abc");
    REQUIRE(s[0] == '1');
    REQUIRE(s[1] == '2');
    REQUIRE(s[2] == '3');
    REQUIRE(s[3] == 'a');
    REQUIRE(s[4] == 'b');
    REQUIRE(s[5] == 'c');
    REQUIRE(s.front() == '1');
    REQUIRE(s.back() == 'c');
    REQUIRE(*s.begin() == '1');
    REQUIRE(*(s.end() - 1) == 'c');
    REQUIRE(*s.end() == '\0');

    s = String<30>("hElLo/*-12");
    REQUIRE(s.toLowerCase() == "hello/*-12");
    REQUIRE("HELLO/*-12" == s.toUpperCase());

    auto s2 = s + String<10>(" World!");
    REQUIRE(s2.capacity() == 20);
    REQUIRE(s2.max_size() == 20);
    REQUIRE(s2.size() == 17);
    REQUIRE(s2.length() == 17);
    REQUIRE(s2 == "hElLo/*-12 World!");

    REQUIRE("[hElLo/*-12 World!]" == ("[" + s2 + "]"));

    s2.resize(20, 'Z');
    REQUIRE(s2 == "hElLo/*-12 World!ZZZ");
    s2.resize(10);
    REQUIRE(s2 == "hElLo/*-12");
    s2.resize(0);
    REQUIRE(s2.empty());
}
