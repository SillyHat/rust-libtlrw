#include "atomic.h"

#define CACHELINE_SIZE 64

/* ======================
 *  TLRW STUFF GOES HERE
 * ======================
 */

/*
 * TLRW bytelock structure.
 */

struct TLRWByteLock {
    uint32_t owner;
    uint32_t read_counter;
    uint8_t byte_array[CACHELINE_SIZE - sizeof(uint32_t) * 2];
};

/*
 * The bytelock initialization unit.
 * Returns an empty bytelock structure.
 */

void tlrw_bytelock_init(struct TLRWByteLock *bytelock) {
    bytelock->owner = 0;
    bytelock->read_counter = 0;
    
    uint32_t i;
    for (i = 0; i < sizeof(bytelock->byte_array); ++i)
        bytelock->byte_array[i] = 0;
    
    return;
}

/*
 * This is what happens when we try to acquire a bytelock for writing.
 */

void tlrw_bytelock_write_lock(struct TLRWByteLock *bytelock, int32_t slot) {
    
    /*
     * The lock is owned by someone else? We'll just wait.
     */
    
    while(!cas_u32(&bytelock->owner, 0, slot))
        cpu_stall();
    
    /*
     * If we are slotted and we were reading something here,
     * let's 'upgrade' from a reader to a writer.
     */
    
    if (slot <= sizeof(bytelock->byte_array))
        store_u8(&bytelock->byte_array[slot - 1], 0);
    
    /*
     * Now let's wait until all the readers (slotted and unslotted)
     * are drained out.
     */
    
    uint32_t i;
    for (i = 0; i < sizeof(bytelock->byte_array); ++i)
        while (load_u8(&bytelock->byte_array[i]))
            cpu_stall();
    
    while (load_u32(&bytelock->read_counter))
        cpu_stall();
    
    return;
}

/*
 * Done writing? Just disown the lock, simple as that!
 */

void tlrw_bytelock_write_unlock(struct TLRWByteLock *bytelock, uint32_t slot) {
    if (load_u32(&bytelock->owner) == slot)
        store_u32(&bytelock->owner, 0);
    
    return;
}

/*
 * This time we are acquring the lock for reading.
 * Let's see what we have to do.
 */

void tlrw_bytelock_read_lock(struct TLRWByteLock *bytelock, uint32_t slot) {
    
    /*
     * If we are owning this lock, just 'downgrade' back
     * to the reader state.
     */
    
    if (load_u32(&bytelock->owner) == slot) {
        store_u8(&bytelock->byte_array[slot - 1], 1);
        store_u32(&bytelock->owner, 0);
        return;
    }
    
    /*
     * Unslotted? Use the readers counter. Look out for the writers!
     */
    
    if (slot > sizeof(bytelock->byte_array)) {
        while (1 < 2) {
            inc_u32(&bytelock->read_counter);
            if (!load_u32(&bytelock->owner))
                break;
            dec_u32(&bytelock->read_counter);
            while (load_u32(&bytelock->owner))
                cpu_stall();
        }
        return;
    }
    
    /*
     * Same stuff, but for slotted guys.
     */
    
    while (1 < 2) {
        store_u8(&bytelock->byte_array[slot - 1], 1);
        if (!load_u32(&bytelock->owner))
            break;
        store_u8(&bytelock->byte_array[slot - 1], 0);
        while (load_u32(&bytelock->owner))
            cpu_stall();
    }
    
    return;
}

/*
 * Done reading? Unslotted threads decrement the readers counter,
 * slotted threads set their array byte to 0.
 */

void tlrw_bytelock_read_unlock(struct TLRWByteLock *bytelock, uint32_t slot) {
    if (slot > sizeof(bytelock->byte_array))
        dec_u32(&bytelock->read_counter);
    else
        store_u8(&bytelock->byte_array[slot - 1], 0);
    
    return;
}
