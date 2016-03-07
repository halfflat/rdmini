#include <limits>
#include <cmath>
#include <iostream>
#include <type_traits>
#include <gtest/gtest.h>

#include "rdmini/util/float_bits.h"


template <typename T>
class float_bits: public ::testing::Test {};

using fptype_list=::testing::Types<float,double>;
TYPED_TEST_CASE(float_bits,fptype_list);

TYPED_TEST(float_bits,round_trip) {
    using fptype=TypeParam;
    using NL=std::numeric_limits<fptype>;
    using FB=rdmini::float_bits<fptype>;

    fptype check[]={0,1,-1,0.3,std::nextafter(fptype(0),fptype(1.0)),NL::quiet_NaN(),NL::max(),NL::infinity()};

    for (fptype f: check) {
        fptype f1=FB::as_float(FB::as_unsigned(f));
        fptype f2=FB::as_float(FB::as_integer(f));

        EXPECT_TRUE(std::isnan(f) && std::isnan(f1) || f==f1);
        EXPECT_TRUE(std::isnan(f) && std::isnan(f2) || f==f2);
    }
}

TYPED_TEST(float_bits,bits_as_fraction) {
    using fptype=TypeParam;

    using FB=rdmini::float_bits<fptype>;
    using uitype=typename FB::uint_type;

    EXPECT_EQ(fptype(0),FB::bits_as_fraction(0));

    uitype all_ones=-1;

    fptype nearly_one=FB::bits_as_fraction(all_ones);
    EXPECT_GT(fptype(1),nearly_one);
    EXPECT_EQ(fptype(1),std::nextafter(nearly_one,fptype(2)));

    // number of significant bits is 1 + number of mantissa bits
    uitype rep_0_5=uitype(1)<<(FB::fraction_bits);
    fptype fp_0_5=FB::bits_as_fraction(rep_0_5);
    EXPECT_EQ(fptype(0.5),fp_0_5);

    uitype rep_0_125=uitype(1)<<(FB::fraction_bits-2);
    fptype fp_0_125=FB::bits_as_fraction(rep_0_125);
    EXPECT_EQ(fptype(0.125),fp_0_125);

    uitype rep_0_75=uitype(3)<<(FB::fraction_bits-1);
    fptype fp_0_75=FB::bits_as_fraction(rep_0_75);
    EXPECT_EQ(fptype(0.75),fp_0_75);
}

TYPED_TEST(float_bits,scale_to_fraction) {
    using fptype=TypeParam;

    using FB=rdmini::float_bits<fptype>;
    using uitype=typename FB::uint_type;
    using itype=typename FB::int_type;

    EXPECT_EQ(fptype(0),FB::scale_to_fraction(0));

    uitype all_ones=-1;

    fptype nearly_one=FB::scale_to_fraction(all_ones);
    EXPECT_GT(fptype(1),nearly_one);
    EXPECT_EQ(fptype(1),std::nextafter(nearly_one,fptype(2)));

    fptype check[]={0.25,0.5,0.75,1/3.0,1/7.0,6.0/7.0};
    double scale=std::ldexp(1.0,std::numeric_limits<uitype>::digits); // needs to be double for exponent size!

    for (fptype f: check) {
        uitype i=(uitype)std::floor(f*scale);

        fptype result=FB::scale_to_fraction(i);
        if (std::is_same<fptype,float>::value)
            EXPECT_FLOAT_EQ(f,result);
        else if (std::is_same<fptype,double>::value)
            EXPECT_DOUBLE_EQ(f,result);
        else FAIL();
    }
}
