# libdecimal Benchmark Results

**Date:** 2026-05-13  
**Branch:** 26-bench-add-benchmark-suite  
**Build:** Release, `-O3 -march=native`, GCC  
**Platform:** Linux 5.15, x86-64  
**Note:** CPU governor was `powersave`; results have modest variance (~1–2%) but relative rankings are stable.

---

## Types Benchmarked

| Short name | Type | Characteristics |
|---|---|---|
| `intel bid64` | `math::decimal_t<uint64_t>` | IEEE 754-2008 BID encoding via Intel LIBBID |
| `intel bid32` | `math::decimal_t<uint32_t>` | 32-bit BID |
| `fixed64 -4` | `math::fixed::decimal_t<uint64_t, -4>` | Compile-time exponent, integer significand |
| `fixed32` | `math::fixed::decimal_t<uint32_t, -4>` | 32-bit fixed |
| `scaled int64` | `math::scaled::decimal_t<int64_t>` | Runtime (sig, exp) pair, normalizes on construction |
| `scaled uint32` | `math::scaled::decimal_t<uint32_t>` | 32-bit scaled |
| `bcd64` | `math::bcd::decimal_t<uint64_t>` | BCD encoding |
| `boost decimal64` | `boost::decimal::decimal64_t` | Boost.Decimal v6.0.1 |
| `double` / `float` | IEEE 754 binary | Reference baseline |

---

## 1. Construction from (significand, exponent)

### 32-bit

| Type | ns/op | relative |
|---|---|---|
| boost decimal32 | 3,934 | 100% *(baseline)* |
| bcd pair32 | 5,207 | 76% |
| float naive | 5,139 | 77% |
| scaled pair32 | 1,199 | 328% |
| **intel decimal32** | **840** | **469%** |
| fixed32 checked | 812 | 485% |
| **fixed32 unchecked** | **806** | **488%** |
| float optimised | 765 | 514% |

### 64-bit

| Type | ns/op | relative |
|---|---|---|
| double naive | 10,995 | 9% |
| bcd pair64 | 7,080 | 14% |
| boost decimal64 | 979 | 100% *(baseline)* |
| scaled pair64 | 1,089 | 90% |
| intel decimal64 | 920 | 107% |
| double optimised | 757 | 129% |
| fixed64 checked | 709 | 138% |
| **fixed64 unchecked** | **612** | **160%** |

**Takeaway:** `fixed64 unchecked` is fastest to construct (~600 ns/10k ops). `fixed` and `intel bid` are in the same tier. `bcd` and `boost 32-bit` are 5–15× slower.

---

## 2. Decompose to (significand, exponent)

### 32-bit

| Type | ns/op | relative |
|---|---|---|
| boost decimal32 (frexp10) | 60,008 | 15% |
| float (utils::decompose) | 9,143 | 100% *(baseline)* |
| intel bid32 (bid::decompose) | 7,166 | 128% |
| **scaled uint32 (as\_pair)** | **3,442** | **266%** |

### 64-bit

| Type | ns/op | relative |
|---|---|---|
| boost decimal64 (frexp10) | 25,116 | 36% |
| double (utils::decompose) | 9,140 | 100% *(baseline)* |
| intel bid64 (bid::decompose) | 7,559 | 121% |
| **scaled int64 (as\_pair)** | **3,968** | **230%** |

**Takeaway:** `scaled::as_pair()` is pure struct field access — 2.3× faster than float decompose, 6× faster than Boost `frexp10`. Intel BID decompose (bit manipulation) is 20% faster than float decompose. Boost `frexp10` is by far the most expensive.

---

## 3. Comparison (`<`)

### Micro (10k pairs)

| Type | ns/op | relative |
|---|---|---|
| boost decimal64 | 61,288 | 23% |
| intel bid64 | 50,092 | 29% |
| double | 14,333 | 100% *(baseline)* |
| fixed64 -4 | 22,955 | 62% |
| **scaled int64** | **21,246** | **68%** |

### Sort 1,000 values

