#if defined(__GNUC__) || defined(__linux__)
#define VOLATILE __volatile__
#define ASM __asm__
#else
#define ASM asm
#define VOLATILE
#endif

#define myInt64 unsigned long long
#define myInt32 unsigned int

#define COUNTER_LO(a) ((a).int32.lo)
#define COUNTER_HI(a) ((a).int32.hi)
#define COUNTER_VAL(a) ((a).int64)

#define COUNTER(a) \
	((unsigned long long)COUNTER_VAL(a))

#define COUNTER_DIFF(a,b) \
	(COUNTER(a) - COUNTER(b))

typedef union
{
    myInt64 int64;
    struct {
        myInt32 lo, hi;
    } int32;
} tsc_counter;

#define RDTSC(cpu_c) ASM VOLATILE ("rdtsc" : "=a" ((cpu_c).int32.lo), "=d"((cpu_c).int32.hi))
#define CPUID() ASM VOLATILE ("cpuid" : : "a" (0) : "bx", "cx", "dx" )

void init_tsc() {
	;
}

myInt64 start_tsc(void) {
    tsc_counter start;
    CPUID();
    RDTSC(start);
    return COUNTER_VAL(start);
}

myInt64 stop_tsc(myInt64 start) {
	tsc_counter end;
	RDTSC(end);
	CPUID();
	return COUNTER_VAL(end) - start;
}
