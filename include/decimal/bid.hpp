/* This file is part of the libdecimal project and is licensed under the MIT License.
 * Copyright © 2017–2025 VargaLABS, Toronto, ON, Canada 🇨🇦 */
 
#pragma once
#include <stdfloat>
#include <format>
#include <iostream>
#include <algorithm>
#include <string>
#include <cstdint>
#include <limits>
#include <string_view>
#include <type_traits>
#include <array>
#include <bid_conf.h>
#include <bid_functions.h>
#include <dfp754.h>

#ifndef BID32_MAX_PRINTED_DIGITS
    #define BID32_MAX_PRINTED_DIGITS 20
#endif

#ifndef BID64_MAX_PRINTED_DIGITS
    #define BID64_MAX_PRINTED_DIGITS 40
#endif

#ifndef BID128_MAX_PRINTED_DIGITS
    #define BID128_MAX_PRINTED_DIGITS 80
#endif
// must go to global namespace
inline bool operator==(const BID_UINT128& a, const BID_UINT128& b) noexcept {
    return a.w[0] == b.w[0] && a.w[1] == b.w[1];
}
inline bool operator>(const BID_UINT128& a, const BID_UINT128& b) {
    return (a.w[1] > b.w[1]) || (a.w[1] == b.w[1] && a.w[0] > b.w[0]);
}
inline BID_UINT128 to_bid128(unsigned __int128 x) noexcept {
    return {static_cast<std::uint64_t>(x), static_cast<std::uint64_t>(x >> 64)};
}
namespace math::bid {
    using uint128_t = BID_UINT128; 
    enum struct category : unsigned char {
        positive = 0b0000, negative = 0b0001, pinf = 0b0010, ninf = 0b0011, nan = 0b0100, zero = 0b1000
    };
    template<class T>
    inline std::string print(bool signbit, T mantissa, int exponent, char* buffer, size_t size) {
        if (mantissa == 0) return "0";
        size_t i = size - 1;
        if(exponent > 0) // suffix with zeroes only 
            while(--exponent) buffer[i--] = '0'; 
        do {
            buffer[i--] = (mantissa % 10) + '0'; 
            mantissa /= 10;
            if(exponent < 0 && !++exponent) buffer[i--] = '.';
        } while (mantissa != 0 && i > 0);
        if(exponent < 0) {
            while(exponent++) buffer[i--] = '0';
            buffer[i--] = '.'; 
        }
        if(buffer[i+1] == '.') buffer[i--] = '0';
        if(signbit) buffer[i--] = '-';
        return std::string(buffer + i + 1, buffer + size);
    }
    inline std::tuple<category, uint32_t, int16_t> decompose(uint32_t bid){
        constexpr int32_t bias = 101;
        constexpr uint32_t discriminant = (0x78u << 24), 
            mask_100m = (0x60u << 24), mask_combination = (0x7cu << 24);            
        uint32_t combination = bid & mask_combination, sign_bit = (bid >> 31);
        if (combination < discriminant) { // not a nan or inf or zero
            int32_t exponent, mantissa;
            if((bid & mask_100m) != mask_100m){
                mantissa = static_cast<int32_t>(bid & ((0x1u << 23) - 1));
                exponent = static_cast<int32_t>((bid & (0xffu << 23)) >> 23) - bias;// 0b0111 1111 1000 0000 7f 80 00 00
            } else { // 100m
                mantissa = static_cast<int32_t>((bid & ((0x1u << 21) - 1)) | (0x1u << 23));
                exponent = static_cast<int32_t>((bid & (0xffu << 21)) >> 21) - bias;
            }
            return std::make_tuple(sign_bit ? category::negative : category::positive, mantissa, exponent);
        } else if(combination > discriminant) return std::make_tuple(category::nan, uint32_t{0}, int16_t{0});
          else return std::make_tuple(sign_bit ? category::ninf : category::pinf, 0x0, 0x0);
    }
    inline uint32_t uint32_to_bid32(uint32_t significand, int32_t exponent) {
        constexpr int bias = 101;
        constexpr uint32_t max_significand = 9'999'999u, zero = 0x32000000u;
        if (significand == 0) return zero;
        if (exponent < -95 || exponent > 96) 
            throw std::out_of_range("Exponent out of Decimal32 range.");
        uint32_t biased_exponent = static_cast<uint32_t>(exponent + bias);
        if (significand > max_significand) return zero; // by standard
        uint32_t bid = (significand & (0x1u << 23)) ? 
            ((biased_exponent << 21) | (0b011u << 29)) : (biased_exponent << 23);
        bid |= (significand & ((0x1u << 24) - 1));
        return bid;
    }
    inline std::string display_bid32(uint32_t bid) {
        std::array<char, BID32_MAX_PRINTED_DIGITS> buffer;
        auto [kind, mantissa, exponent] = decompose(bid);
        switch(kind){
            case category::nan: return "nan";
            case category::pinf: return "+inf";
            case category::ninf: return "-inf";
            default: return print(kind != category::positive, mantissa, exponent, buffer.data(), buffer.size());
        }
    }    
    inline std::tuple<category, uint64_t, int16_t> decompose(uint64_t bid){
        constexpr int64_t bias = 398;
        constexpr uint64_t discriminant = (0x78ull << 56), 
            mask_100m = (0x60ull << 56), mask_combination = (0x7cull << 56);
        uint64_t combination = bid & mask_combination, sign_bit = (bid >> 63);
        if (combination < discriminant) { // not a nan or inf or zero
            int64_t exponent, mantissa;
            if((bid & mask_100m) != mask_100m){
                mantissa = static_cast<int64_t>(bid & ((0x1ull << 53) - 1));
                exponent = static_cast<int64_t>((bid & (0x3ffull << 53)) >> 53) - bias;
            } else { // 100m
                mantissa = static_cast<int64_t>((bid & ((0x1ull << 51) - 1)) | (0x1ull << 53));
                exponent = static_cast<int64_t>((bid & (0x3ffull << 51)) >> 51) - bias;
            }
            return std::make_tuple(sign_bit ? category::negative : category::positive, mantissa, exponent);
        } else if(combination > discriminant) return std::make_tuple(category::nan, 0x0, 0x0);
        else return std::make_tuple(sign_bit ? category::ninf : category::pinf, 0x0, 0x0);
    }
    inline uint64_t uint64_to_bid64(uint64_t significand, int exponent) {
        constexpr int64_t bias = 398;
        constexpr uint64_t max_significand = 9'999'999'999'999'999ull,
            zero = 0x31C0000000000000ull;
        if (significand == 0) return zero;  // +0 in Decimal64
        if (exponent < -383 || exponent > 384) 
            throw std::out_of_range("Exponent out of Decimal64 range.");
        uint64_t biased_exponent = static_cast<uint64_t>(exponent + bias);
        if (significand > max_significand) return zero; // by standard
        uint64_t bid = ((significand & (0x1ull << 53)) != 0) ? 
            ((biased_exponent << 51) | (0b011ull << 61)) : (biased_exponent << 53);
        bid |= (significand & ((0x1ull << 53) - 1));
        return bid;
    }
    inline std::string display_bid64(uint64_t bid) {
        std::array<char, BID64_MAX_PRINTED_DIGITS> buffer;
        auto [kind, mantissa, exponent] = decompose(bid);
        switch(kind) {
            case category::nan: return "nan";
            case category::pinf: return "+inf";
            case category::ninf: return "-inf";
            default: return print(kind != category::positive, mantissa, exponent, buffer.data(), buffer.size());
        }
    }
    inline std::tuple<category, unsigned __int128, int16_t> decompose(const BID_UINT128& bid) noexcept {
        constexpr int32_t bias = 6176;
        constexpr uint64_t sign_mask = 0x8000'0000'0000'0000ull, combination_shift = 46, combination_mask  = (1ull << 17) - 1,
            top5_shift = 12, steering_mask = 0x3ull << 15, steering_11 = 0x3ull << 15, top5_inf = 0x1eull, top5_nan = 0x1full,
            coeff_small_hi_mask = (1ull << 49) - 1, coeff_large_hi_mask = (1ull << 47) - 1, exponent_mask = (1ull << 14) - 1;
        constexpr unsigned __int128 max_coeff = (static_cast<unsigned __int128>(0x0001ed09bead87c0ull) << 64) | static_cast<unsigned __int128>(0x378d8e63ffffffffull);
        const uint64_t hi = bid.w[1], lo = bid.w[0],  combination = (hi >> combination_shift) & combination_mask;
        const uint64_t top5 = combination >> top5_shift;
        const bool sign_bit = (hi & sign_mask) != 0;
        if ((combination & steering_mask) != steering_11) {
            const int32_t exponent = static_cast<int32_t>(combination >> 3) - bias;
            const unsigned __int128 mantissa = (static_cast<unsigned __int128>(hi & coeff_small_hi_mask) << 64) | static_cast<unsigned __int128>(lo);
            return { sign_bit ? category::negative : category::positive, mantissa, static_cast<int16_t>(exponent)};
        } else if (top5 == top5_nan) return {category::nan, 0, 0};
        if (top5 == top5_inf) return {sign_bit ? category::ninf : category::pinf, 0, 0};
        const int32_t exponent = static_cast<int32_t>((combination >> 1) & exponent_mask) - bias; // finite, large coefficient branch (implicit leading "100")
        unsigned __int128 mantissa = (static_cast<unsigned __int128>(1) << 113) | (static_cast<unsigned __int128>(hi & coeff_large_hi_mask) << 64) | static_cast<unsigned __int128>(lo);
        if (mantissa > max_coeff) mantissa = 0; // non-canonical coefficients decode as zero
        return { sign_bit ? category::negative : category::positive, mantissa, static_cast<int16_t>(exponent)};
    }
    inline std::string display_bid128(const BID_UINT128& bid) {
        std::array<char, BID128_MAX_PRINTED_DIGITS> buffer;
        auto [kind, coefficient, exponent] = decompose(bid);
        switch (kind) {
            case category::nan: return "nan";
            case category::pinf: return "+inf";
            case category::ninf: return "-inf";
            default:
                return print(kind != category::positive, coefficient, exponent, buffer.data(), buffer.size());
        }
    }    
    inline BID_UINT128 uint128_to_bid128_(BID_UINT128 significand, int exponent) {
        constexpr std::uint64_t bias = 6176;
        constexpr BID_UINT128 max_significand = {{0x378d8e63ffffffffull, 0x0001ed09bead87c0ull}}, zero = {{0x0000000000000000ull, 0x3040000000000000ull}};
        if (significand.w[1] == 0 && significand.w[0] == 0) return zero;
        if (exponent < -6176 || exponent > 6111) throw std::out_of_range("Exponent out of Decimal128 range.");
        if (significand > max_significand) return zero;
        const std::uint64_t biased_exponent = static_cast<std::uint64_t>(exponent) + bias;
        return {significand.w[0],(significand.w[1] & 0x0001FFFFFFFFFFFFull) | ((biased_exponent & 0x3fffull) << 49)};
    }
    inline BID_UINT128 uint128_to_bid128(BID_UINT128 significand, int exponent) {
        constexpr std::uint64_t bias = 6176;
        constexpr BID_UINT128 max_significand = {{0x378d8e63ffffffffull, 0x0001ed09bead87c0ull}}, zero = {{0x0000000000000000ull, 0x3040000000000000ull}};
        if (significand.w[1] == 0 && significand.w[0] == 0) return zero;
        if (exponent < -6176 || exponent > 6111) throw std::out_of_range("Exponent out of Decimal128 range.");
        if (significand > max_significand) return zero;
        const std::uint64_t biased_exponent = static_cast<std::uint64_t>(exponent) + bias;
        if ((significand.w[1] >> 49) == 0) return {significand.w[0], (significand.w[1] & 0x0001FFFFFFFFFFFFull) | ((biased_exponent & 0x3fffull) << 49)}; 
        // small finite branch: coefficient < 2^113
        return { significand.w[0], ((significand.w[1] & 0x00007FFFFFFFFFFFull)) | ((biased_exponent & 0x3fffull) << 47) | (0x3ull << 61)};  // large finite branch: coefficient has implicit leading 100...
    }    
}