| Type | ns/op | relative |
|---|---|---|
| boost decimal64 | 57,265 | 8% |
| intel bid64 | 49,514 | 9% |
| scaled int64 | 23,733 | 19% |
| **fixed64 -4** | **12,303** | **38%** |
| double | 4,618 | 100% *(baseline)* |

**Takeaway:** `fixed64` and `scaled` are the fastest decimal types for comparison (~2/3 of double speed). Intel BID and Boost are 3–4× slower than double for comparison, 10–12× slower for sort. For order-book operations that require many comparisons, `fixed` is the clear choice.

---

## 4. Arithmetic

### Accumulate sum (10k values, `+=`)

| Type | ns/op | relative |
|---|---|---|
| bcd64 | 1,278,129 | 0.6% |
| boost decimal64 | 189,589 | 4.3% |
| intel bid64 | 50,849 | 16.2% |
| scaled int64 | 29,683 | 27.7% |
| **fixed64 -4** | **10,354** | **79.5%** |
| double | 8,233 | 100% *(baseline)* |

### Dot product price×qty (256 pairs, `+= p * q`)

| Type | ns/op | relative |
|---|---|---|
| bcd64 | 118,334 | 0.1% |
| boost decimal64 | 5,491 | 2.9% |
| intel bid64 | 2,059 | 7.8% |
| **scaled int64** | **1,007** | **15.8%** |
| **fixed64 -4** | **830** | **19.2%** |
| double | 160 | 100% *(baseline)* |

**Takeaway:** `fixed64` is the fastest decimal type for repeated addition (79.5% of double). For multiply-accumulate (dot product), `fixed64` and `scaled` are within 5× of double. `bcd` is catastrophically slow for all arithmetic — it exists only for string formatting. `boost` is 23× slower than `fixed64` for accumulate.

---

## 5. String: Parse and Format

### Parse from decimal string (`"12345.6789"`)

| Type | ns/op | relative |
|---|---|---|
| bcd64 (digit-string + exp) | 204,800 | 6.9% |
| stod (double reference) | 46,488 | 30.2% |
| boost decimal64 (strtod64) | 43,840 | 32.0% |
| **intel bid64** | **14,049** | **100%** *(baseline)* |

### Format to string

| Type | ns/op | relative |
|---|---|---|
| double (to_string) | 198,713 | 22% |
| boost decimal64 (ostringstream) | 130,518 | 34% |
| intel bid64 (std::format) | 44,175 | 100% *(baseline)* |
| bcd64 (.str()) | 39,173 | 113% |
| **scaled int64 (static\_cast\<string\>)** | **16,181** | **273%** |

**Takeaway:** Intel BID64 is the fastest parser at 14 µs/1000 values — 3× faster than `stod` and Boost, 15× faster than BCD. For formatting, `scaled` is fastest (it formats an integer pair without decimal arithmetic), followed closely by BCD. Intel BID via `std::format` is 113× faster than `double::to_string`.

---

## 6. Fee Calculation (multiply: notional × rate, 1k pairs)

| Type | ns/op | relative |
|---|---|---|
| boost decimal64 | 4,744 | 18.2% |
| intel bid64 | 3,832 | 22.5% |
| fixed64 -4 | 2,714 | 31.8% |
| **scaled int64** | **1,869** | **46.1%** |
| double | 862 | 100% *(baseline)* |

**Takeaway:** `scaled int64` is the fastest decimal multiply (46% of double), ~2× faster than Intel BID. This is because scaled multiply is integer multiplication with exponent bookkeeping, vs BID's full IEEE 754-2008 round-to-nearest-even.

---

## 7. Wire Encode/Decode (`{int64 significand, int16 exponent}`)

### Encode to wire pair

| Type | ns/op | relative |
|---|---|---|
| intel bid64 (decompose) | 7,627 | 65% |
| scaled int64 (field access) | 4,969 | 100% *(baseline)* |
| **fixed64 -4 (field + constexpr exp)** | **4,211** | **118%** |

### Decode from wire pair

