/* ===================
 *  ALL THINGS ATOMIC
 * ===================
 */

#ifndef ATOMIC_H__
#define ATOMIC_H__

#if (defined(__i386__) || defined(__x86_64__)) && defined(__GNUC__)

#include <inttypes.h>

/*
 * Stall and memory fence (compile-time) operations.
 */
 
void cpu_stall(void) {
    __asm__ __volatile__("pause" ::: "memory");
}

void memory_barrier(void) {
    __asm__ __volatile__("" ::: "memory");
}

/*
 * Compare-and-swap for 32-bit integers.
 */
 
uint32_t cas_u32(uint32_t *item, uint32_t compare, uint32_t set) {
    uint32_t value;
    __asm__ __volatile__("lock cmpxchgl %1, %2"
                        : "=a" (value)
                        : "q"  (set),
                          "m"  (*item),
                          "a"  (compare)
                        : "memory", "cc");
    return value;
}

/*
 * Atomic store-to-memory functions for
 * 8-bit and 32-bit integers.
 */

void store_u8(uint8_t *item, uint8_t value) {
    __asm__ __volatile__("movb %1, %0"
                        : "=m"  (*item)
                        : "q"   (value)
                        : "memory", "cc");
}

void store_u32(uint32_t *item, uint32_t value) {
    __asm__ __volatile__("movl %1, %0"
                        : "=m"  (*item)
                        : "q"   (value)
                        : "memory", "cc");
}

/*
 * Atomic load-from-memory functions for
 * 8-bit and 32-bit integers.
 */

uint8_t load_u8(uint8_t *item) {
    uint8_t value;
    __asm__ __volatile__("movb %1, %0"
                        : "=q"  (value)
                        : "m"   (*item)
                        : "memory", "cc");
    return value;
}

uint32_t load_u32(uint32_t *item) {
    uint32_t value;
    __asm__ __volatile__("movl %1, %0"
                        : "=q"  (value)
                        : "m"   (*item)
                        : "memory", "cc");
    return value;
}

/*
 * Atomic increment and decrement functions for
 * 32-bit integers. Not really needed since
 * we have atomic store-to-memory stuff, but they may come in handy.
 */

void inc_u32(uint32_t *item) {
    __asm__ __volatile__("lock incl %0"
                        :   "+m" (*item)
                        ::  "memory", "cc");
}

void dec_u32(uint32_t *item) {
    __asm__ __volatile__("lock decl %0"
                        :   "+m" (*item)
                        ::  "memory", "cc");
}

#else
#error Something happened...
#endif

#endif /* ATOMIC_H__ */
