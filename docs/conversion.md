---
hide:
  - toc
---
<div class="float-left mr-6 mb-4 max-w-xl" markdown>
| :material-cog:{.icon} Capability                                | :material-decimal-decrease: **dec32** | :material-decimal: **dec64** | :material-decimal-increase: **dec128** |
|-----------------------------------------------------------------|:----------------------------:|:-------------------------------------:|:----------------------------------:|
| :material-format-text:{.icon} String import/export              | :material-check-bold:        |  :material-check-bold:                |  :material-check-bold:             |
| :material-swap-horizontal:{.icon} Float import/export           | :material-check-bold:        |  :material-check-bold:                |  :material-check-bold:             |
| :material-memory:{.icon} BID raw access                         | :material-check-bold:        |  :material-check-bold:                |  :material-check-bold:             |
| :material-database:{.icon} DPD import/export                    | :material-check-bold:        |  :material-check-bold:                |  :material-check-bold:             |
| :material-function-variant:{.icon} Decompose (sign, coeff, exp) | :material-check-bold:        |  :material-check-bold:                |  :material-check-bold:             |
| :material-variable:{.icon} `coefficient()` utility              | :material-check-bold:        |  :material-check-bold:                |  :material-check-bold:             |
</div>

**LIBDECIMAL** uses **BID as its canonical internal representation** for `decimal32`, `decimal64`, and `decimal128`. This is the core design choice: arithmetic, comparisons, formatting, and most constructors operate directly on the raw BID form.

**DPD**, in contrast, is treated as an interchange encoding. It is fully supported for import/export via explicit wrapper conversions, allowing values to be round-tripped when required. In practice, this means:

* BID is the **working format**
* DPD is a **wire-format companion**

A decimal value can be explicitly converted **to raw BID** or **to raw DPD**, and constructed back **from DPD** as needed. Internally, these transitions are handled by `impl::bid2dpd()` and `impl::dpd2bid()` specializations for 32-, 64-, and 128-bit payloads.

<div class="clear-both"></div>
#### Examples
=== ":material-decimal-decrease:{.icon} decimal32"

    ```cpp
    using dec32_t = math::decimal_t<std::uint32_t>;

    dec32_t a{"123.45"};                 // string → BID
    dec32_t b{12345u, -2};               // coefficient/exponent → BID
    dec32_t c = dec32_t::from(1.25);     // binary float → BID

    auto raw_bid = static_cast<math::bid_t<std::uint32_t>>(a).value;
    auto raw_dpd = static_cast<math::dpd_t<std::uint32_t>>(a).value;

    dec32_t d{ math::dpd_t<std::uint32_t>{raw_dpd} };   // DPD → BID → decimal
    auto s = static_cast<std::string>(d);               // BID → text
    auto x = static_cast<double>(d);                    // BID → binary float
    ```
=== ":material-decimal:{.icon} decimal64"

    ```cpp
    using dec64_t = math::decimal_t<std::uint64_t>;

    dec64_t a{"123.45"};                 // string → BID
    dec64_t b{12345ull, -2};             // coefficient/exponent → BID
    dec64_t c = dec64_t::from(1.25);     // binary float → BID

    auto raw_bid = static_cast<math::bid_t<std::uint64_t>>(a).value;
    auto raw_dpd = static_cast<math::dpd_t<std::uint64_t>>(a).value;

    dec64_t d{ math::dpd_t<std::uint64_t>{raw_dpd} };   // DPD → BID → decimal
    auto s = static_cast<std::string>(d);               // BID → text
    auto x = static_cast<double>(d);                    // BID → binary float
    ```
=== ":material-decimal-increase:{.icon} decimal128"

    ```cpp
    using dec128_t = math::decimal_t<BID_UINT128>;

    dec128_t a{"123.45"};                 // string → BID
    dec128_t b{BID_UINT128{12345u}, -2};  // coefficient/exponent → BID
    dec128_t c = dec128_t::from(1.25);    // binary float → BID

    auto raw_bid = static_cast<math::bid_t<BID_UINT128>>(a).value;
    auto raw_dpd = static_cast<math::dpd_t<BID_UINT128>>(a).value;

    dec128_t d{ math::dpd_t<BID_UINT128>{raw_dpd} };   // DPD → BID → decimal
    auto s = static_cast<std::string>(d);              // BID → text
    auto x = static_cast<double>(d);                   // BID → binary float
    ```
=== ":material-function-variant:{.icon} Finite values"

    Decomposition follows: $x = (-1)^s \cdot C \cdot 10^e$
    ```cpp
    -123.45 → (negative, 12345, -2)
     123.45 → (positive, 12345, -2)
    ```

