
All constants are compile-time initialized via string → `decimal_t<T>` conversion. High-precision decimal constants defined in `math::constants<T>`, where `T` controls precision.


=== "Example"
    ```cpp
    using decimal = math::constants<uint64_t>;

    auto x = decimal::pi;
    auto y = decimal::sqrt2;
    auto z = decimal::deg_to_rad * angle;
    // composability
    auto circumference = decimal::two_pi * r;
    auto angle_rad     = deg * decimal::deg_to_rad;
    auto exp_growth    = decimal::e.pow(t);
    ```
=== ":material-decimal-decrease:{.icon} decimal32"

    ```cpp
    template <> struct constants<uint32_t>{ // 7 digit precision
        using decimal_t = typename math::decimal_t<uint32_t>;
        inline static const decimal_t nan          = static_cast<decimal_t>("nan");
        inline static const decimal_t inf          = static_cast<decimal_t>("inf");
        inline static const decimal_t zero         = static_cast<decimal_t>("0");
        inline static const decimal_t one          = static_cast<decimal_t>("1");

        inline static const decimal_t pi           = static_cast<decimal_t>("3.141593");
        inline static const decimal_t tau          = static_cast<decimal_t>("6.283185");
        inline static const decimal_t e            = static_cast<decimal_t>("2.718282");
        inline static const decimal_t phi          = static_cast<decimal_t>("1.618034");
        inline static const decimal_t golden_ratio = phi;
        inline static const decimal_t silver_ratio = static_cast<decimal_t>("2.414214");
        inline static const decimal_t sqrt2        = static_cast<decimal_t>("1.414214");
        inline static const decimal_t sqrt3        = static_cast<decimal_t>("1.732051");
        inline static const decimal_t sqrt5        = static_cast<decimal_t>("2.236068");
        inline static const decimal_t ln2          = static_cast<decimal_t>("0.693147");
        inline static const decimal_t ln10         = static_cast<decimal_t>("2.302586");
        inline static const decimal_t log2e        = static_cast<decimal_t>("1.442696");
        inline static const decimal_t log10e       = static_cast<decimal_t>("0.4342945");
        inline static const decimal_t deg_to_rad   = static_cast<decimal_t>("0.01745329");
        inline static const decimal_t rad_to_deg   = static_cast<decimal_t>("57.29578");
        inline static const decimal_t inv_pi       = static_cast<decimal_t>("0.3183099");
        inline static const decimal_t two_pi       = static_cast<decimal_t>("6.283185");
        inline static const decimal_t half_pi      = static_cast<decimal_t>("1.570796");
        inline static const decimal_t quarter_pi   = static_cast<decimal_t>("0.7853982");
        inline static const decimal_t cube_root_2  = static_cast<decimal_t>("1.259921");
        inline static const decimal_t cube_root_3  = static_cast<decimal_t>("1.4422496");
    };
    ```
