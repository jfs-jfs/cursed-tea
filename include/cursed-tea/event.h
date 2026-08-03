#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>
#include <wchar.h>

enum CtEventType {
  START_EVENT,
  KEY_EVENT,
  EXIT_EVENT,
  CUSTOM_EVENT,
  RESIZE_EVENT,
};

struct CtEvent {
  enum CtEventType type;

  // Custom event data
  void *data;

  // To share between all
  union {
    // KEY_EVENT
    wchar_t key;

    // RESIZE_EVENT
    struct {
      size_t x, y;
    } new_size;

    // CUSTOM_EVENT DETECTION
    int custom_signal;
  };
};

struct CtDelayedEvent {
  long delay_ms;
  struct CtEvent event;
};

void ct_events_setup();
size_t ct_event_count();
void ct_event_send_exit();
void ct_event_send_resize(const size_t new_x, const size_t new_y);
void ct_event_send_key(const wchar_t key);
void ct_event_send_custom(const int signal, void *data);
void ct_event_send_custom_delayed(const int signal, const long delay_ms,
                               void *data);

struct CtEvent *ct_event_next();
void ct_event_consumed();
