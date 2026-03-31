// The MIT License (MIT)
//
// Copyright (c) 2018 Mateusz Pusz
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <catch2/catch_test_macros.hpp>
#include <mp-units/framework.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si.h>
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <stdexcept>
#endif

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

#if MP_UNITS_HOSTED

// ============================================================================
// throw_on_overflow policy tests
// ============================================================================

namespace {

QUANTITY_SPEC(test_angle_throw, isq::angular_measure);

inline constexpr struct throw_origin final : absolute_point_origin<test_angle_throw> {
} throw_origin;

}  // namespace

template<>
inline constexpr auto mp_units::quantity_bounds<throw_origin> = mp_units::throw_on_overflow{-90 * deg, 90 * deg};

namespace {

using qp_throw = quantity_point<test_angle_throw[deg], throw_origin, double>;

}  // namespace

TEST_CASE("throw_on_overflow", "[bounded][throw]")
{
  SECTION("values within bounds do not throw")
  {
    CHECK_NOTHROW(qp_throw(45.0 * deg, throw_origin));
    CHECK_NOTHROW(qp_throw(0.0 * deg, throw_origin));
    CHECK_NOTHROW(qp_throw(90.0 * deg, throw_origin));
    CHECK_NOTHROW(qp_throw(-90.0 * deg, throw_origin));
  }

  SECTION("value above max throws")
  {
    CHECK_THROWS_AS(qp_throw(91.0 * deg, throw_origin), std::overflow_error);
    CHECK_THROWS_AS(qp_throw(180.0 * deg, throw_origin), std::overflow_error);
  }

  SECTION("value below min throws")
  {
    CHECK_THROWS_AS(qp_throw(-91.0 * deg, throw_origin), std::overflow_error);
    CHECK_THROWS_AS(qp_throw(-180.0 * deg, throw_origin), std::overflow_error);
  }

  SECTION("arithmetic resulting in out-of-bounds throws")
  {
    auto pt = qp_throw(85.0 * deg, throw_origin);
    CHECK_THROWS_AS(pt += 10.0 * deg, std::overflow_error);

    auto pt2 = qp_throw(-85.0 * deg, throw_origin);
    CHECK_THROWS_AS(pt2 -= 10.0 * deg, std::overflow_error);
  }

  SECTION("arithmetic within bounds does not throw")
  {
    auto pt = qp_throw(45.0 * deg, throw_origin);
    CHECK_NOTHROW(pt += 30.0 * deg);
    CHECK(pt.quantity_from(throw_origin) == 75.0 * deg);

    auto pt2 = qp_throw(-45.0 * deg, throw_origin);
    CHECK_NOTHROW(pt2 -= 30.0 * deg);
    CHECK(pt2.quantity_from(throw_origin) == -75.0 * deg);
  }

  SECTION("increment out-of-bounds throws")
  {
    using qp_throw_int = quantity_point<test_angle_throw[deg], throw_origin, int>;
    auto pt = qp_throw_int(90 * deg, throw_origin);
    CHECK_THROWS_AS(++pt, std::overflow_error);

    auto pt2 = qp_throw_int(-90 * deg, throw_origin);
    CHECK_THROWS_AS(--pt2, std::overflow_error);
  }

  SECTION("postfix increment out-of-bounds throws")
  {
    using qp_throw_int = quantity_point<test_angle_throw[deg], throw_origin, int>;
    auto pt = qp_throw_int(90 * deg, throw_origin);
    CHECK_THROWS_AS(pt++, std::overflow_error);

    auto pt2 = qp_throw_int(-90 * deg, throw_origin);
    CHECK_THROWS_AS(pt2--, std::overflow_error);
  }

  SECTION("increment within bounds does not throw")
  {
    using qp_throw_int = quantity_point<test_angle_throw[deg], throw_origin, int>;
    auto pt = qp_throw_int(45 * deg, throw_origin);
    CHECK_NOTHROW(++pt);
    CHECK(pt.quantity_from(throw_origin) == 46 * deg);

    auto pt2 = qp_throw_int(45 * deg, throw_origin);
    CHECK_NOTHROW(pt2++);
    CHECK(pt2.quantity_from(throw_origin) == 46 * deg);
  }

  SECTION("exception message is meaningful")
  {
    try {
      qp_throw(91.0 * deg, throw_origin);
      FAIL("Expected std::overflow_error to be thrown");
    } catch (const std::overflow_error& e) {
      CHECK(std::string(e.what()).length() > 0);
    }
  }
}

#endif  // MP_UNITS_HOSTED
