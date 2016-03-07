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

    struct float_data {
        using float_type=float;
        using int_type=std::int32_t;
        using uint_type=std::uint32_t;

        static_assert(sizeof(float)==sizeof(int_type),"unexpected float size");
        static_assert(std::numeric_limits<float>::is_iec559,"non-IEC559 float representation");

        static constexpr unsigned sign_offset=31;
        static constexpr unsigned sign_bits=1;
        static constexpr uint_type sign_mask=impl::make_mask<uint_type>(sign_offset,sign_bits);

        static constexpr unsigned exponent_offset=23;
        static constexpr unsigned exponent_bits=8;
        static constexpr uint_type exponent_bias=127;

        static constexpr unsigned fraction_offset=0;
        static constexpr unsigned fraction_bits=23;
    };

    struct double_data {
        using float_type=double;
        using int_type=std::int64_t;
        using uint_type=std::uint64_t;

        static_assert(sizeof(double)==sizeof(int_type),"unexpected double size");
        static_assert(std::numeric_limits<double>::is_iec559,"non-IEC559 float representation");

        static constexpr unsigned sign_offset=63;
        static constexpr unsigned sign_bits=1;

        static constexpr unsigned exponent_offset=52;
        static constexpr unsigned exponent_bits=11;
        static constexpr uint_type exponent_bias=1023;

        static constexpr unsigned fraction_offset=0;
        static constexpr unsigned fraction_bits=52;
    };

    // common functionality
    template <typename X>
    struct float_bits_common: X {
        using typename X::int_type;
        using typename X::uint_type;
        using typename X::float_type;

        using X::fraction_bits;
        using X::fraction_offset;
        using X::exponent_bits;
        using X::exponent_offset;
        using X::exponent_bias;
        using X::sign_bits;
        using X::sign_offset;

        static constexpr uint_type sign_mask=impl::make_mask<uint_type>(sign_offset,sign_bits);
        static constexpr uint_type exponent_mask=impl::make_mask<uint_type>(exponent_offset,exponent_bits);
        static constexpr uint_type fraction_mask=impl::make_mask<uint_type>(fraction_offset,fraction_bits);

        // check consistency of bits and offsets
        static_assert(X::fraction_offset==0,"bad float bit specification");
        static_assert(X::sign_bits==1,"bad float bit specification");
        static_assert(X::fraction_offset+X::fraction_bits==X::exponent_offset,"bad float bit specification");
        static_assert(X::fraction_offset+X::fraction_bits+X::exponent_bits==X::sign_offset,"bad float bit specification");

        static int_type as_integer(float_type x) {
            int_type i;
            std::memcpy(&i,&x,sizeof(x));
            return i;
        }

        static uint_type as_unsigned(float_type x) {
            uint_type i;
            std::memcpy(&i,&x,sizeof(x));
            return i;
        }

        static float_type as_float(uint_type i) {
            float_type x;
            std::memcpy(&x,&i,sizeof(x));
            return x;
        }

        /** Interpret lower order bits as fraction in [0,1) */

        static float_type bits_as_fraction(uint_type i) {
            // m is mask for msb of lower order bits of the integer;
            // (m-1) is the mask for the remaining lower order bits.
            constexpr uint_type m=uint_type(1)<<fraction_bits;
            constexpr uint_type half=(exponent_bias-1)<<exponent_offset;

            return as_float(i&(m-1)|half)-as_float(((i&m)-1)&half);
        }

        /** Re-scale unsigned integral argument to interval [0,1) */

        template <typename T>
        static typename std::enable_if<std::is_integral<T>::value,float_type>::type
        scale_to_fraction(T i) {
            using U=typename std::make_unsigned<T>::type;
            constexpr int shift=(int)(sizeof(U)*CHAR_BIT)-(int)(fraction_bits+1);

            return bits_as_fraction(shift>0?(U(i)>>shift):(U(i)<<(-shift)));
        }
    };
} 


template <>
struct float_bits<float>: impl::float_bits_common<impl::float_data> {};

template <>
struct float_bits<double>: impl::float_bits_common<impl::double_data> {};

} // namespace rdmini

#endif // ndef FLOAT_BITS_H_
