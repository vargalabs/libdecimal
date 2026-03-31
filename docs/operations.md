---
hide:
  - toc
---

=== ":material-function:{.icon} Unary operations ℝ → ℝ"

    These operate on a single operand `x ∈ decimal_t<T>`.

    | Category           | Operation   | Symbol / Function | Description           | Notes                    |
    |--------------------|-------------|-------------------|-----------------------|--------------------------|
    | Arithmetic         | Identity    | `+x`              | Returns `x` unchanged | No-op                    |
    | Arithmetic         | Negation    | `-x`              | Additive inverse      | `x + (-x) = 0`           |
    | Root               | Square root | `sqrt(x)`         | √x                    | Exact at perfect squares |
    | Exponential        | Natural exp | `exp(x)`          | e^x                   |                          |
    | Logarithmic        | Natural log | `log(x)`          | ln(x)                 | `log(1)=0`               |
    | Logarithmic        | Base-10 log | `log10(x)`        | log₁₀(x)              |                          |
    | Trigonometric      | Sine        | `sin(x)`          | sin(x)                |                          |
    | Trigonometric      | Cosine      | `cos(x)`          | cos(x)                |                          |
    | Trigonometric      | Tangent     | `tan(x)`          | tan(x)                |                          |
    | Hyperbolic         | sinh        | `sinh(x)`         | sinh(x)               |                          |
    | Hyperbolic         | cosh        | `cosh(x)`         | cosh(x)               |                          |
    | Hyperbolic         | tanh        | `tanh(x)`         | tanh(x)               |                          |
    | Inverse trig       | arcsin      | `asin(x)`         | sin⁻¹(x)              |                          |
    | Inverse trig       | arccos      | `acos(x)`         | cos⁻¹(x)              |                          |
    | Inverse trig       | arctan      | `atan(x)`         | tan⁻¹(x)              |                          |
    | Inverse hyperbolic | asinh       | `asinh(x)`        |                       |                          |
    | Inverse hyperbolic | acosh       | `acosh(x)`        |                       |                          |
    | Inverse hyperbolic | atanh       | `atanh(x)`        |                       |                          |
    | Special            | Gamma       | `tgamma(x)`       | Γ(x)                  |                          |
    | Special            | Log Gamma   | `lgamma(x)`       | log Γ(x)              |                          |

=== ":material-swap-horizontal:{.icon} Binary operations ℝ × ℝ → ℝ"

    These operate on two operands `x, y ∈ decimal_t<T>`.

    | Category   | Operation      | Symbol   | Description             | Algebraic Guarantees        |
    |------------|----------------|----------|-------------------------|-----------------------------|
    | Arithmetic | Addition       | `x + y`  | Sum                     | Commutative, associative    |
    | Arithmetic | Subtraction    | `x - y`  | Difference              |                             |
    | Arithmetic | Multiplication | `x * y`  | Product                 | Commutative, associative    |
    | Arithmetic | Division       | `x / y`  | Quotient                | `x/x = 1` (x ≠ 0)           |
    | Assignment | Add assign     | `x += y` | In-place addition       |                             |
    | Assignment | Sub assign     | `x -= y` | In-place subtraction    |                             |
    | Assignment | Mul assign     | `x *= y` | In-place multiplication | *(expected / conventional)* |
    | Assignment | Div assign     | `x /= y` | In-place division       | *(expected / conventional)* |

=== ":material-sigma:{.icon} Structural properties"

    Enforced where exact arithmetic permits.

    | Property                | Expression                  |
    |-------------------------|-----------------------------|
    | Additive identity       | `x + 0 = x`                 |
    | Additive inverse        | `x + (-x) = 0`              |
    | Multiplicative identity | `x * 1 = x`                 |
    | Zero annihilation       | `x * 0 = 0`                 |
    | Commutativity (+, *)    | `a ∘ b = b ∘ a`             |
    | Associativity (+, *)    | `(a ∘ b) ∘ c = a ∘ (b ∘ c)` |
    | Distributivity          | `a(b + c) = ab + ac`        |

=== ":material-list-status:{.icon} Status"

    | Category        | Status       |
    | --------------- | ------------ |
    | +  -  * /       | :material-check-bold:{.text-green-700} exact      |
    | Algebraic laws  | :material-check-bold:{.text-green-700} enforced   |
    | Transcendentals | :material-check-bold:{.text-green-700} available  |
    | Cross precision | :material-check-bold:{.text-green-700} consistent |
    | Determinism     | :material-check-bold:{.text-green-700}            |

