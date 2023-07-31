#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#if defined(__x86_64__)
#include "tsc_x86.h"
#else
#include "tsc_arm.h"
#endif

#include "libsysiphus.h"

#define MAX_INJ_MEASUREMENTS 1024

bool enable_inj;
size_t inj_size;
const size_t default_inj_size = 0;

bool measure_inj;
uint64_t inj_measurements[MAX_INJ_MEASUREMENTS];
size_t measurements_count = 0;
myInt64 inj_measure_start_t;

enum inj_methods {
    INJ_METHOD_NANOSLEEP = 0,
    INJ_METHOD_SUM_LOOP  = 1,
};
const size_t default_inj_method = INJ_METHOD_NANOSLEEP;
void (*inj_f)(const long);


static inline void start_measurement()
{
    if (measure_inj && measurements_count < MAX_INJ_MEASUREMENTS) {
        inj_measure_start_t = start_tsc();
    }
}

static inline void end_measurement()
{
    if (measure_inj && measurements_count < MAX_INJ_MEASUREMENTS) {
	inj_measurements[measurements_count++] = stop_tsc(inj_measure_start_t);
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

    for (size_t i = injection_size; i > 0; i--)
        sum += i;

    res = sum;
}

inline void sysiphus_inject()
{
    if (enable_inj) {
        start_measurement();
        inj_f(inj_size);
        end_measurement();
    }
}

int sysiphus_init()
{
    enable_inj = getenv("LIBSISYPHUS_INJECTION") ? true : false;

    if (enable_inj) {
        const char *inj_size_str = getenv("LIBSISYPHUS_INJECTION_SIZE");
        if (inj_size_str)
            inj_size = atoi(inj_size_str);

        const char *inj_method_str = getenv("LIBSISYPHUS_INJECTION_METHOD");
        size_t inj_method_id = inj_method_str ? atoi(inj_method_str) : default_inj_method;

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

        measure_inj = getenv("LIBSISYPHUS_MEASURE_INJECTION_CYCLES") ? true : false;
        if (measure_inj)
            init_tsc();

        return 0;
    }

    return 1;
}

void sysiphus_finalize()
{
    if (measure_inj) {
        uint64_t sum;
        for (size_t i = 0; i < measurements_count; i++)
            sum += inj_measurements[i];
        sum /= measurements_count;

        printf("[LIBSISYPHUS] Average cycles per latency injection is %lu\n", sum);
    }
}
