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

#include <mp-units/compat_macros.h>
#include <mp-units/framework.h>
#include <mp-units/systems/isq/space_and_time.h>
#include <mp-units/systems/si.h>

namespace {

using namespace mp_units;
using namespace mp_units::si::unit_symbols;

// ============================================================================
// Test quantity specifications and origins with bounds
// ============================================================================

QUANTITY_SPEC(test_angle_clamp, isq::angular_measure);
QUANTITY_SPEC(test_angle_wrap, isq::angular_measure);
QUANTITY_SPEC(test_angle_reflect, isq::angular_measure);

inline constexpr struct clamp_origin final : absolute_point_origin<test_angle_clamp> {
} clamp_origin;
inline constexpr struct wrap_origin final : absolute_point_origin<test_angle_wrap> {
} wrap_origin;
inline constexpr struct reflect_origin final : absolute_point_origin<test_angle_reflect> {
} reflect_origin;

// Separate origins for unit conversion tests (need double bounds)
QUANTITY_SPEC(test_angle_clamp_convert, isq::angular_measure);
QUANTITY_SPEC(test_angle_wrap_convert, isq::angular_measure);

inline constexpr struct clamp_convert_origin final : absolute_point_origin<test_angle_clamp_convert> {
} clamp_convert_origin;
inline constexpr struct wrap_convert_origin final : absolute_point_origin<test_angle_wrap_convert> {
} wrap_convert_origin;

// Origins for testing point_for() with bounds
QUANTITY_SPEC(bounded_altitude, isq::height);

inline constexpr struct altitude_msl final : absolute_point_origin<bounded_altitude> {
} altitude_msl;

}  // namespace

template<>
inline constexpr auto mp_units::quantity_bounds<clamp_origin> = mp_units::clamp_to_range{-90 * deg, 90 * deg};

template<>
inline constexpr auto mp_units::quantity_bounds<wrap_origin> = mp_units::wrap_to_range{-180 * deg, 180 * deg};

template<>
inline constexpr auto mp_units::quantity_bounds<reflect_origin> = mp_units::reflect_in_range{-90 * deg, 90 * deg};

// Bounds for unit conversion tests (use double to support degree-to-radian conversions)
template<>
inline constexpr auto mp_units::quantity_bounds<clamp_convert_origin> =
  mp_units::clamp_to_range{-90.0 * deg, 90.0 * deg};

template<>
inline constexpr auto mp_units::quantity_bounds<wrap_convert_origin> =
  mp_units::wrap_to_range{-180.0 * deg, 180.0 * deg};

// Altitude bounds: [-100m, 1000m] for MSL
template<>
inline constexpr auto mp_units::quantity_bounds<altitude_msl> = mp_units::clamp_to_range{-100 * m, 1000 * m};

// Relative origins for altitude with different bounds to test point_for() clamping
inline constexpr struct ground_level_bounded final : mp_units::relative_point_origin<altitude_msl + 100 * m> {
} ground_level_bounded;
inline constexpr struct tower_base_bounded final : mp_units::relative_point_origin<altitude_msl + 200 * m> {
} tower_base_bounded;

// ground_level has narrower bounds to trigger clamping during point_for()
template<>
inline constexpr auto mp_units::quantity_bounds<ground_level_bounded> = mp_units::clamp_to_range{-50 * m, 500 * m};

// tower_base has same bounds as MSL
template<>
inline constexpr auto mp_units::quantity_bounds<tower_base_bounded> = mp_units::clamp_to_range{-100 * m, 1000 * m};

