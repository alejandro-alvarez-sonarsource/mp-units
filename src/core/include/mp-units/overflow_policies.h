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
#endif
#endif

namespace mp_units {

// ============================================================================
// Overflow policies
//
// Each policy is a class template parameterised on a quantity type Q.
// It stores the [min, max] bounds and provides operator()(V v) that
// enforces those bounds on a quantity of compatible type.
// ============================================================================

/**
 * @brief Policy that asserts the value is within [min, max] (terminates on violation).
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

/**
 * @brief Policy that clamps the value to [min, max].
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
