# `scaled.hpp`

## Overview

`scaled.hpp` implements a minimal, allocation-free numeric type based on a canonical triplet:

[
x = (\texttt{category}, m, e), \quad m \in \mathbb{N},\ e \in \mathbb{Z}
]

* **category** ∈ { `positive`, `negative`, `zero`, `pinf`, `ninf`, `nan` }
* **significand** `m` stored as unsigned integer
* **exponent** `e` stored as signed integer

Sign is carried exclusively by `category`.

---

## Representation

```cpp
struct scaled_t {
    category kind;
    significand_t m;
    exponent_t e;
};
```

### Invariants

* `kind == zero`  ⇒ `m = 0`, `e = 0`
* `kind ∈ {positive, negative}` ⇒ `m > 0`
* `kind ∈ {pinf, ninf, nan}` ⇒ `m = 0`, `e = 0` (ignored)
* no sign encoded in `m`

---

## Semantics

### Finite values

[
x =
\begin{cases}
+m \cdot B^e & \text{if } kind = positive \
-m \cdot B^e & \text{if } kind = negative
\end{cases}
]

`B` is the implicit radix (typically 10 or 2 depending on context).

---

### Special values

| category | meaning      |
| -------- | ------------ |
| `zero`   | exact 0      |
| `pinf`   | +∞           |
| `ninf`   | −∞           |
| `nan`    | not-a-number |

---

## Decomposition

```cpp
auto [kind, m, e] = decompose(x);
```

* exact for all finite values
* canonical:

  * zero → `{zero, 0, 0}`
  * no signed zero
* matches `decimal.hpp` and IEEE-754 decomposition (`utils.hpp`)

---

## Construction

```cpp
scaled_t x{category::positive, m, e};
```

or via helpers:

```cpp
make(m, e)          // finite
make_zero()
make_inf(sign)
make_nan()
```

---

## Normalization

Normalization enforces:

* `m = 0` ⇒ `zero`
* no redundant representations
* optional trimming of trailing base factors:
  [
  m \cdot B^e \rightarrow (m', e') \quad \text{with } \gcd(m', B)=1
  ]

---

## Arithmetic (finite domain)

Operations defined on `{positive, negative, zero}`:

* addition / subtraction: exponent alignment
* multiplication:
  [
  (m_1,e_1)\cdot(m_2,e_2) = (m_1 m_2,\ e_1 + e_2)
  ]
* division (if supported): inverse scaling

Special values may:

* propagate (`nan`)
* saturate (`±inf`)
* or be rejected (implementation-defined)

---

## Comparison

```cpp
std::partial_ordering
```

* `nan` is unordered
* otherwise lexicographic after exponent alignment

---

## Properties

* **exact arithmetic** (no rounding unless explicitly introduced)
* **deterministic layout** (POD / trivially serializable)
* **no hidden state** (no implicit normalization assumptions)
* **branch-light decomposition**
* suitable for:

  * financial math
  * serialization / hashing
  * deterministic replay
  * ZK-friendly representations

---

## Relationship to other components

* `decimal.hpp` → hardware/BID-backed decimal FP
* `utils.hpp` → IEEE-754 float decomposition
* `scaled.hpp` → integer-backed canonical representation

All share:

[
x \leftrightarrow (\texttt{category}, m, e)
]

---

## Notes

* collapses `+0` and `-0` → `zero`
* not a full IEEE-754 model (by design)
* exponent range depends on `exponent_t`
* overflow behavior depends on `significand_t`

---

## Minimal example

```cpp
scaled_t x{category::positive, 12345, -2}; // 123.45

auto [c, m, e] = decompose(x);
// c = positive, m = 12345, e = -2
```
