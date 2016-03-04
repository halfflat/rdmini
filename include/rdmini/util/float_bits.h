#ifndef FLOAT_BITS_H_
#define FLOAT_BITS_H_

#include <cstdint>
#include <type_traits>
#include <limits>
#include <climits>
#include <cstring>

namespace rdmini {

template <typename F>
struct float_bits;

namespace impl {
    template <typename T>
    inline constexpr T make_mask(unsigned shift,unsigned bits) {
        static_assert(std::is_integral<T>::value,"non-integral type parameter");
        using U=typename std::make_unsigned<T>::type;

        return static_cast<T>(((U(1)<<bits)-1u)<<shift);
    }
} 

template <>
struct float_bits<float> {
    using int_type=std::int32_t;
    using uint_type=std::uint32_t;

    static_assert(sizeof(float)==sizeof(int_type),"unexpected float size");
    static_assert(std::numeric_limits<float>::is_iec559,"non-IEC559 float representation");

    static constexpr unsigned sign_offset=31;
    static constexpr unsigned sign_bits=1;
    static constexpr uint_type sign_mask=impl::make_mask<uint_type>(sign_offset,sign_bits);

    static constexpr unsigned exponent_offset=23;
    static constexpr unsigned exponent_bits=8;
    static constexpr uint_type exponent_mask=impl::make_mask<uint_type>(exponent_offset,exponent_bits);

    static constexpr uint_type exponent_bias=127;

    static constexpr unsigned fraction_offset=0;
    static constexpr unsigned fraction_bits=23;
    static constexpr uint_type fraction_mask=impl::make_mask<uint_type>(fraction_offset,fraction_bits);

    static int_type as_integer(float x) {
        int_type i;
        std::memcpy(&i,&x,sizeof(x));
        return i;
    }

    static uint_type as_unsigned(float x) {
        uint_type i;
        std::memcpy(&i,&x,sizeof(x));
        return i;
    }

    static float as_float(uint_type i) {
        float x;
        std::memcpy(&x,&i,sizeof(x));
        return x;
    }

};

template <>
struct float_bits<double> {
    using int_type=std::int64_t;
    using uint_type=std::uint64_t;

    static_assert(sizeof(double)==sizeof(int_type),"unexpected double size");
    static_assert(std::numeric_limits<double>::is_iec559,"non-IEC559 float representation");

    static constexpr unsigned sign_offset=63;
    static constexpr unsigned sign_bits=1;
    static constexpr uint_type sign_mask=impl::make_mask<uint_type>(sign_offset,sign_bits);

    static constexpr unsigned exponent_offset=52;
    static constexpr unsigned exponent_bits=11;
    static constexpr uint_type exponent_mask=impl::make_mask<uint_type>(exponent_offset,exponent_bits);

    static constexpr uint_type exponent_bias=1023;

    static constexpr unsigned fraction_offset=0;
    static constexpr unsigned fraction_bits=52;
    static constexpr uint_type fraction_mask=impl::make_mask<uint_type>(fraction_offset,fraction_bits);

    static int_type as_integer(double x) {
        int_type i;
        std::memcpy(&i,&x,sizeof(x));
        return i;
    }

    static uint_type as_unsigned(double x) {
        uint_type i;
        std::memcpy(&i,&x,sizeof(x));
        return i;
    }

    static double as_float(uint_type i) {
        double x;
        std::memcpy(&x,&i,sizeof(x));
        return x;
    }

};

/** Interpret lower order bits as fraction in [0,1) */

template <typename F>
F bits_as_fraction(typename float_bits<F>::uint_type i) {
    using X=float_bits<F>;

    return X::as_float((X::exponent_bias<<X::exponent_offset)+((i<<X::fraction_offset)&X::fraction_mask))+F(-1);
}

/** Re-scale unsigned integral argument to interval [0,1) */

template <typename F,typename T>
typename std::enable_if<std::is_integral<T>::value,F>::type
scale_to_fraction(T i) {
    using U=typename std::make_unsigned<T>::type;
    using X=float_bits<F>;

    constexpr int shift=sizeof(U)*CHAR_BIT-X::fraction_bits;

    return bits_as_fraction<F>(shift>0?(U(i)>>shift):(U(i)<<(-shift)));
}

} // namespace rdmini

#endif // ndef FLOAT_BITS_H_
