#pragma once
#include <stdint.h>

typedef uint32_t u32;

/* from coreinit.rpl */
#ifdef WIIU
void OSSetPerformanceMonitor(u32, u32, u32, u32, u32, u32, u32);
#endif

/* Start cycle and instruction counters */
static inline void wiiu_start_perf_counters() {
#ifdef WIIU
    OSSetPerformanceMonitor(0b00001101, 0b11000000, 0, 0, 0, 0, 0);
#endif
}

/* Stop cycle and instruction counters */
static inline void wiiu_stop_perf_counters() {
#ifdef WIIU
    OSSetPerformanceMonitor(0b00000001, 0, 0, 0, 0, 0, 0);
#endif
}

/* Reset cycle and instruction counters */
static inline void wiiu_reset_perf_counters() {
#ifdef WIIU
    OSSetPerformanceMonitor(0b00001100, 0, 0, 0, 0, 0, 0);
#endif
}

static inline u32 wiiu_get_cycles() {
#ifdef WIIU
    u32 result;
    __asm__ volatile("mfspr %0, 937" : "=r" (result) : );
    return result;
#else
    u32 hi, lo;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return lo;
#endif
}

static inline u32 wiiu_get_insns() {
#ifdef WIIU
    u32 result;
    __asm__ volatile("mfspr %0, 938" : "=r" (result) : );
    return result;
#else
    return 0;
#endif
}

#define WIIU_PERF_START() \
    u32 cycles_start = wiiu_get_cycles();

#define WIIU_PERF_END(CTR, OP) \
    extern uint64_t perf_cycles_##CTR; \
    u32 cycles_end = wiiu_get_cycles(); \
    perf_cycles_##CTR OP cycles_end - cycles_start;
