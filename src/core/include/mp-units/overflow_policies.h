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

#pragma once

#include <mp-units/bits/module_macros.h>
#include <mp-units/ext/contracts.h>
#include <mp-units/framework/quantity_concepts.h>

#ifndef MP_UNITS_IN_MODULE_INTERFACE
#ifdef MP_UNITS_IMPORT_STD
import std;
#else
#include <concepts>
#include <cstdlib>
#if MP_UNITS_HOSTED
#include <stdexcept>
#endif
#endif
#endif

namespace mp_units {

// ============================================================================
// Overflow policies
//
// Each policy is a class template parameterised on a quantity type Q.
// It stores the [min, max] bounds and provides operator()(V v) that
// enforces those bounds on a quantity of compatible type.
//
// Available policies:
//   1. assert_in_range       - Contract checking (may be disabled in release)
//   2. throw_on_overflow     - Throw exception (recoverable errors) [hosted only]
//   3. terminate_on_overflow - Always terminate (safety-critical) [freestanding-safe]
//   4. clamp_to_range        - Saturate to boundaries (error correction)
//   5. wrap_to_range         - Modulo wrapping to [min, max)
//   6. reflect_in_range      - Bounce/fold at boundaries (physics)
//
// When to use:
//   - Use throw_on_overflow when overflow is an error that callers can recover from
//   - Use terminate_on_overflow in safety-critical systems requiring immediate halt
//   - Use assert_in_range for logic errors during development (debug only)
//   - Use clamp_to_range when you want to "correct" out-of-range values
//   - Use wrap_to_range for periodic/cyclic values (angles, hours)
//   - Use reflect_in_range for physical boundaries (latitude, bouncing particles)
// ============================================================================

/**
 * @brief Policy that asserts the value is within [min, max] (terminates on violation).
 *
 * Contract checking via MP_UNITS_EXPECTS - may be disabled in release builds
 * depending on contract checking configuration. For guaranteed bounds checking
 * in all builds, use terminate_on_overflow or throw_on_overflow instead.
 */
MP_UNITS_EXPORT template<Quantity Q>
struct assert_in_range {
  Q min;
  Q max;

  template<Quantity V>
  constexpr V operator()(V v) const
  {
    MP_UNITS_EXPECTS(v >= min && v <= max);
    return v;
  }
};

#if MP_UNITS_COMP_CLANG && MP_UNITS_COMP_CLANG < 17

template<Quantity Q>
assert_in_range(Q, Q) -> assert_in_range<Q>;

#endif

#if MP_UNITS_HOSTED

/**
 * @brief Policy that throws std::overflow_error when value is outside [min, max] (hosted only).
 *
 * Use this policy when overflow represents a recoverable error condition
 * that should be handled by caller code via exception handling.
 *
 * Example:
 * @code{cpp}
 * try {
 *   auto p = point_for<throw_on_overflow{0 * deg, 90 * deg}>(91 * deg);
 * } catch (const std::overflow_error& e) {
 *   // Handle out-of-range error
 * }
 * @endcode
 */
MP_UNITS_EXPORT template<Quantity Q>
struct throw_on_overflow {
  Q min;
  Q max;

  template<Quantity V>
  constexpr V operator()(V v) const
  {
    if (v < min || v > max) {
      throw std::overflow_error("Value out of bounds");
    }
    return v;
  }
};

#if MP_UNITS_COMP_CLANG && MP_UNITS_COMP_CLANG < 17

template<Quantity Q>
throw_on_overflow(Q, Q) -> throw_on_overflow<Q>;

#endif

#endif  // MP_UNITS_HOSTED

/**
 * @brief Policy that terminates the program when value is outside [min, max] (freestanding-safe).
 *
 * Use this policy in safety-critical systems where overflow represents
 * an unrecoverable error that must halt execution immediately.
 * Provides a diagnostic message before terminating.
 *
 * Unlike assert_in_range (which may be disabled in release builds),
 * terminate_on_overflow ALWAYS checks bounds and ALWAYS terminates.
 *
 * Example:
 * @code{cpp}
 * // For safety-critical systems:
 * auto critical_sensor = point_for<terminate_on_overflow{-100 * deg, 100 * deg}>(value);
 * // Out-of-range values will print diagnostic and terminate
 * @endcode
 */
MP_UNITS_EXPORT template<Quantity Q>
struct terminate_on_overflow {
  Q min;
  Q max;

  template<Quantity V>
  constexpr V operator()(V v) const
  {
    if (v < min || v > max) {
      std::abort();
    }
    return v;
  }
};

#if MP_UNITS_COMP_CLANG && MP_UNITS_COMP_CLANG < 17

template<Quantity Q>
terminate_on_overflow(Q, Q) -> terminate_on_overflow<Q>;

#endif

/**
 * @brief Policy that clamps the value to [min, max].
 *
 * Saturates out-of-range values to the nearest boundary.
 * Use when you want to "correct" invalid values rather than signal an error.
 */
MP_UNITS_EXPORT template<Quantity Q>
struct clamp_to_range {
  Q min;
  Q max;

  template<Quantity V>
  constexpr V operator()(V v) const
  {
    if (v < min) return V{min};
    if (v > max) return V{max};
    return v;
  }
};

#if MP_UNITS_COMP_CLANG && MP_UNITS_COMP_CLANG < 17

template<Quantity Q>
clamp_to_range(Q, Q) -> clamp_to_range<Q>;

#endif

/**
 * @brief Policy that wraps the value into the half-open range [min, max).
 *
 * Uses modulo arithmetic to wrap values into the range.
 * Use for periodic/cyclic quantities (angles, time-of-day, etc.).
 * For example, with [0°, 360°): 370° -> 10°, -10° -> 350°.
 */
MP_UNITS_EXPORT template<Quantity Q>
struct wrap_to_range {
  Q min;
  Q max;

  template<Quantity V>
  constexpr V operator()(V v) const
  {
    const quantity range = max - min;
    while (v >= max) v -= range;
    while (v < min) v += range;
    return v;
  }
};

#if MP_UNITS_COMP_CLANG && MP_UNITS_COMP_CLANG < 17

template<Quantity Q>
wrap_to_range(Q, Q) -> wrap_to_range<Q>;

#endif

/**
 * @brief Policy that reflects (folds) the value at both boundaries.
 *
 * Values that exceed [min, max] are "bounced back" from the boundary.
 * For example, with [-90, 90] (latitude): 91 -> 89, 180 -> 0, 270 -> -90.
 */
MP_UNITS_EXPORT template<Quantity Q>
struct reflect_in_range {
  Q min;
  Q max;

  template<Quantity V>
  constexpr V operator()(V v) const
  {
    const quantity range = max - min;
    const quantity period = V{2 * range};
    while (v >= V{min + period}) v -= period;
    while (v < min) v += period;
    if (v > max) v = V{2 * max} - v;
    return v;
  }
};

#if MP_UNITS_COMP_CLANG && MP_UNITS_COMP_CLANG < 17

template<Quantity Q>
reflect_in_range(Q, Q) -> reflect_in_range<Q>;

#endif

}  // namespace mp_units