namespace {

using qp_clamp = quantity_point<test_angle_clamp[deg], clamp_origin, double>;
using qp_wrap = quantity_point<test_angle_wrap[deg], wrap_origin, double>;
using qp_reflect = quantity_point<test_angle_reflect[deg], reflect_origin, double>;
using qp_clamp_int = quantity_point<test_angle_clamp[deg], clamp_origin, int>;

// Type aliases for unit conversion tests (need double bounds)
using qp_clamp_convert = quantity_point<test_angle_clamp_convert[deg], clamp_convert_origin, double>;
using qp_wrap_convert = quantity_point<test_angle_wrap_convert[deg], wrap_convert_origin, double>;

// ============================================================================
// Clamp policy
// ============================================================================

static_assert(qp_clamp(45.0 * deg, clamp_origin).quantity_from(clamp_origin) == 45.0 * deg);
static_assert(qp_clamp(100.0 * deg, clamp_origin).quantity_from(clamp_origin) == 90.0 * deg);
static_assert(qp_clamp(-120.0 * deg, clamp_origin).quantity_from(clamp_origin) == -90.0 * deg);
static_assert(qp_clamp(90.0 * deg, clamp_origin).quantity_from(clamp_origin) == 90.0 * deg);
static_assert(qp_clamp(-90.0 * deg, clamp_origin).quantity_from(clamp_origin) == -90.0 * deg);

// ============================================================================
// Wrap policy
// ============================================================================

static_assert(qp_wrap(90.0 * deg, wrap_origin).quantity_from(wrap_origin) == 90.0 * deg);
static_assert(qp_wrap(200.0 * deg, wrap_origin).quantity_from(wrap_origin) == -160.0 * deg);
static_assert(qp_wrap(-200.0 * deg, wrap_origin).quantity_from(wrap_origin) == 160.0 * deg);
static_assert(qp_wrap(180.0 * deg, wrap_origin).quantity_from(wrap_origin) == -180.0 * deg);

// ============================================================================
// Reflect policy
// ============================================================================

static_assert(qp_reflect(45.0 * deg, reflect_origin).quantity_from(reflect_origin) == 45.0 * deg);
static_assert(qp_reflect(91.0 * deg, reflect_origin).quantity_from(reflect_origin) == 89.0 * deg);
static_assert(qp_reflect(-91.0 * deg, reflect_origin).quantity_from(reflect_origin) == -89.0 * deg);
static_assert(qp_reflect(90.0 * deg, reflect_origin).quantity_from(reflect_origin) == 90.0 * deg);
static_assert(qp_reflect(-90.0 * deg, reflect_origin).quantity_from(reflect_origin) == -90.0 * deg);

// ============================================================================
// Mutating operators enforce bounds
// ============================================================================

consteval bool clamp_plus_assign()
{
  auto pt = qp_clamp(80.0 * deg, clamp_origin);
  pt += 20.0 * deg;
  return pt.quantity_from(clamp_origin) == 90.0 * deg;
}
static_assert(clamp_plus_assign());

consteval bool clamp_minus_assign()
{
  auto pt = qp_clamp(-80.0 * deg, clamp_origin);
  pt -= 20.0 * deg;
  return pt.quantity_from(clamp_origin) == -90.0 * deg;
}
static_assert(clamp_minus_assign());

consteval bool clamp_pre_increment()
{
  auto pt = qp_clamp_int(90 * deg, clamp_origin);
  ++pt;
  return pt.quantity_from(clamp_origin) == 90 * deg;
}
static_assert(clamp_pre_increment());

consteval bool clamp_pre_decrement()
{
  auto pt = qp_clamp_int(-90 * deg, clamp_origin);
  --pt;
  return pt.quantity_from(clamp_origin) == -90 * deg;
}
static_assert(clamp_pre_decrement());

consteval bool clamp_post_increment()
{
  auto pt = qp_clamp_int(89 * deg, clamp_origin);
  auto old_value = pt++;
  // Check old value returned
  if (old_value.quantity_from(clamp_origin) != 89 * deg) return false;
  // Check new value is clamped to max
  return pt.quantity_from(clamp_origin) == 90 * deg;
}
static_assert(clamp_post_increment());

consteval bool clamp_post_decrement()
{
  auto pt = qp_clamp_int(-89 * deg, clamp_origin);
  auto old_value = pt--;
  // Check old value returned
  if (old_value.quantity_from(clamp_origin) != -89 * deg) return false;
  // Check new value is clamped to min
  return pt.quantity_from(clamp_origin) == -90 * deg;
}
static_assert(clamp_post_decrement());

consteval bool wrap_post_increment()
{
  auto pt_wrap_int = quantity_point<test_angle_wrap[deg], wrap_origin, int>(179 * deg, wrap_origin);
  auto old_value = pt_wrap_int++;
  // Check old value returned
  if (old_value.quantity_from(wrap_origin) != 179 * deg) return false;
  // Check new value wraps to -180 (since range is [-180, 180))
  return pt_wrap_int.quantity_from(wrap_origin) == -180 * deg;
}
static_assert(wrap_post_increment());

// ============================================================================
// Unit conversion with bounded quantity_points
// ============================================================================

// Simple scaling: degrees to radians and back
consteval bool angle_unit_conversion()
{
  auto pt = qp_clamp_convert(45.0 * deg, clamp_convert_origin);
  auto in_rad = pt.in(rad);
  // 45° in radians
  constexpr auto expected_rad = (45.0 * deg).numerical_value_in(rad);
  return in_rad.quantity_from(clamp_convert_origin).numerical_value_in(rad) == expected_rad;
}
static_assert(angle_unit_conversion());

consteval bool clamp_with_unit_scaling()
{
  // Create with radians (exceeds bounds in degrees: π rad = 180°), should clamp to 90°
  constexpr auto pi_rad = 1.0 * pi * rad;
  auto pt = qp_clamp_convert(pi_rad, clamp_convert_origin);
  // Expected: clamped to 90°
  constexpr auto expected = 90.0 * deg;
  return pt.quantity_from(clamp_convert_origin) == expected;
}
static_assert(clamp_with_unit_scaling());

// Wrap with unit scaling
consteval bool wrap_with_unit_scaling()
{
  // 400° wraps to 40° (400 - 360 = 40)
  auto pt = qp_wrap_convert(400.0 * deg, wrap_convert_origin);
  const auto in_rad = pt.in(rad);
  const auto back_in_deg = in_rad.quantity_from(wrap_convert_origin);
  // Wrapped value: 400 - 360 = 40, which is in [-180, 180)
  return back_in_deg == 40.0 * deg;
}
static_assert(wrap_with_unit_scaling());

// ============================================================================
// Temperature conversion with offset units
// ============================================================================

// Temperature origin with bounds in kelvin
QUANTITY_SPEC(bounded_temperature, isq::thermodynamic_temperature);

inline constexpr struct temp_origin final : absolute_point_origin<bounded_temperature> {
} temp_origin;

}  // namespace

