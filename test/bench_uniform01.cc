#include <random>
#include <iostream>
#include <iomanip>

#include "rdmini/util/uniform01.h"
#include "rdmini/timer.h"

namespace timer=rdmini::timer;

template <typename U,typename G>
double bench_dist(size_t n_trial,size_t n_inner,U dist,G &rng) {
    timer::hr_timer T;
    double min_time=std::numeric_limits<double>::max();

    typename U::result_type x=0;
    for (size_t trial=0;trial<n_trial;++trial) {
        T.start();
        for (size_t i=0;i<n_inner;++i) x+=dist(rng);
        T.stop();
        double t=T.time();
        if (t<min_time) min_time=t;
    }
    return min_time;
}


int main() {
    timer::hr_timer T;

    constexpr size_t n_inner=1000000;
    constexpr size_t n_trial=100;

    std::mt19937_64 g;
    std::uniform_real_distribution<double> u_stdlib;
    rdmini::uniform01_distribution<double> u_u01;

    g.seed();
    double t_u01=bench_dist(n_trial,n_inner,u_u01,g);
    t_u01/=n_inner;

    g.seed();
    double t_stdlib=bench_dist(n_trial,n_inner,u_stdlib,g);
    t_stdlib/=n_inner;

    std::cout << "Mean evaluation time over " << n_inner << " iterations (best of " << n_trial << " trials)\n";
    std::cout << "Using bit-operation implementation: " << std::boolalpha << decltype(u_u01)::use_rng_raw_bits(g) << "\n";
    std::cout << "std::uniform_real_distribution: " << t_stdlib*1.0e9 << " [ns]\n";
    std::cout << "rdmini::uniform01_distribution: " << t_u01*1.0e9 << " [ns]\n";
}

