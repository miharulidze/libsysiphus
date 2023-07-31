#include <functional>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <unistd.h>

#if defined(__x86_64__)
#include "tsc_x86.hpp"
#else
#include "tsc_arm.hpp"
#endif

bool enable_inj;
static std::size_t inj_size;
const std::size_t default_inj_size = 0;

bool measure_inj;
std::vector<uint64_t> inj_measurements;
const std::size_t default_inj_measurements_count = 1024;
myInt64 inj_measure_start_t;

enum inj_methods {
    INJ_METHOD_NANOSLEEP = 0,
    INJ_METHOD_SUM_LOOP  = 1,
};
const std::size_t default_inj_method = INJ_METHOD_NANOSLEEP;
std::function<void(const std::size_t)> inj_f;


static inline void start_measurement()
{
    if (measure_inj && inj_measurements.size() < inj_measurements.capacity()) {
        inj_measure_start_t = start_tsc();
    }
}

static inline void end_measurement()
{
    if (measure_inj && inj_measurements.size() < inj_measurements.capacity()) {
        inj_measurements.push_back(stop_tsc(inj_measure_start_t));
    }
}


static void nanosleep_f(const long injection_size)
{
    const struct timespec sleep_time = {0, injection_size};
    nanosleep(&sleep_time, NULL);
}

static void sum_loop_f(const long injection_size)
{
    volatile uint8_t res;
    uint8_t sum = 0;

    for (auto i = injection_size; i > 0; i--)
        sum += i;

    res = sum;
}

inline void sisyphus_inject_latency()
{
    if (enable_inj) {
        start_measurement();
        inj_f(inj_size);
        end_measurement();
    }
}

int sisyphus_init()
{
    enable_inj = std::getenv("LIBSISYPHUS_INJECTION") ? true : false;

    if (enable_inj) {
        const char *inj_size_str = std::getenv("LIBSISYPHUS_INJECTION_SIZE");
        if (inj_size_str)
            inj_size = std::atoi(inj_size_str);

        const char *inj_method_str = std::getenv("LIBSISYPHUS_INJECTION_METHOD");
        std::size_t inj_method_id = inj_method_str ? std::atoi(inj_method_str) : default_inj_method;

        switch (inj_method_id) {
        case INJ_METHOD_NANOSLEEP:
            inj_f = nanosleep_f;
            break;
        case INJ_METHOD_SUM_LOOP:
            inj_f = sum_loop_f;
            break;
        default:
            inj_f = nanosleep_f;
        }

        measure_inj = std::getenv("LIBSISYPHUS_MEASURE_INJECTION_CYCLES") ? true : false;
        if (measure_inj) {
            init_tsc();
            inj_measurements.reserve(default_inj_measurements_count);
        }

        return 0;
    }

    return 1;
}

void sisyphus_finalize()
{
    if (measure_inj) {
        uint64_t sum;
        for (const auto &measurement : inj_measurements)
            sum += measurement;

        sum /= inj_measurements.size();

        std::cout << "[LIBSISYPHUS] Average cycles per latency injection is " << sum << std::endl;
    }
}