namespace math {
    using uint128_t = BID_UINT128;
    template <class T> struct bid_t {
        bid_t(T value) : value(value){}
        operator T() const { return value; }
        T value;
    };
    template <class T> struct dpd_t {
        dpd_t(T value) : value(value){}
        operator T() const { return value; }
        T value;
    };
    template <class T> struct decimal_t;

    namespace impl {
        template<typename T> T dpd2bid(T);
        template<typename T> T bid2dpd(T);
        template<typename T> T uint2bid(T, int precision);
        template<typename T> T zero();
        template<typename F, typename T> T bid2float(F);
        template<typename F, typename T> T float2bid(F);
        template<typename T> T str2bid(std::string_view);
        template<typename T> T str2bid(const std::string&);
        template<typename T> std::string bid2str(T);
        template<typename T> T identity(T arg) { return arg; };

        template<> uint32_t bid2dpd(uint32_t bid){ return __bid_to_dpd32(bid); }
        template<> uint32_t dpd2bid(uint32_t dpd){ return __bid_dpd_to_bid32(dpd); }
        template<> uint32_t uint2bid(uint32_t significand, int exponent){ return bid::uint32_to_bid32(significand, exponent); }
        template<> uint32_t zero(){ return 0x32000000U; }
        template<> double bid2float<uint32_t, double>(uint32_t bid){ unsigned int flags; return __bid32_to_binary64(bid, 0, &flags); }
        template<> uint32_t float2bid<float, uint32_t>(float bin){ unsigned int flags; return __binary32_to_bid32(bin, 0, &flags); }
        template<> uint32_t float2bid<double, uint32_t>(double bin){ unsigned int flags; return __binary64_to_bid32(bin, 0, &flags); }
        template<> uint32_t float2bid<long double, uint32_t>(long double bin){ unsigned int flags; return __binary80_to_bid32(bin, 0, &flags); }
        template<> uint32_t str2bid(std::string_view str){ unsigned int flags; std::string tmp{str}; return __bid32_from_string(const_cast<char*>(tmp.data()), 0, &flags); }
        template<> uint32_t str2bid(const std::string& str){ unsigned int flags; return __bid32_from_string(const_cast<char*>(str.data()), 0, &flags); }
        template<> std::string bid2str(uint32_t bid){ return bid::display_bid32(bid); };

