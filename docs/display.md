---
hide:
  - toc
---

The library provides consistent, human-readable formatting across  :material-decimal-decrease: `decimal32` · :material-decimal: `decimal64` · :material-decimal-increase: `decimal128` Display is available via: **`std::string`** conversion  , **`operator<<`** (ostream)   **`std::format`** and low-level BID helpers (**`display_bidXX`**, **`print`**)

=== ":material-play-circle:{.icon}  Basic usage"

    ```cpp
    math::decimal_t<uint32_t> a{"123.45"};

    std::string s = static_cast<std::string>(a); // "123.45"
    std::cout << a;                              // "123.45"
    std::format("value={}", a);                  // "value=123.45"
    ```
=== ":material-memory:{.icon}  BID-level display"

    Low-level formatting operates directly on raw BID values:

    ```cpp
    math::bid::display_bid32(uint32_t)
    math::bid::display_bid64(uint64_t)
    math::bid::display_bid128(BID_UINT128)
    ```

    Example:

    ```cpp
    auto raw = math::bid::uint32_to_bid32(12345, -2);
    math::bid::display_bid32(raw);  // "123.45"
    ```
=== ":material-function-variant:{.icon}  Finite values"

    Numbers are rendered in normalized decimal form:

    | Internal            | Output     |
    |---------------------|------------|
    | `(12345, -2)`       | `"123.45"` |
    | `(5, -3)`           | `"0.005"`  |
    | `(42, 3)`           | `"4200"`   |

    - Negative values include `-`
    - No scientific notation for `decimal32` / `decimal64`
    - `decimal128` is normalized to fixed notation

=== ":material-plus-minus-variant:{.icon}  Zero handling"

    Zero is **canonically normalized**:

    ```cpp
    0, -0, 0E±k → "0"
    ```
    - Sign and exponent are ignored
    - All representations collapse to `"0"`

=== ":material-infinity:{.icon}  Special values"

    | Value | Output   |
    |------|----------|
    | NaN  | `"nan"`  |
    | +∞   | `"+inf"` |
    | -∞   | `"-inf"` |

    Example:

    ```cpp
    math::bid::display_bid32(0x78000000u);  // "+inf"
    math::bid::display_bid32(0xf8000000u);  // "-inf"
    math::bid::display_bid32(0x7c000000u);  // "nan"
    ```
=== ":material-decimal-increase:{.icon}  decimal128 normalization"

    Internally, `decimal128` may use scientific form:

    ```cpp
    +125E-2
    ```

    Display is normalized:

    ```cpp
    "1.25" → "1.25"

    "3.141592653589793238462643383279502"
    → "3.141592653589793238462643383279502"
    ```

    - No `E` notation in output
    - Precision: **34 significant digits**
    - Inputs exceeding precision are rounded

    ```cpp
    "3.1415926535897932384626433832795029"
    → "3.141592653589793238462643383279503"
    ```

<div class="clear-both"></div>

=== ":material-swap-horizontal:{.icon} Float conversions"

    ```cpp
    decimal32{0.1} → "0.1000000"
    decimal64{0.1} → "0.1000000000000000"
    ```
    This reflects: $\text{binary float} \neq \text{exact decimal value}$

    - Full decimal expansion is preserved
    - No “pretty-printing” heuristics

=== ":material-cog:{.icon}  Low-level print()"

    Internal helper used by all display functions:

    ```cpp
    math::bid::print(sign, mantissa, exponent, buffer, size)
    ```

    Behavior:

    - Places decimal point according to exponent  
    - Pads with zeros  
    - Preserves trailing zeros  

    ```cpp
    print(false, 123, -2) → "1.23"
    print(false, 5, -3)   → "0.005"
    print(false, 42, 3)   → "4200"
    ```

=== ":material-alert:{.icon}  Known differences"

    | Area               | Note                                          |
    |--------------------|-----------------------------------------------|
    | Float inputs       | May expand (e.g. `"0.1000000"`)               |
    | Zero normalization | Always rendered as `"0"`                      |
    | Invalid input      | Always mapped to NaN                          |
    | Scientific Notation| Exponent is off by 1, see: `test/thirdparty.cpp` |
    


<div class="clear-both"></div>