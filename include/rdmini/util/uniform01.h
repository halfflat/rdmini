#ifndef UNIFORM01_H_
#define UNIFORM01_H_

#include <cstdint>
#include <iostream>

#include "rdmini/util/float_bits.h"

/** Low(-er?) overhead uniform random distribution
 * 
 * Distribution for the half-open interval [0,1)
 * for floating point types.
 *
 * Provides optimised implementations when the
 * supplied uniform random number generator has
 * range [0,2^k-1].
 *
 * Implementation maps values from the RNG
 * monotonically; this and uniformity preserves
 * any discrepancy property of the RNG sequence.
 */

namespace rdmini {

namespace impl {
    inline constexpr bool is_pow2_minus1(std::uintmax_t i) {
        return (i&(i+1))==0;
    }

    // true if generator G produces values in [0,2^k-1]
    // where k is greater than or equal to the number
    // of representable binary digits in F
    template <typename F,typename G>
    struct use_rng_raw_bits {
        static constexpr bool value=
            G::min()==0 &&
            is_pow2_minus1(G::max()) &&
            (1ull<<std::numeric_limits<F>::digits)-1<=G::max();
    };

    template <typename fptype,bool use_raw>
    struct u01_impl {
        template <typename G>
        static fptype run(G &g) {
            return std::generate_canonical<fptype,std::numeric_limits<fptype>::digits>(g);
        }
    };
        
    template <typename fptype>
    struct u01_impl<fptype,true> {
        template <typename G>
        static fptype run(G &g) {
            using uint_type=typename rdmini::float_bits<fptype>::uint_type;
            return rdmini::float_bits<fptype>::bits_as_fraction(uint_type(g()));
        }
    };
}

template <typename fptype>
struct uniform01_distribution {
    typedef fptype result_type;
    struct param_type {};

    uniform01_distribution() {}
    explicit uniform01_distribution(param_type) {}

    // no state is maintained:
    void reset() {}
    param_type param() const { return param_type{}; }
    void param(param_type) {}

    constexpr fptype min() const { return 0; }
    constexpr fptype max() const { return 1; }
    
    bool operator==(uniform01_distribution) const { return true; }
    bool operator!=(uniform01_distribution) const { return false; }

    // sampling is specialised based on generator properties
    template <typename G>
    fptype operator()(G &g,param_type) { return (*this)(g); }

    template <typename G>
    fptype operator()(G &g) {
        return impl::u01_impl<fptype,impl::use_rng_raw_bits<fptype,G>::value>::run(g);
    }

    // for testing/verification:
    template <typename G>
    static bool use_rng_raw_bits(const G &) { return impl::use_rng_raw_bits<fptype,G>::value; }
};

template <typename charT,typename traits,typename fptype>
std::basic_ostream<charT,traits> &operator<<(std::basic_ostream<charT,traits> &os,uniform01_distribution<fptype>) {
    return os; // NOP
}

template <typename charT,typename traits,typename fptype>
std::basic_istream<charT,traits> &operator>>(std::basic_ostream<charT,traits> &is,uniform01_distribution<fptype> &) {
    return is; // NOP
}

} // namespace rdmini

#endif // ndef UNIFORM01_H_