// Temperature bounds: [200K, 400K] with clamping
template<>
inline constexpr auto mp_units::quantity_bounds<temp_origin> = mp_units::clamp_to_range{
  delta<bounded_temperature[si::kelvin]>(200.0), delta<bounded_temperature[si::kelvin]>(400.0)};

namespace {

using qp_temp = quantity_point<bounded_temperature[si::kelvin], temp_origin, double>;

// Value within bounds
static_assert(qp_temp(delta<si::kelvin>(300.0), temp_origin).quantity_from(temp_origin) == delta<si::kelvin>(300.0));

// Above max clamps
static_assert(qp_temp(delta<si::kelvin>(500.0), temp_origin).quantity_from(temp_origin) == delta<si::kelvin>(400.0));

// Below min clamps
static_assert(qp_temp(delta<si::kelvin>(100.0), temp_origin).quantity_from(temp_origin) == delta<si::kelvin>(200.0));

// Convert to millikelvin (simple scaling)
consteval bool temperature_unit_scaling()
{
  auto pt = qp_temp(delta<si::kelvin>(300.0), temp_origin);
  auto in_mK = pt.in(si::milli<si::kelvin>);
  // 300 K = 300'000 mK
  return in_mK.quantity_from(temp_origin).numerical_value_in(si::milli<si::kelvin>) == 300'000.0;
}
static_assert(temperature_unit_scaling());

// Creating with millikelvin (out of bounds) clamps correctly
consteval bool temperature_clamp_with_millikelvin()
{
  // 500'000 mK = 500 K, which exceeds max of 400 K
  auto pt = qp_temp(delta<si::milli<si::kelvin>>(500'000.0), temp_origin);
  // Should clamp to 400 K = 400'000 mK
  return pt.in(si::milli<si::kelvin>).quantity_from(temp_origin) == delta<si::milli<si::kelvin>>(400'000.0);
}
static_assert(temperature_clamp_with_millikelvin());

// ============================================================================
// Conversions between different origins (with offsets)
// ============================================================================

// Test conversion between absolute_zero and ice_point origins
// ice_point is at 273.15K above absolute_zero
consteval bool convert_between_offset_origins()
{
  // Create a bounded point at temp_origin: 300K
  auto pt_temp = qp_temp(delta<bounded_temperature[si::kelvin]>(300.0), temp_origin);

  // Convert to quantity_point with si::absolute_zero origin
  // This requires going through the quantity: quantity_from gives us the displacement
  const auto q = pt_temp.quantity_from(temp_origin);
  auto pt_abs_zero = si::absolute_zero + q;

  // Now convert to ice_point (which is 273.15K above absolute_zero)
  const auto q_from_ice = pt_abs_zero.quantity_from(si::ice_point);

  // 300K from absolute_zero = (300 - 273.15)K from ice_point = 26.85K
  // Use approximate comparison due to floating point precision
  constexpr auto expected = 300.0 - 273.15;
  const auto actual = q_from_ice.numerical_value_in(si::kelvin);
  return (actual >= expected - 0.0001) && (actual <= expected + 0.0001);
}
static_assert(convert_between_offset_origins());

// ============================================================================
// point_for() with bounded quantity points enforces bounds
// ============================================================================

using qp_altitude_msl = quantity_point<bounded_altitude[m], altitude_msl, double>;
using qp_ground_bounded = quantity_point<bounded_altitude[m], ground_level_bounded, double>;
using qp_tower_bounded = quantity_point<bounded_altitude[m], tower_base_bounded, double>;

consteval bool point_for_from_relative_to_absolute_enforces_bounds()
{
  // Create a point at 700m from altitude_msl (valid for MSL's bounds [-100m, 1000m])
  auto pt = qp_altitude_msl(700.0 * m, altitude_msl);

  // Convert to ground_level_bounded using point_for()
  // Displacement from ground: 700m - 100m = 600m
  // This exceeds ground_level's max of 500m, should clamp to 500m
  auto pt_ground = pt.point_for(ground_level_bounded);
  return pt_ground.quantity_from(ground_level_bounded) == 500.0 * m;
}
static_assert(point_for_from_relative_to_absolute_enforces_bounds());

consteval bool point_for_between_relative_origins_enforces_bounds()
{
  // Create a point at 700m from tower_base_bounded (valid for tower's bounds [-100m, 1000m])
  auto pt = qp_tower_bounded(700.0 * m, tower_base_bounded);

  // Convert to ground_level_bounded using point_for()
  // tower_base = ground_level + 100m, so displacement from ground = 700m + 100m = 800m
  // This exceeds ground_level's max of 500m, should clamp to 500m
  auto pt_ground = pt.point_for(ground_level_bounded);
  return pt_ground.quantity_from(ground_level_bounded) == 500.0 * m;
}
static_assert(point_for_between_relative_origins_enforces_bounds());

// ============================================================================
// terminate_on_overflow policy (successful paths)
// ============================================================================

// Origin with terminate policy
QUANTITY_SPEC(test_angle_terminate, isq::angular_measure);

inline constexpr struct terminate_origin final : absolute_point_origin<test_angle_terminate> {
} terminate_origin;

}  // namespace

template<>
inline constexpr auto mp_units::quantity_bounds<terminate_origin> =
  mp_units::terminate_on_overflow{-90 * deg, 90 * deg};

namespace {

using qp_terminate = quantity_point<test_angle_terminate[deg], terminate_origin, double>;

// Values within bounds should work fine
static_assert(qp_terminate(45.0 * deg, terminate_origin).quantity_from(terminate_origin) == 45.0 * deg);
static_assert(qp_terminate(0.0 * deg, terminate_origin).quantity_from(terminate_origin) == 0.0 * deg);
static_assert(qp_terminate(90.0 * deg, terminate_origin).quantity_from(terminate_origin) == 90.0 * deg);
static_assert(qp_terminate(-90.0 * deg, terminate_origin).quantity_from(terminate_origin) == -90.0 * deg);

// Arithmetic within bounds
consteval bool terminate_arithmetic_within_bounds()
{
  auto pt = qp_terminate(45.0 * deg, terminate_origin);
  pt += 30.0 * deg;  // Result: 75°, within bounds
  return pt.quantity_from(terminate_origin) == 75.0 * deg;
}
static_assert(terminate_arithmetic_within_bounds());

consteval bool terminate_increment_within_bounds()
{
  using qp_terminate_int = quantity_point<test_angle_terminate[deg], terminate_origin, int>;
  auto pt = qp_terminate_int(45 * deg, terminate_origin);
  ++pt;  // Result: 46°, within bounds
  return pt.quantity_from(terminate_origin) == 46 * deg;
}
static_assert(terminate_increment_within_bounds());

// Note: Out-of-bounds cases for terminate_on_overflow cannot be tested at compile time
// since they result in std::terminate(). These are tested in runtime tests instead.

}  // namespace
