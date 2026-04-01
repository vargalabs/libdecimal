---
hide:
  - toc
---

**LIBDECIMAL** is a focused, header-only **C++** library for precise, deterministic decimal arithmetic built directly on Intel LIBBID — without exposing its low-level, error-prone interface. It delivers exact base-10 semantics, preserves native BID representations for seamless interop, and integrates cleanly with modern C++ (C++20/23), all while remaining lightweight and dependency-free. The result is predictable, audit-friendly arithmetic you can trust in trading, risk, and financial systems — where “almost correct” is simply wrong.


## :material-battery-positive:{.icon} What it Does
**LIBDECIMAL** provides **exact base-10 arithmetic** with deterministic behavior, built directly on Intel LIBBID — but without exposing its unsafe, low-level interface. In practice, it gives you:

* **Exact decimal math** — no binary rounding artifacts (`0.1 + 0.2 == 0.3`, always)
* **Deterministic results** — identical inputs → identical outputs across platforms
* **Native BID representation** — zero-copy interop with hardware and Intel tooling
* **Full control over precision and rounding** — no hidden surprises
* **Seamless C++ integration** — idiomatic types, conversions, and formatting
* **Audit-friendly behavior** — every value has a clear, reproducible representation
* **Send decimal values to** Python or JavaScript — **no hacks**, no precision loss
* **Bit-exact compatibility** — no surprises
* **Zero overhead interop** — plug into what you already have

## :material-chart-line:{.icon} Why you need this (Trading & Risk Systems)
In trading systems, **numerical error is not a rounding issue — it is a PnL issue**. Binary floating-point (`float`, `double`) introduces small inconsistencies that accumulate:$0.1 + 0.2 \ne 0.3$
That’s harmless in graphics or UI code. In trading infrastructure, it quietly infects every layer — pricing, execution, accounting — until small discrepancies turn into real financial exposure. The problem isn’t just precision; it’s **systemic inconsistency**. What starts as a tiny rounding error propagates through matching engines, fee calculations, and risk models, eventually surfacing as mismatches between systems that are supposed to agree. Below is how that breakdown happens — and how `libdecimal` eliminates it at the source:

=== ":material-alert:{.icon} Where floats break"

    That’s harmless in graphics. It is **not harmless** in:

    - order matching  
    - fee calculation  
    - position tracking  
    - risk aggregation  
    - settlement and reconciliation  

=== ":material-bug:{.icon} The real problems"

    - **Hidden drift** → balances don’t reconcile  
    - **Non-determinism** → backtest ≠ live  
    - **Rounding inconsistencies** → exchange vs internal mismatch  
    - **Audit failures** → cannot reproduce historical results exactly  

=== ":material-shield-check:{.icon} What libdecimal fixes"

    - **Exact pricing**  
      Prices, sizes, fees — computed in base-10 without loss  

    - **Deterministic execution paths**  
      Same inputs → same outputs (backtest == production)  

    - **Exchange-aligned arithmetic**  
      Matches tick sizes, lot sizes, and fee rules  

    - **Reliable reconciliation**  
      Internal books match external statements exactly  

    - **Auditability**  
      Every number can be traced and reproduced  

## :material-compare:{.icon} Why not something else?
Because your stack already speaks **BID**. Exchanges, gateways, SDKs — somewhere in the pipeline, **Intel decimal is already there**. If you bring in another decimal library, you’re introducing **conversions, rounding differences, subtle inconsistencies** $\text{same value} \neq \text{same behavior}$

## :material-run-fast:{.icon} This is not for you if

=== ":material-thumb-down:{.text-red-600} You’re fine with this"

    - You think `double` is “good enough”  
    - You’re fine with **`0.1 + 0.2 ≈ 0.3`**  
    - You accept **rounding drift** as a fact of life  
    - You don’t need **deterministic results** across systems  
    - You’ve never had to **reconcile real money**  
    - You’re building UI, graphics, or anything where **nobody audits the numbers**  

=== ":material-alert-circle:{.text-red-600} Then own the consequences"

    - mismatched balances → **your problem**  
    - inconsistent backtests → **your problem**  
    - rounding differences vs exchange → **your problem**  
    - audit trail doesn’t reproduce → **your problem**  


???+ example ":material-information-outline:{.icon .mr-3} Attribution & Design"

    **LIBDECIMAL** builds on Intel’s LIBBID implementation of IEEE 754 decimal arithmetic. The heavy lifting — the arithmetic itself — comes from the work of  **Marius Cornea**, **John Harrison**, **Cristina Anderson**, and **Evgeny Gvozdev**.  The underlying model traces back to **Mike Cowlishaw** and the IEEE 754 standard.

    **The template/macro contraption** that makes it usable in modern C++, along **with the full decomposition of BID into sign, significand, and exponent — that’s on me.**

<div class="clear-both"></div>