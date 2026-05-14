# Performance

!!! note "Environment"
    **CPU:** 11th Gen Intel Core i7-11700K @ 3.60 GHz · **OS:** Linux 5.15 x86_64  
    **Build:** `-O3 -march=native`, C++23  
    **Baseline:** `boost::decimal` = 100 %; numbers **above 100 %** are **faster** than boost.  
    CPU frequency scaling was active (powersave governor + turbo); figures are indicative, not lab-grade.

---

## Construct from (significand, exponent)

=== "32-bit"

    | relative | ns/op | type |
    |---------:|------:|:-----|
    | **100.0 %** | 3 934 | `boost decimal32` — baseline |
    | 76.5 % | 5 142 | `float` naive |
    | **509 %** | 772 | `float` optimised |
    | **454 %** | 866 | `intel bid32` |
    | **326 %** | 1 208 | `scaled uint32` |
    | 75.2 % | 5 228 | `bcd uint32` |
    | **480 %** | 820 | `fixed32` checked |
    | **484 %** | 814 | `fixed32` unchecked |

=== "64-bit"

    | relative | ns/op | type |
    |---------:|------:|:-----|
    | **100.0 %** | 981 | `boost decimal64` — baseline |
    | 8.9 % | 11 047 | `double` naive |
    | **130 %** | 757 | `double` optimised |
    | **106 %** | 926 | `intel bid64` |
    | 89.4 % | 1 096 | `scaled uint64` |
    | 13.7 % | 7 132 | `bcd uint64` |
    | **137 %** | 718 | `fixed64` checked |
    | **158 %** | 621 | `fixed64` unchecked |

Fixed-exponent types dominate at both widths. BCD construction is 7–12× slower than boost.

---

## Decompose to (significand, exponent)

=== "32-bit"

    | relative | ns/op | type |
    |---------:|------:|:-----|
    | **100.0 %** | 61 368 | `boost decimal32 frexp10` — baseline |
    | **671 %** | 9 152 | `float utils::decompose` |
    | **855 %** | 7 181 | `intel bid32 bid::decompose` |
    | **1781 %** | 3 446 | `scaled uint32 as_pair` |

=== "64-bit"

    | relative | ns/op | type |
    |---------:|------:|:-----|
    | **100.0 %** | 24 250 | `boost decimal64 frexp10` — baseline |
    | **265 %** | 9 147 | `double utils::decompose` |
    | **361 %** | 6 727 | `intel bid64 bid::decompose` |
    | **610 %** | 3 974 | `scaled int64 as_pair` |

`boost::decimal::frexp10` is the most expensive decompose path. `scaled::as_pair()` is a direct field read — **18× faster** at 32-bit, **6× faster** at 64-bit. Use libdecimal types for any path that must inspect the representation (serialization, wire encoding, logging).

---

## Compare

=== "Micro (10 000 adjacent pairs)"

    | relative | ns/op | type |
    |---------:|------:|:-----|
    | **100.0 %** | 60 017 | `boost decimal64` — baseline |
    | **385 %** | 15 611 | `double` |
    | **119 %** | 50 563 | `intel bid64` |
    | **276 %** | 21 771 | `fixed64 -4` |
    | **294 %** | 20 431 | `scaled int64` |

=== "Sort (1 000 values)"

    | relative | ns/op | type |
    |---------:|------:|:-----|
    | **100.0 %** | 56 215 | `boost decimal64` — baseline |
    | **1240 %** | 4 535 | `double` |
    | **111 %** | 50 457 | `intel bid64` |
    | **454 %** | 12 370 | `fixed64 -4` |
    | **237 %** | 23 721 | `scaled int64` |

Boost comparison is the slowest decimal option. `fixed64 -4` sorts 4.5× faster; boost comparison is nearly identical to intel BID64.

---

## Arithmetic

=== "Accumulate (10 000 values)"

    | relative | ns/op | type |
    |---------:|------:|:-----|
    | **100.0 %** | 190 245 | `boost decimal64` — baseline |
    | **2378 %** | 8 002 | `double` |
    | **373 %** | 51 030 | `intel bid64` |
    | 14.9 % | 1 278 906 | `bcd64` |
    | **1886 %** | 10 090 | `fixed64 -4` |
    | **632 %** | 30 082 | `scaled int64` |

=== "Dot product (256 pairs)"

    | relative | ns/op | type |
    |---------:|------:|:-----|
    | **100.0 %** | 5 457 | `boost decimal64` — baseline |
    | **3439 %** | 159 | `double` |
    | **264 %** | 2 070 | `intel bid64` |
    | 4.6 % | 118 188 | `bcd64` |
    | **654 %** | 834 | `fixed64 -4` |
    | **530 %** | 1 030 | `scaled int64` |

Boost arithmetic is the slowest decimal path. `fixed64` is 19× faster for accumulation. BCD should not be used for arithmetic — it is 7–20× **slower** than boost.

---

## String parse and format

=== "Parse (1 000 values)"

    | relative | ns/op | type |
    |---------:|------:|:-----|
    | **100.0 %** | 44 106 | `boost decimal64` — baseline |
    | **315 %** | 14 008 | `intel bid64` |
    | 21.5 % | 205 445 | `bcd64` (digit-string + exp) |
    | 94.7 % | 46 559 | `stod` (double reference) |

=== "Format (1 000 values)"

    | relative | ns/op | type |
    |---------:|------:|:-----|
    | **100.0 %** | 131 586 | `boost decimal64` — baseline |
    | **293 %** | 44 877 | `intel bid64` (`std::format`) |
    | **238 %** | 55 215 | `bcd64` (`.str()`) |
    | **827 %** | 15 919 | `scaled int64` (cast to string) |
    | 66.3 % | 198 355 | `double` (`to_string`) |

