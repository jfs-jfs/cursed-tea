#pragma once

#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define FIFO_INITIAL_SIZE 16
// A Growable ring buffer really
struct CtFIFO {
  pthread_mutex_t lock;
  size_t count;
  size_t capacity;
  size_t item_size;
  size_t head_idx;
  size_t tail_idx;
  uint8_t *items; // Avoid undefined behaviour of void pointer arithmetic
};

/* All operations that require a CtFIFO pointer will assert it is not null */

bool ct_fifo_setup(struct CtFIFO *fifo,
                size_t item_size); // Returns false if couldn't allocate space
void ct_fifo_cleanup(struct CtFIFO *fifo); // Cleans the internal state of the CtFIFO
                                      //
bool ct_fifo_push(
    struct CtFIFO *fifo,
    const void *new_item);          // Returns false if couldn't allocate space
void ct_fifo_pop(struct CtFIFO *fifo);   // Removes next item from memory
void *ct_fifo_next(struct CtFIFO *fifo); // Gets a pointer to the next item
