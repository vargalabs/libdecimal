# Intel BID Scientific Notation Parsing — Failure Summary

Direct calls to Intel’s BID parsing functions:

- `__bid32_from_string`
- `__bid64_from_string`

show incorrect handling of **scientific notation (`e` / `E`)**.

The issue is systematic and affects both positive and negative exponents, as well as fractional mantissas.

## Observed Behavior

### Positive Exponents

| Input  | Expected | Actual |
|--------|----------|--------|
| `1e1`  | `10`     | `1`    |
| `1e2`  | `100`    | `10`   |
| `1e3`  | `1000`   | `100`  |
| `1e4`  | `10000`  | `1000` |

Pattern:$1eN \rightarrow 10^{N-1}$ → **Exponent is off by -1**

### Fractional Scientific Notation

| Input    | Expected | Actual |
|----------|----------|--------|
| `1.2e2`  | `120`    | `12`   |
| `-1.2e2` | `-120`   | `-12`  |

Pattern: $( a.b ) \cdot 10^N \rightarrow (a.b) \cdot 10^{N-1}$

### Signed Values

| Input    | Expected | Actual |
|----------|----------|--------|
| `-1e1`   | `-10`    | `-1`   |
| `-1.2e2` | `-120`   | `-12`  |

→ Same off-by-one exponent error applies

### Negative Exponents (Appear Correct)

| Input   | Expected | Actual |
|---------|----------|--------|
| `1e-1`  | `0.1`    | `0.1`  |
| `1e-2`  | `0.01`   | `0.01` |
| `1e-3`  | `0.001`  | `0.001` |

→ Negative exponent path behaves correctly

### Malformed Input Handling

| Input   | Expected Behavior | Actual Behavior |
|---------|------------------|-----------------|
| `""`    | error / flag     | `nan`, flags = 0 |
| `"-"`   | error / flag     | `nan`, flags = 0 |
| `"abc"` | error / flag     | `nan`, flags = 0 |
| `"1..2"`| error / flag     | `nan`, flags = 0 |

→ Parser returns `nan`  
→ **No error flags are set**

## Root Cause

The Intel parser appears to apply incorrect exponent normalization for nonnegative exponents: $\text{internal exponent} = \text{parsed exponent} - 1$ instead of:$\text{internal exponent} = \text{parsed exponent}$ This results in a systematic **decade shift error**.

## Impact

- Scientific notation parsing is **incorrect and unreliable**
- Error flags (`flags`) are **not usable for validation**
- Behavior is consistent but **mathematically wrong**


## Recommended Strategy: Do NOT use scientific notation in financial systems