Intel BID64 parses 3× faster than boost. For formatting, `scaled`'s cast-to-string is 8× faster than boost's `ostringstream <<`.

---

## Fee calculation — `notional × rate`

| relative | ns/op | type |
|---------:|------:|:-----|
| **100.0 %** | 4 607 | `boost decimal64` — baseline |
| **569 %** | 810 | `double` |
| **124 %** | 3 730 | `intel bid64` |
| **176 %** | 2 617 | `fixed64 -4` |
| **273 %** | 1 688 | `scaled int64` |

Multiply is boost's least-bad category; intel is only 1.24× faster. Scaled is 2.7× faster.

---

## Wire encode / decode

!!! note
    No boost type in this group — baseline is `scaled int64` (first runner).

=== "Encode → wire pair"

    | relative | ns/op | type |
    |---------:|------:|:-----|
    | **100.0 %** | 4 667 | `scaled int64` (field access) — baseline |
    | **116 %** | 4 024 | `fixed64 -4` (field + constexpr exp) |
    | 64.3 % | 7 264 | `intel bid64` (decompose) |

=== "Decode ← wire pair"

    | relative | ns/op | type |
    |---------:|------:|:-----|
    | **100.0 %** | 14 047 | `scaled int64` (direct ctor) — baseline |
    | **310 %** | 4 531 | `fixed64 -4` (direct ctor) |
    | **172 %** | 8 168 | `intel bid64` (encode from pair) |

`scaled` encode is a field read — near-free. But its constructor calls `normalize()` unconditionally, making decode 3× slower than `fixed64`. Prefer `fixed64` for round-trip wire paths.

---

## Risk limit — accumulate + clamp (10 000 values)

| relative | ns/op | type |
|---------:|------:|:-----|
| **100.0 %** | 219 475 | `boost decimal64` — baseline |
| **1370 %** | 16 017 | `double` |
| **279 %** | 78 644 | `intel bid64` |
| **612 %** | 35 873 | `fixed64 -4` |
| **227 %** | 96 909 | `scaled int64` |

Under realistic mixed-op pressure (add + branch + assign per step), boost is still the slowest decimal type.

---

## Summary

| type | construct | decompose | compare | accumulate | multiply | parse | format | risk |
|:-----|:---------:|:---------:|:-------:|:----------:|:--------:|:-----:|:------:|:----:|
| `double` | 1.3× | 2.7× | 3.9× | 23.8× | 5.7× | 0.95× | 0.66× | 13.7× |
| `intel bid64` | 1.1× | 3.6× | 1.2× | 3.7× | 1.2× | **3.2×** | 2.9× | 2.8× |
| `fixed64 -4` | **1.6×** | — | **2.8×** | **18.9×** | 1.8× | — | — | **6.1×** |
| `scaled int64` | 0.9× | **6.1×** | 2.9× | 6.3× | **2.7×** | — | **8.3×** | 2.3× |
| `bcd64` | 0.1× | — | — | 0.15× | 0.05× | 0.2× | 2.4× | — |

Numbers are speedup relative to boost decimal (>1× = faster than boost).

### Type selection

| use case | recommended type | reason |
|:---------|:-----------------|:-------|
| Arithmetic — PnL, accumulation | `fixed64 -4` | 7–19× faster; compile-time exponent eliminates runtime normalization |
| Multiply hot path — fees, notional | `scaled int64` | 2.7× faster; no overflow risk from fixed exponent |
| Compare / sort / rank | `fixed64 -4` | 2.8–4.5× faster |
| Wire encode | `scaled int64` | field read, near-free |
| Wire decode | `fixed64 -4` | 3× faster; avoids `normalize()` in ctor |
| String parse | `intel bid64` | 3.2× faster than boost |
| String format | `scaled int64` | 8.3× faster via cast-to-string |
| Inspect internals / serialize | `scaled int64` | 6–18× faster `as_pair()` |
| General purpose / mixed | `intel bid64` | competitive across all ops, no exponent constraint |
| Avoid for arithmetic | `bcd64` | 7–20× slower than boost across all arithmetic ops |

---

## Repeating the experiment

### Prerequisites

```bash
# build directory must exist and be configured with benchmarks enabled
cmake -S . -B build \
      -DCMAKE_BUILD_TYPE=Release \
      -Dlibdecimal_BUILD_BENCHMARKS=ON
```

### Running the harness

```bash
# from the repo root
bash bench/run-all.sh
```

The script:

1. Rebuilds only the changed benchmark targets (`ninja -j$(nproc)`)
2. Runs all 8 benchmarks in order: `construct-from-pair`, `decompose`, `compare`, `arithmetic`, `string`, `ops-fee`, `encode-decode`, `mixed-ops`
3. Writes a timestamped log to `bench-results/bench-<YYYYMMDD-HHMMSS>.txt`

### Changing the baseline

nanobench uses the **first `bench.run()` call** in each `Bench` instance as the 100 % reference when `.relative(true)` is set. To rebaseline on a different type, move its `bench.run()` block to the top of the group in the corresponding `bench/*.cpp` file, then rerun the harness.

For `construct-from-pair.cpp`, which uses a `static_for<tuple_t>` loop, the first type in the tuple definition is the baseline:

```cpp
using import_64_t = std::tuple<
    traits_t<boost::decimal::decimal64_t>,   // <-- baseline (first = 100%)
    bench::ieee754<double, bench::kind_t::naive>,
    ...
>;
```

### Pinning the CPU for stable results

```bash
# disable frequency scaling (requires root)
sudo cpupower frequency-set -g performance
# or with pyperf
pip install pyperf
sudo python -m pyperf system tune
# then run
bash bench/run-all.sh
# restore after
sudo cpupower frequency-set -g powersave
```