        template<> uint64_t bid2dpd(uint64_t bid){ return __bid_to_dpd64(bid); }
        template<> uint64_t dpd2bid(uint64_t dpd){ return __bid_dpd_to_bid64(dpd); }
        template<> uint64_t uint2bid(uint64_t significand, int exponent){ return bid::uint64_to_bid64(significand, exponent); }
        template<> uint64_t zero(){ return 0x31C0000000000000ULL; }
        template<> long double bid2float<uint64_t, long double>(uint64_t bid){ unsigned int flags; return __bid64_to_binary80(bid, 0, &flags); }
        template<> uint64_t float2bid<float, uint64_t>(float bin){ unsigned int flags; return __binary32_to_bid64(bin, 0, &flags); }
        template<> uint64_t float2bid<double, uint64_t>(double bin){ unsigned int flags; return __binary64_to_bid64(bin, 0, &flags); }
        template<> uint64_t float2bid<long double, uint64_t>(long double bin){ unsigned int flags; return __binary80_to_bid64(bin, 0, &flags); }
        template<> uint64_t str2bid(std::string_view str){ unsigned int flags; std::string tmp{str}; return __bid64_from_string(const_cast<char*>(tmp.data()), 0, &flags); }
        template<> uint64_t str2bid(const std::string& str){ unsigned int flags; return __bid64_from_string(const_cast<char*>(str.data()), 0, &flags); }
        template<> std::string bid2str(uint64_t bid){ return bid::display_bid64(bid); };

