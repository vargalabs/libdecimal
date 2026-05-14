# `scaled.hpp`

## Overview

`math::scaled::decimal_t<significand_t, exponent_t>` is a minimal, allocation-free decimal type based on a (significand, exponent) pair:

$$x = m \cdot 10^e, \quad m \in \mathbb{Z},\ e \in \mathbb{Z}$$

Sign is encoded directly in the significand (when `significand_t` is signed). There are no NaN or Inf values — the type operates in the finite domain only.

---

## Declaration

```cpp
namespace math::scaled {
    template <class significand_t, class exponent_t = std::int16_t>
    struct decimal_t;
}
```

`significand_t` must be integral. Signed `significand_t` gives full positive/negative range. Unsigned restricts to non-negative values.

---

## Construction

```cpp
using d64 = math::scaled::decimal_t<int64_t>;

d64 x{12345, -2};           // 123.45  (significand=12345, exponent=-2)
d64 y{-50,   -2};           // -0.50
d64 z{};                    // 0

explicit d64 from_int{42};  // significand=42, exponent=0
```

Construction calls `normalize()` which strips trailing decimal zeros from the significand, incrementing the exponent accordingly:

```cpp
d64{1000, -3}  →  {1, 0}    // 1.000 → 1e0
d64{500,  -2}  →  {5, -1}   // 5.00  → 5e-1
```

---

## Arithmetic

```cpp
d64 a{12345, -2};   // 123.45
d64 b{50,    -2};   //   0.50

d64 sum  = a + b;   // 123.95  — exponent aligned, widened intermediate
d64 diff = a - b;   // 122.95
d64 prod = a * b;   //  61.725 — exponents add
d64 quot = a / b;   // 246     — integer division of aligned significands
```

Addition and subtraction align exponents using `__int128` widening to avoid intermediate overflow. Multiplication adds exponents. Division is integer quotient only — no fractional rescue.

---

## Comparison

`operator<=>` returns `std::strong_ordering` (not `partial_ordering`):

```cpp
d64 p{100, -2};   // 1.00
d64 q{1,    0};   // 1

bool eq = (p == q);                               // true — aligned comparison
auto ord = (p <=> q);                             // strong_ordering::equal
```

`operator==` also aligns exponents before comparing.

---

## Decomposition

```cpp
auto [sig, exp] = x.as_pair();   // std::pair<significand_t, exponent_t>
```

---

## Utilities

```cpp
// Absolute value (requires signed significand_t)
d64 abs_val = math::scaled::abs(x);

// Conversion to floating-point
double d  = static_cast<double>(x);
float  f  = static_cast<float>(x);
auto   ld = static_cast<long double>(x);

// String representation
std::string s = static_cast<std::string>(x);   // "12345e-2"

// Checksum (for hashing / verification)
uint64_t h = math::scaled::checksum(x);
```

---

## Relationship to other types

| | `math::decimal_t` | `math::bcd::decimal_t` | `math::fixed::decimal_t` | `math::scaled::decimal_t` |
|---|:---:|:---:|:---:|:---:|
| NaN / Inf | yes | yes | yes | **no** |
| Category field | yes | yes | yes | **no** |
| Exponent | runtime | runtime | **compile-time** | runtime |
| Sign encoding | category | category | category | **significand** |
| Ordering | partial | partial | partial | **strong** |
| External dep | LIBBID | none | none | none |

---

## Minimal example

```cpp
#include <decimal/scaled.hpp>

using price_t = math::scaled::decimal_t<int64_t>;

price_t bid{10250, -2};   // 102.50
price_t ask{10275, -2};   // 102.75
price_t spread = ask - bid;

auto [sig, exp] = spread.as_pair();
// sig = 25, exp = -2  →  0.25
```
