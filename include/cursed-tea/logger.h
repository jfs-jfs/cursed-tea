#pragma once

#include <assert.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>

#define WIDEN2(x) L##x
#define WIDEN(x) WIDEN2(x)

#define log_trace() ct_logger_trace(__FUNCTION__, WIDEN(__FILE__), __LINE__)

#define log_debug(m) ct_logger_debug((m), WIDEN(__FILE__), __LINE__)
#define log_info(m) ct_logger_info((m), WIDEN(__FILE__), __LINE__)
#define log_warn(m) ct_logger_warn((m), WIDEN(__FILE__), __LINE__)
#define log_error(m) ct_logger_error((m), WIDEN(__FILE__), __LINE__)
#define log_crit(m) ct_logger_crit((m), WIDEN(__FILE__), __LINE__)
#define log_fmt(lvl, fmt, ...)                                                 \
  ct_logger_formatted(WIDEN(__FILE__), __LINE__, (lvl), (fmt), ##__VA_ARGS__)

enum CtLogLevel {
  LOG_TRACE,
  LOG_DEBUG,
  LOG_INFO,
  LOG_WARN,
  LOG_ERR,
  LOG_CRIT,
};

struct CtLogger {
  FILE *stream;
  enum CtLogLevel level;
  pthread_mutex_t lock;
};

void ct_logger_setup();
void ct_logger_level(const enum CtLogLevel level);
void ct_logger_file(const char *file_path, const bool append);

void ct_logger_formatted(const wchar_t *file, const size_t line_number,
                      const enum CtLogLevel level, const wchar_t *fmt, ...);

void ct_logger_trace(const char *function, const wchar_t *file,
                  const size_t line_number);
void ct_logger_debug(const wchar_t *msg, const wchar_t *file,
                  const size_t line_number);
void ct_logger_info(const wchar_t *msg, const wchar_t *file,
                 const size_t line_number);
void ct_logger_warn(const wchar_t *msg, const wchar_t *file,
                 const size_t line_number);
void ct_logger_error(const wchar_t *msg, const wchar_t *file,
                  const size_t line_number);
void ct_logger_crit(const wchar_t *msg, const wchar_t *file,
                 const size_t line_number);