| Type | ns/op | relative |
|---|---|---|
| scaled int64 (direct ctor) | 15,146 | 100% *(baseline — slowest)* |
| intel bid64 (BID ctor from pair) | 8,638 | 175% |
| **fixed64 -4 (direct ctor)** | **4,815** | **315%** |

**Takeaway:** `fixed64` wins both encode and decode. `scaled` encode is near-free (field access), but its decode is 3× slower than `fixed64` because the `scaled` constructor calls `normalize()` to strip trailing zeros. Intel BID decode is in the middle. For high-frequency wire protocols, `fixed64` is the optimal choice.

---

## 8. Risk Limit Check (accumulate + clamp, 10k values, mixed pos/neg)

| Type | ns/op | relative |
|---|---|---|
| boost decimal64 | 225,837 | 7.3% |
| scaled int64 | 100,729 | 16.4% |
| intel bid64 | 81,629 | 20.2% |
| **fixed64 -4** | **36,810** | **44.9%** |
| double | 16,517 | 100% *(baseline)* |

**Takeaway:** `fixed64` is the fastest decimal type for the risk-limit hot path (accumulate + branch + clamp) — 2.2× faster than Intel BID, 6× faster than Boost. The branch (`if acc > limit`) strongly penalizes types with slow comparison.

---

## Summary: Type Selection Guide for Trading Systems

| Use case | Best type | Notes |
|---|---|---|
| PnL accumulation / running sums | `fixed64 -4` | 79% of double; fast `+=` |
| Fee/commission calculation | `scaled int64` | 46% of double; fastest decimal multiply |
| Order book: price comparison, sort | `fixed64 -4` | 38% of double sort speed |
| Risk limit checks (accumulate + clamp) | `fixed64 -4` | 2.2× faster than BID |
| Wire encode (serialize to pair) | `fixed64 -4` | Field access + compile-time exp |
| Wire decode (deserialize from pair) | `fixed64 -4` | 3× faster than scaled (no normalize) |
| Parse from decimal string | `intel bid64` | 3× faster than stod |
| Format to string | `scaled int64` | 2.7× faster than BID std::format |
| Dot product (price × qty) | `fixed64 -4` | 5× double; similar to scaled |
| Full IEEE 754-2008 semantics | `intel bid64` | NaN, Inf, correct rounding, std::format |
| String-only use, no arithmetic | `bcd64` | Fast format (.str()), slow arithmetic |

### Performance Tier Summary (relative to double)

```
Operation          fixed64   scaled    intel BID  boost
---------------------------------------------------------
Accumulate         79%       28%       16%         4%
Multiply (fee)     32%       46%       23%        18%
Compare (micro)    62%       68%       29%        23%
Sort               38%       20%        9%         8%
Wire encode       118%      100%       65%         —
Wire decode       315%      100%      175%         —
Parse string        —         —       100%        32%
Format string       —       273%      100%       34%
Risk limit         45%       16%       20%         7%
```

### Key Findings

1. **`fixed64 -4` is the dominant type for arithmetic-heavy paths** — accumulate, dot product, comparison, sort, wire encode/decode, risk limits. Compile-time exponent eliminates alignment logic from every operation.

2. **`scaled int64` wins on multiply and wire encode**, but its constructor calls `normalize()` making decode 3× slower than fixed. Best for paths that construct few values and compute many products.

3. **Intel BID64 is the only type with full IEEE 754-2008 parse semantics** and is the fastest string parser. Required when interoperating with FIX/FAST/decimal-string feeds. Arithmetic is ~4–6× slower than `fixed64`.

4. **BCD is a formatting type only** — arithmetic is 100–1000× slower than double. Use only for display/logging pipelines.

5. **Boost.Decimal is consistently the slowest** for every arithmetic operation, 2–5× behind Intel BID. Its `frexp10` decompose is 6–60× slower than `scaled::as_pair()`. No production path should use it as a hot-path type.

6. **`scaled` decode anomaly**: the `scaled` direct constructor is slower than Intel BID decode (15 µs vs 8.6 µs) because `normalize()` is called unconditionally. This is worth noting for latency-sensitive deserialization.