        template<> uint128_t bid2dpd(uint128_t bid){ return __bid_to_dpd128(bid); }
        template<> uint128_t dpd2bid(uint128_t dpd){ return __bid_dpd_to_bid128(dpd); }
        template<> uint128_t uint2bid(uint128_t significand, int exponent){ return bid::uint128_to_bid128(significand, exponent); }
        template<> uint128_t zero(){ return {}; } // TODO:  
        template<> long double bid2float<uint128_t, long double>(uint128_t bid){ unsigned int flags; return __bid128_to_binary80(bid, 0, &flags); }
        template<> uint128_t float2bid<float, uint128_t>(float bin){ unsigned int flags; return __binary32_to_bid128(bin, 0, &flags); }
        template<> uint128_t float2bid<double, uint128_t>(double bin){ unsigned int flags; return __binary64_to_bid128(bin, 0, &flags); }
        template<> uint128_t float2bid<long double, uint128_t>(long double bin){ unsigned int flags; return __binary80_to_bid128(bin, 0, &flags); }
        template<> uint128_t str2bid(std::string_view str){ unsigned int flags; std::string tmp{str}; return __bid128_from_string(const_cast<char*>(tmp.data()), 0, &flags); }
        template<> uint128_t str2bid(const std::string& str){ unsigned int flags; return __bid128_from_string(const_cast<char*>(str.data()), 0, &flags); }
        template<> std::string bid2str(uint128_t bid){ return bid::display_bid128(bid); };
    }

