# Decimal Type Selection Guide

libdecimal provides four decimal representations. This guide helps you pick the right one.

---

## Decision Matrix

| Need | Use |
|---|---|
| Full IEEE 754-2008 compliance, transcendentals, audit trail | `math::decimal_t<T>` (BID) |
| Compile-time fixed scale, constexpr, zero overhead | `math::fixed::decimal_t<T, exponent>` |
| Exact arithmetic, pure C++, no Intel LIBBID dependency | `math::bcd::decimal_t<T>` |
| Lightweight (significand, exponent) pair, serialization | `math::scaled::decimal_t<T>` |

---

## `math::decimal_t<T>` — BID (Binary Integer Decimal)

**When to use:** You need full IEEE 754-2008 decimal semantics — transcendentals, NaN/Inf propagation, exchange-compatible arithmetic, or auditability.

```cpp
#include <decimal/bid.hpp>
using namespace math::literals;

// Construction
math::decimal_t<uint64_t> price(12345, -2);   // 123.45
auto fee = "0.0025"_dec;                       // string literal
auto total = price * fee;                      // exact

// Transcendentals
auto result = math::exp(price);

// Formatting
std::println("{}", price);     // "123.45"

// Precisions: decimal_t<uint32_t> = 7 digits, <uint64_t> = 16, <BID_UINT128> = 34
```

**Strengths:** Intel LIBBID-backed, full operator set, `std::formatter`, UDLs (`_dec32`, `_dec64`, `_dec128`, `_dec`), math constants at all three precisions, DPD↔BID conversion.

**Trade-offs:** Requires linking Intel LIBBID (`libbid_static`).

---

## `math::fixed::decimal_t<T, exponent>` — Fixed-Point

**When to use:** The decimal scale is known at compile time (e.g., always 4 decimal places for currency). Constexpr context, embedded, or you need zero-overhead arithmetic.

```cpp
#include <decimal/fixed.hpp>
using price_t = math::fixed::decimal_t<uint64_t, -4>;  // 4 decimal places

price_t p{math::bid::category::positive, 1234567};  // 123.4567
price_t q{math::bid::category::positive, 9999};     // 0.9999
price_t sum = p + q;                                // 124.4566, overflow-checked

// Decompose
auto [kind, significand, exp] = p.decompose();
// kind = positive, significand = 1234567, exp = -4 (compile-time constant)

// Constexpr
static_assert(price_t::exponent_v == -4);
static_assert(price_t::scale_factor_v == 10000);
```

**Strengths:** No external dependency, `constexpr`, widened-intermediate overflow detection (`uint128_t` for 64-bit), `checksum()`.

**Trade-offs:** Exponent is baked into the type — you cannot mix scales without a cast.

---

## `math::bcd::decimal_t<T>` — Binary Coded Decimal

**When to use:** You need exact decimal arithmetic without Intel LIBBID, or you need digit-level access (nibble-packed storage), or you are targeting an environment where LIBBID is unavailable.

```cpp
#include <decimal/bcd.hpp>
using bcd64 = math::bcd::decimal_t<uint64_t>;

bcd64 a{123456789ull};          // positive, exponent = 0
bcd64 b{"987654321", -3};       // 987654.321, from string
bcd64 sum = a + b;              // exact, overflow-checked

// Digit-level: packed as nibbles in significand_t
auto [kind, significand, exp] = a.decompose();

// String representation
std::string s = a.str();        // "123456789e0"

// Supports: +, -, *, /, ==, <=>, str(), to_long_double(), checksum()
```

**Strengths:** Pure C++ (no LIBBID), runtime exponent, `constexpr`-friendly operations, BCD digit access via `packed_bcd()`.

**Trade-offs:** Slower than BID for bulk arithmetic (nibble-pack/unpack per operation); no transcendentals.

---

## `math::scaled::decimal_t<T>` — Scaled Integer

**When to use:** You need a lightweight (significand, exponent) pair — wire format, serialization, hash keys, or intermediate representations where NaN/Inf semantics are not required.

```cpp
#include <decimal/scaled.hpp>
using scaled64 = math::scaled::decimal_t<int64_t>;

scaled64 x{12345, -2};    // 123.45
scaled64 y{50, -2};       // 0.50
scaled64 sum = x + y;     // 123.95 (exponent-aligned)

// Signed significand encodes sign directly
scaled64 neg{-12345, -2}; // -123.45
scaled64 abs_val = math::scaled::abs(neg);

// Decompose
auto [sig, exp] = x.as_pair();

// Comparison (strong_ordering — no NaN)
bool less = (x < y);

// Checksum for hashing/verification
uint64_t h = math::scaled::checksum(x);
```

**Strengths:** Minimal footprint, no NaN/Inf overhead, `strong_ordering` (not `partial_ordering`), signed or unsigned significand, `checksum()`.

**Trade-offs:** No NaN/Inf — not IEEE 754. Division truncates (integer quotient). No string parsing constructor.

---

## Shared Infrastructure

All four types share `math::bid::category`:

```cpp
enum struct category : unsigned char {
    positive = 0b0000, negative = 0b0001,
    pinf     = 0b0010, ninf     = 0b0011,
    nan      = 0b0100, zero     = 0b1000
};
```

BID, BCD, and fixed types all use this enum for their `kind` field. Code that inspects `math::bid::category` works uniformly across the family.

---

## Summary

```
Need NaN/Inf + transcendentals?  ──→  math::decimal_t  (BID)
Fixed scale, constexpr?           ──→  math::fixed::decimal_t
Pure C++, digit access?           ──→  math::bcd::decimal_t
Lightweight pair, serialization?  ──→  math::scaled::decimal_t
```
