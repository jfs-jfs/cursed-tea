#include "fifo.h"
#include <assert.h>
#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

bool _fifo_grow(struct CtFIFO *);

bool ct_fifo_setup(struct CtFIFO *fifo, size_t item_size) {
  assert(NULL != fifo);
  fifo->count = 0;
  fifo->head_idx = 0; // Points at first item
  fifo->tail_idx = 0; // Points to the next available slot
  fifo->capacity = FIFO_INITIAL_SIZE;
  fifo->item_size = item_size;
  fifo->items = malloc(item_size * FIFO_INITIAL_SIZE);

  if (NULL == fifo->items) {
    fifo->capacity = 0;
    fifo->item_size = 0;
    return false;
  }
  pthread_mutex_init(&fifo->lock, NULL);

  return true;
}

bool ct_fifo_push(struct CtFIFO *fifo, const void *new_item) {
  assert(NULL != fifo);

  pthread_mutex_lock(&fifo->lock);
  if (fifo->capacity == fifo->count) {
    if (!_fifo_grow(fifo)) {
      pthread_mutex_unlock(&fifo->lock);
      return false;
    }
  }

  memcpy(fifo->items + (fifo->item_size * fifo->tail_idx), new_item,
         fifo->item_size);

  fifo->count++;
  fifo->tail_idx = (fifo->tail_idx + 1) % fifo->capacity;
  pthread_mutex_unlock(&fifo->lock);
  return true;
}

void *ct_fifo_next(struct CtFIFO *fifo) {
  assert(NULL != fifo);
  if (0 == fifo->count) {
    return NULL;
  }
  return fifo->items + (fifo->item_size * fifo->head_idx);
}
void ct_fifo_pop(struct CtFIFO *fifo) {
  assert(NULL != fifo);

  if (0 == fifo->count) {
    return;
  }

  pthread_mutex_lock(&fifo->lock);
  fifo->count--;
  fifo->head_idx = (fifo->head_idx + 1) % fifo->capacity;
  pthread_mutex_unlock(&fifo->lock);
}

// Not using locks as it is called internally and all calls are already inside
// locked zones
bool _fifo_grow(struct CtFIFO *fifo) {
  assert(NULL != fifo);

  // Malloc instead of realloc as it is faster and we have to rearrange the
  // memory ourselves after so no need to copy the data on allocation
  void *new_allocation = malloc(fifo->capacity * 2 * fifo->item_size);
  if (NULL == new_allocation) {
    return false;
  }

  // Cycle through items and copy them orderly
  for (size_t j = 0, el_idx = fifo->head_idx; j < fifo->count;
       el_idx = (el_idx + 1) % fifo->capacity, j++) {
    memcpy(new_allocation + (j * fifo->item_size),
           fifo->items + (el_idx * fifo->item_size), fifo->item_size);
  }

  // Delete the old buffer and update struct
  free(fifo->items);
  fifo->items = new_allocation;
  fifo->capacity *= 2;
  fifo->head_idx = 0;
  fifo->tail_idx = fifo->count;
  return true;
}

void ct_fifo_cleanup(struct CtFIFO *fifo) {
  assert(NULL != fifo);
  free(fifo->items);
  fifo->items = NULL;
  fifo->capacity = 0;
  fifo->tail_idx = 0;
  fifo->head_idx = 0;
  fifo->item_size = 0;
  fifo->count = 0;
  pthread_mutex_destroy(&fifo->lock);
}