    // operator defintions
    #define sigma_arithmetic_operator(_type, _op, _bid_fname) inline decimal_t<_type> operator _op(decimal_t<_type> lhs, decimal_t<_type> rhs) {               \
        unsigned int flags; return decimal_t<_type>::set(_bid_fname(lhs.value, rhs.value, 0, &flags)); } 
    #define sigma_comparison_operator(_type, _op, _bid_fname) inline bool operator _op(decimal_t<_type> lhs, decimal_t<_type> rhs) {\
        unsigned int flags; return _bid_fname(lhs.value, rhs.value, &flags); }
   
    #define sigma_decimal_class(size, value_t, float_t)                                                                                                         \
        template <> struct decimal_t<value_t> {                                                                                                                 \
            decimal_t() : value(impl::zero<value_t>()){}                                                                                                        \
            decimal_t(value_t significand, int exponent) : value(impl::uint2bid<value_t>(significand, exponent)){}                                              \
            decimal_t(float_t bin) : value(impl::float2bid<float_t,value_t>(bin)){}                                                                             \
            decimal_t(dpd_t<value_t> dpd) : value(impl::dpd2bid<value_t>(dpd)){}                                                                                \
            decimal_t(const char* ptr): value(impl::str2bid<value_t>(std::string_view{ptr})){}                                                                  \
            decimal_t(std::string_view str): value(impl::str2bid<value_t>(str)){}                                                                               \
            decimal_t(const std::string& str): value(impl::str2bid<value_t>(str)){}                                                                             \
            static decimal_t<value_t> from(float bin){ return set(impl::float2bid<float, value_t>(bin));}                                                       \
            static decimal_t<value_t> from(double bin){ return set(impl::float2bid<double, value_t>(bin));}                                                     \
            static decimal_t<value_t> from(long double bin){ return set(impl::float2bid<long double, value_t>(bin));}                                           \
            static decimal_t<value_t> set(value_t value){ decimal_t decimal; decimal.value = value; return decimal;}                                            \
            explicit operator dpd_t<value_t>() const { return impl::bid2dpd<value_t>(this->value); }                                                            \
            explicit operator bid_t<value_t>() const { return value; }                                                                                          \
            explicit operator float_t() const { return impl::bid2float<value_t, float_t>(this->value); }                                                        \
            operator std::string() const { return impl::bid2str(value); }                                                                                       \
            decimal_t<value_t>& operator +=(const decimal_t& rhs) {unsigned int flags; value = __bid##size##_add(value, rhs.value, 0, &flags);  return *this; } \
            decimal_t<value_t>& operator -=(const decimal_t& rhs) {unsigned int flags; value = __bid##size##_sub(value, rhs.value, 0, &flags);  return *this; } \
            decimal_t<value_t> operator +() const { return *this; }                                                                                             \
            decimal_t<value_t> operator -() const { decimal_t<value_t> out; out.value = __bid##size##_negate(value); return out; }                              \
            value_t value;                                                                                                                                      \
        }

    sigma_decimal_class(32,  uint32_t,  double);
    sigma_decimal_class(64,  uint64_t,  long double);
    sigma_decimal_class(128, uint128_t, long double);
    // operator combinations
    #define sigma_external_operators(_bit_size_)                                                                                                                                                \
        sigma_arithmetic_operator(uint##_bit_size_##_t, +, __bid##_bit_size_##_add);               sigma_arithmetic_operator(uint##_bit_size_##_t, -, __bid##_bit_size_##_sub);                 \
        sigma_arithmetic_operator(uint##_bit_size_##_t, *, __bid##_bit_size_##_mul);               sigma_arithmetic_operator(uint##_bit_size_##_t, /, __bid##_bit_size_##_div);                 \
        sigma_comparison_operator(uint##_bit_size_##_t, ==, __bid##_bit_size_##_quiet_equal);      sigma_comparison_operator(uint##_bit_size_##_t, !=, __bid##_bit_size_##_quiet_not_equal);    \
        sigma_comparison_operator(uint##_bit_size_##_t, <, __bid##_bit_size_##_quiet_less);        sigma_comparison_operator(uint##_bit_size_##_t, >, __bid##_bit_size_##_quiet_greater);       \
        sigma_comparison_operator(uint##_bit_size_##_t, <=, __bid##_bit_size_##_quiet_less_equal); sigma_comparison_operator(uint##_bit_size_##_t, >=, __bid##_bit_size_##_quiet_greater_equal);
        // operator instatiations for various bit sizes
        sigma_external_operators(32); sigma_external_operators(64); sigma_external_operators(128); 
    #undef sigma_external_operators
    #undef sigma_decimal_class
    #undef sigma_comparison_operator
    #undef sigma_arithmetic_operator

    #define sigma_decimal_literal(type, literal) inline decimal_t<type> operator""##literal(const char* characters) { \
            std::string data(characters);                                                                             \
            data.erase(std::remove(data.begin(), data.end(), '\''), data.end());                                      \
            return decimal_t<type>(data);                                                                             \
        }        
    namespace literals {
        sigma_decimal_literal(uint32_t, _dec32);
        sigma_decimal_literal(uint64_t, _dec64);
        sigma_decimal_literal(uint128_t, _dec128);
        sigma_decimal_literal(uint64_t, _dec);
    }
    #undef sigma_decimal_literal
    // begin utilities:
    template <typename bid_t> std::tuple<bool, bid_t, int16_t> decompose(decimal_t<bid_t> bid){
        auto [discriminant, mantissa, exponent] = bid::decompose(bid.value);
        switch(discriminant) {
            case bid::category::nan: throw std::runtime_error("nan");
            case bid::category::pinf: return std::make_tuple(false, std::numeric_limits<bid_t>::max(), std::numeric_limits<int16_t>::max());
            case bid::category::ninf: return std::make_tuple(true, std::numeric_limits<bid_t>::max(), std::numeric_limits<int16_t>::max());            
            case bid::category::negative: return std::make_tuple(true, mantissa, exponent);
            case bid::category::positive: return std::make_tuple(false, mantissa, exponent);
        }
        #if defined(__cpp_lib_unreachable) && __cpp_lib_unreachable >= 202202L
            std::unreachable();
        #else
            throw std::runtime_error("invalid decimal category");
        #endif
    }
    template <> inline std::tuple<bool, BID_UINT128, int16_t> decompose(decimal_t<BID_UINT128> bid) {
        auto [discriminant, mantissa, exponent] = bid::decompose(bid.value);
        switch (discriminant) {
            case bid::category::nan: throw std::runtime_error("nan");
            case bid::category::pinf: return std::make_tuple(false, BID_UINT128{{~0ull, ~0ull}}, std::numeric_limits<int16_t>::max());
            case bid::category::ninf: return std::make_tuple(true, BID_UINT128{{~0ull, ~0ull}}, std::numeric_limits<int16_t>::max());
            case bid::category::negative: return std::make_tuple(true, to_bid128(mantissa), exponent);
            case bid::category::positive: return std::make_tuple(false, to_bid128(mantissa), exponent);
        }
    #if defined(__cpp_lib_unreachable) && __cpp_lib_unreachable >= 202202L
        std::unreachable();
    #else
        throw std::runtime_error("invalid decimal category");
    #endif
    }
    template <typename bid_t> bid_t mantissa(decimal_t<bid_t> bid) {
        auto [discriminant, mantissa, exponent] = bid::decompose(bid.value);
        switch(discriminant) {
            case bid::category::nan: throw std::runtime_error("nan");
            case bid::category::pinf: throw std::runtime_error("inexact exception: +inf");
            case bid::category::ninf: throw std::runtime_error("inexact exception: -inf");
            case bid::category::negative: throw std::runtime_error("attempting to convert negative value to unsigned");
            case bid::category::positive: 
                if constexpr (std::is_same_v<bid_t, BID_UINT128>)
                    return to_bid128(mantissa);
                else return mantissa;
        }
        #if defined(__cpp_lib_unreachable) && __cpp_lib_unreachable >= 202202L
            std::unreachable();
        #else
            throw std::runtime_error("invalid decimal category");
        #endif
    }
    // end utilities
}

#define sigma_stream_op(type) std::ostream& operator<< (std::ostream& out, math::decimal_t<type> value){             \
    return out << static_cast<std::string>(value);                                                                   \
}
namespace math {
    sigma_stream_op(uint32_t);
    sigma_stream_op(uint64_t);
    sigma_stream_op(BID_UINT128);
}
#undef sigma_stream_op

namespace std {
    template <typename integral_t>
    struct formatter<math::decimal_t<integral_t>, char> {
        formatter<std::string_view, char> delegate;

        constexpr auto parse(std::format_parse_context& ctx) {
            return delegate.parse(ctx);
        }

        template <typename format_context_t>
        auto format(const math::decimal_t<integral_t>& value, format_context_t& ctx) const {
            const std::string text = static_cast<std::string>(value);
            return delegate.format(std::string_view{text}, ctx);
        }
    };
}
namespace math {
    /*
    intel_bid_prefixes := {__bid32_ __bid64_ __bid128_}
    intel_bid_functions := {tgamma lgamma tanh tan sinh pow log2 log1p log10 hypot expm1 expm2 exp10 exp erfc erf cosh cbrt
        atanh asinh asin acosh acos rem quantize quantexp abs negate sqrt quantum pow}
    unary:= tgamma lgamma tanh tan sinh log2 log1p log10 exp expm1 expm2 exp10 erfc erf cosh cbrt atanh asinh asin acosh acos abs negate sqrt
                  32bit   64bit  128bit   
    precisions:   7       16     34
    TODO: pow hypot rem quantize quantexp quantum
    */

    #define sigma_unary_op(bit_size, fn_call)                                                                \
        decimal_t<uint##bit_size##_t> fn_call(decimal_t<uint##bit_size##_t> decimal, unsigned int flags=0U){ \
            decimal_t<uint##bit_size##_t> bid;                                                               \
            bid.value = __bid##bit_size##_##fn_call(decimal.value, BID_ROUNDING_TO_NEAREST, &flags);         \
            return bid;                                                                                      \
        }
    // problems: sigma_unary_op(N, expm2)  sigma_unary_op(N, abs)
    #define sigma_op(N) sigma_unary_op(N, tgamma) sigma_unary_op(N, lgamma)                                                       \
    sigma_unary_op(N, sin)   sigma_unary_op(N, cos)   sigma_unary_op(N, tan)   sigma_unary_op(N, sinh)  sigma_unary_op(N, cosh)   \
    sigma_unary_op(N, tanh)  sigma_unary_op(N, asin)  sigma_unary_op(N, acos)  sigma_unary_op(N, atan)  sigma_unary_op(N, asinh)  \
    sigma_unary_op(N, acosh) sigma_unary_op(N, atanh) sigma_unary_op(N, log)   sigma_unary_op(N, log2)  sigma_unary_op(N, log10)  \
    sigma_unary_op(N, log1p) sigma_unary_op(N, exp)   sigma_unary_op(N, expm1) sigma_unary_op(N, exp10)                           \
    sigma_unary_op(N, erf)   sigma_unary_op(N, erfc)  sigma_unary_op(N, cbrt)  sigma_unary_op(N, sqrt) 

    sigma_op(32); sigma_op(64); sigma_op(128);

    #undef sigma__op
    #undef sigma_unary_op
    #undef sigma_binary_op
    template <typename value_t> inline decimal_t<value_t> abs(decimal_t<value_t> x);
    #define sigma_abs(bit_size, value_type)                                 \
        inline decimal_t<value_type> abs(decimal_t<value_type> x) {         \
            decimal_t<value_type> out;                                      \
            out.value = __bid##bit_size##_abs(x.value);                     \
            return out;                                                     \
        }

    sigma_abs(32,  uint32_t)
    sigma_abs(64,  uint64_t)
    sigma_abs(128, BID_UINT128)
}
// constants
namespace math {
    template <typename integral_type> struct constants;
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
        inline static const decimal_t golden_ratio = phi;        
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
}