=== ":material-plus-minus-variant:{.icon} Signed zero"

    Sign is preserved even when: $C = 0$
    ```cpp
    +0 → (positive, 0, e)
    -0 → (negative, 0, e)
    ```

=== ":material-infinity:{.icon} Non-finite values"

    Explicit classification of special values.
    ```cpp
    nan  → (nan,  0, 0)
    +inf → (pinf, 0, 0)
    -inf → (ninf, 0, 0)
    ```





=== ":material-decimal-decrease:{.icon} decimal32 calls"

    | Source                                           | Target                             |    Status | Notes                                          |
    | ------------------------------------------------ | ---------------------------------- | --------: | ---------------------------------------------- |
    | `(significand, exponent)`                        | decimal                            | supported | Encoded as BID using `uint32_to_bid32()`       |
    | `const char*`, `std::string`, `std::string_view` | decimal                            | supported | Parsed via Intel BID string conversion         |
    | `float`, `double`, `long double`                 | decimal                            | supported | Converted to BID                               |
    | decimal                                          | `std::string`                      | supported | Printed from BID                               |
    | decimal                                          | `float` / `double` / `long double` | supported | Converted from BID                             |
    | decimal                                          | `bid_t<std::uint32_t>`             | supported | Exposes raw internal BID bits                  |
    | decimal                                          | `dpd_t<std::uint32_t>`             | supported | Exports DPD from internal BID                  |
    | `dpd_t<std::uint32_t>`                           | decimal                            | supported | Imports DPD via BID                            |

=== ":material-decimal:{.icon} decimal64 calls"

    | Source                                           | Target                             |    Status | Notes                                          |
    | ------------------------------------------------ | ---------------------------------- | --------: | ---------------------------------------------- |
    | `(significand, exponent)`                        | decimal                            | supported | Encoded as BID using `uint64_to_bid64()`       |
    | `const char*`, `std::string`, `std::string_view` | decimal                            | supported | Parsed via Intel BID string conversion         |
    | `float`, `double`, `long double`                 | decimal                            | supported | Converted to BID                               |
    | decimal                                          | `std::string`                      | supported | Printed from BID                               |
    | decimal                                          | `float` / `double` / `long double` | supported | Converted from BID                             |
    | decimal                                          | `bid_t<std::uint64_t>`             | supported | Exposes raw internal BID bits                  |
    | decimal                                          | `dpd_t<std::uint64_t>`             | supported | Exports DPD from internal BID                  |
    | `dpd_t<std::uint64_t>`                           | decimal                            | supported | Imports DPD via BID                            |

=== ":material-decimal-increase:{.icon} decimal128 calls"

    | Source                                           | Target                             |    Status | Notes                                          |
    | ------------------------------------------------ | ---------------------------------- | --------: | ---------------------------------------------- |
    | `(significand, exponent)`                        | decimal                            | supported | Encoded as BID using `uint128_to_bid128()`     |
    | `const char*`, `std::string`, `std::string_view` | decimal                            | supported | Parsed via Intel BID string conversion         |
    | `float`, `double`, `long double`                 | decimal                            | supported | Converted to BID                               |
    | decimal                                          | `std::string`                      | supported | Printed from BID                               |
    | decimal                                          | `float` / `double` / `long double` | supported | Converted from BID                             |
    | decimal                                          | `bid_t<BID_UINT128>`               | supported | Exposes raw internal BID bits                  |
    | decimal                                          | `dpd_t<BID_UINT128>`               | supported | Exports DPD from internal BID                  |
    | `dpd_t<BID_UINT128>`                             | decimal                            | supported | Imports DPD via BID                            |

The conversion paths are exercised directly by the test suite across all supported formats. For **`decimal32`** and **`decimal64`**, this includes string construction, floating-point construction, explicit floating conversion, and full **DPD roundtrip** validation. **`decimal128`** is covered with the same guarantees, including string and floating-point construction as well as DPD roundtrip support, ensuring consistent behavior across all widths. In addition to conversion, the library provides full structural decomposition of BID-encoded values into: $(\text{sign},\ \text{coefficient},\ \text{exponent})$ via: **`auto [kind, coefficient, exponent] = math::decompose(x);`**

This interface is implemented uniformly across **`decimal32`**, **`decimal64`**, and **`decimal128`**, and operates over the complete IEEE 754 decimal domain. All value classes are supported, including finite values (both positive and negative), signed zero, infinities (`±inf`), and NaN.

For finite values, the decomposition follows: $x = (-1)^s \cdot C \cdot 10^e$ where:
* `kind` encodes the sign and classification
* `coefficient` is the integer significand
* `exponent` is the unbiased decimal exponent

<div class="clear-both"></div>