#include <cursed-tea/event.h>
#include "fifo.h"
#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

static struct CtFIFO EVENTS_QUEUE;

void _events_cleanup();

void _ms_sleep(long ms) {
  struct timespec ts;
  ts.tv_sec = ms / 1000;
  ts.tv_nsec = (ms % 1000) * 1000000L;
  nanosleep(&ts, NULL);
}

void ct_events_setup() {
  ct_fifo_setup(&EVENTS_QUEUE, sizeof(struct CtEvent));
  atexit(_events_cleanup);
  struct CtEvent starting_event = {START_EVENT, NULL, {}};
  ct_fifo_push(&EVENTS_QUEUE, &starting_event);
}

size_t ct_event_count() { return EVENTS_QUEUE.count; }

struct CtEvent *ct_event_next() { return (struct CtEvent *)ct_fifo_next(&EVENTS_QUEUE); }
void ct_event_consumed() { ct_fifo_pop(&EVENTS_QUEUE); }

void ct_event_send_custom(const int signal, void *data) {
  struct CtEvent custom = {CUSTOM_EVENT, data, {signal}};
  ct_fifo_push(&EVENTS_QUEUE, &custom);
}

void ct_event_send_key(const wchar_t key) {
  struct CtEvent key_ev = {KEY_EVENT, NULL, {key}};
  ct_fifo_push(&EVENTS_QUEUE, &key_ev);
}

void ct_event_send_resize(const size_t new_x, const size_t new_y) {
  struct CtEvent resize_ev;
  resize_ev.type = RESIZE_EVENT;
  resize_ev.data = NULL;
  resize_ev.new_size.x = new_x;
  resize_ev.new_size.y = new_y;
  ct_fifo_push(&EVENTS_QUEUE, &resize_ev);
}

void ct_event_send_exit() {
  struct CtEvent exit = {EXIT_EVENT, NULL, {}};
  ct_fifo_push(&EVENTS_QUEUE, &exit);
}

void *_send_delayed(void *delayed_event) {
  struct CtDelayedEvent *devent = (struct CtDelayedEvent *)delayed_event;
  _ms_sleep(devent->delay_ms);
  ct_event_send_custom(devent->event.custom_signal, devent->event.data);
  free(devent);
  return NULL;
}

void ct_event_send_custom_delayed(const int signal, const long delay_ms,
                               void *data) {
  pthread_t new_thread;
  struct CtDelayedEvent *devent = malloc(sizeof(struct CtDelayedEvent));
  devent->event.type = CUSTOM_EVENT;
  devent->event.custom_signal = signal;
  devent->event.data = data;
  devent->delay_ms = delay_ms;
  pthread_t thread = pthread_create(&new_thread, NULL, _send_delayed, devent);
  pthread_detach(thread);
}

void _events_cleanup() { ct_fifo_cleanup(&EVENTS_QUEUE); }
