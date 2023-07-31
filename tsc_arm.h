#define myInt64 unsigned long long
#define INT32 unsigned int

typedef union
{
    myInt64 int64;
    struct {INT32 lo, hi;} int32;
} tsc_counter;

static inline myInt64 rdtsc(void)
{
    myInt64 val;
    asm volatile("mrs %0, cntvct_el0" : "=r" (val));
    return val;
}

void init_tsc() {
	;
}

myInt64 start_tsc(void) {
    tsc_counter start;
    start.int64 = rdtsc();
    return start.int64;
}

myInt64 stop_tsc(myInt64 start) {
	tsc_counter end;
	end.int64 = rdtsc();
	return end.int64 - start;
}