=== ":material-decimal:{.icon} decimal64"

    ```cpp
    template <> struct constants<uint64_t> { // 16 digit precision
        using decimal_t = typename math::decimal_t<uint64_t>;
        inline static const decimal_t nan          = static_cast<decimal_t>("nan");
        inline static const decimal_t inf          = static_cast<decimal_t>("inf");
        inline static const decimal_t zero         = static_cast<decimal_t>("0");
        inline static const decimal_t one          = static_cast<decimal_t>("1");

        inline static const decimal_t pi           = static_cast<decimal_t>("3.141592653589793");
        inline static const decimal_t tau          = static_cast<decimal_t>("6.283185307179586");
        inline static const decimal_t e            = static_cast<decimal_t>("2.718281828459045");
        inline static const decimal_t phi          = static_cast<decimal_t>("1.618033988749895");
        inline static const decimal_t silver_ratio = static_cast<decimal_t>("2.414213562373095");
        inline static const decimal_t sqrt2        = static_cast<decimal_t>("1.414213562373095");
        inline static const decimal_t sqrt3        = static_cast<decimal_t>("1.732050807568877");
        inline static const decimal_t sqrt5        = static_cast<decimal_t>("2.236067977499790");
        inline static const decimal_t ln2          = static_cast<decimal_t>("0.6931471805599453");
        inline static const decimal_t ln10         = static_cast<decimal_t>("2.302585092994046");
        inline static const decimal_t log2e        = static_cast<decimal_t>("1.442695040888963");
        inline static const decimal_t log10e       = static_cast<decimal_t>("0.4342944819032518");
        inline static const decimal_t deg_to_rad   = static_cast<decimal_t>("0.01745329251994330"); 
        inline static const decimal_t rad_to_deg   = static_cast<decimal_t>("57.29577951308232");
        inline static const decimal_t inv_pi       = static_cast<decimal_t>("0.3183098861837907");
        inline static const decimal_t two_pi       = static_cast<decimal_t>("6.283185307179586");
        inline static const decimal_t half_pi      = static_cast<decimal_t>("1.570796326794897");
        inline static const decimal_t quarter_pi   = static_cast<decimal_t>("0.785398163397448");
        inline static const decimal_t cube_root_2  = static_cast<decimal_t>("1.259921049894873");
        inline static const decimal_t cube_root_3  = static_cast<decimal_t>("1.442249570307408");
    };
    ```
=== ":material-decimal-increase:{.icon} decimal128"

    ```cpp
    template <> struct constants<BID_UINT128> { // 34 digit precision
        using decimal_t = typename math::decimal_t<BID_UINT128>;
        inline static const decimal_t nan          = static_cast<decimal_t>("nan");
        inline static const decimal_t inf          = static_cast<decimal_t>("inf");
        inline static const decimal_t zero         = static_cast<decimal_t>("0");
        inline static const decimal_t one          = static_cast<decimal_t>("1");

        inline static const decimal_t pi           = static_cast<decimal_t>("3.1415926535897932384626433832795029");
        inline static const decimal_t tau          = static_cast<decimal_t>("6.2831853071795864769252867665590058");
        inline static const decimal_t e            = static_cast<decimal_t>("2.7182818284590452353602874713526625");
        inline static const decimal_t phi          = static_cast<decimal_t>("1.6180339887498948482045868343656381");
        inline static const decimal_t golden_ratio = phi;
        inline static const decimal_t silver_ratio = static_cast<decimal_t>("2.4142135623730950488016887242096981");
        inline static const decimal_t sqrt2        = static_cast<decimal_t>("1.4142135623730950488016887242096981");
        inline static const decimal_t sqrt3        = static_cast<decimal_t>("1.7320508075688772935274463415058724");
        inline static const decimal_t sqrt5        = static_cast<decimal_t>("2.2360679774997896964091736687312762");
        inline static const decimal_t ln2          = static_cast<decimal_t>("0.6931471805599453094172321214581766");
        inline static const decimal_t ln10         = static_cast<decimal_t>("2.3025850929940456840179914546843642");
        inline static const decimal_t log2e        = static_cast<decimal_t>("1.4426950408889634073599246810018921");
        inline static const decimal_t log10e       = static_cast<decimal_t>("0.4342944819032518276511289189166051");
        inline static const decimal_t deg_to_rad   = static_cast<decimal_t>("0.0174532925199432957692369076848861");
        inline static const decimal_t rad_to_deg   = static_cast<decimal_t>("57.2957795130823208767981548141051703");
        inline static const decimal_t inv_pi       = static_cast<decimal_t>("0.3183098861837906715377675267450287");
        inline static const decimal_t two_pi       = tau;
        inline static const decimal_t half_pi      = static_cast<decimal_t>("1.5707963267948966192313216916397514");
        inline static const decimal_t quarter_pi   = static_cast<decimal_t>("0.7853981633974483096156608458198757");
        inline static const decimal_t cube_root_2  = static_cast<decimal_t>("1.2599210498948731647672106072782284");
        inline static const decimal_t cube_root_3  = static_cast<decimal_t>("1.4422495703074083823216383107801096");
    };        
    ```    

<div class="clear-both"></div>