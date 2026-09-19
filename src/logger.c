#include <cursed-tea/logger.h>
#include <stdio.h>

struct CtLogger LOGGER = {NULL, LOG_TRACE, {}};

void _logger_cleanup();
void _ensure_setup();
void _write(const wchar_t *level_str, const wchar_t *msg,
            const wchar_t *at_file, const size_t at_line);

void ct_logger_setup() {
  LOGGER.stream = stderr;
  LOGGER.level = LOG_TRACE;
  pthread_mutex_init(&LOGGER.lock, NULL);
  atexit(_logger_cleanup);
}

void ct_logger_level(const enum CtLogLevel level) {
  _ensure_setup();
  LOGGER.level = level;
}

void ct_logger_file(const char *file_path, const bool append) {
  _ensure_setup();
  if (LOGGER.stream != stderr) {
    fclose(LOGGER.stream);
    LOGGER.stream = NULL;
  }

  if (append) {
    LOGGER.stream = fopen(file_path, "a");
    return;
  }
  LOGGER.stream = fopen(file_path, "w");
}

void ct_logger_trace(const char *function, const wchar_t *file,
                  const size_t line_number) {
  _ensure_setup();
  if (LOGGER.level > LOG_TRACE)
    return;

  wchar_t buffer[1024];
  if (mbstowcs(buffer, function, 1024) == (size_t)-1)
    buffer[0] = L'\0';
  else
    buffer[1023] = L'\0';
  _write(L"TRACE", buffer, file, line_number);
}

void ct_logger_debug(const wchar_t *msg, const wchar_t *file,
                  const size_t line_number) {
  _ensure_setup();
  if (LOGGER.level > LOG_DEBUG)
    return;
  _write(L"DEBUG", msg, file, line_number);
}

void ct_logger_info(const wchar_t *msg, const wchar_t *file,
                 const size_t line_number) {
  _ensure_setup();
  if (LOGGER.level > LOG_INFO)
    return;
  _write(L"INFO", msg, file, line_number);
}

void ct_logger_warn(const wchar_t *msg, const wchar_t *file,
                 const size_t line_number) {
  _ensure_setup();
  if (LOGGER.level > LOG_WARN)
    return;
  _write(L"WARN", msg, file, line_number);
}

void ct_logger_error(const wchar_t *msg, const wchar_t *file,
                  const size_t line_number) {
  _ensure_setup();
  if (LOGGER.level > LOG_ERR)
    return;
  _write(L"ERROR", msg, file, line_number);
}

void ct_logger_crit(const wchar_t *msg, const wchar_t *file,
                 const size_t line_number) {
  _ensure_setup();
  if (LOGGER.level > LOG_CRIT)
    return;
  _write(L"CRIT", msg, file, line_number);
  perror("CRITICAL LOG EVENT STOPPING");
  exit(EXIT_FAILURE);
}

void ct_logger_formatted(const wchar_t *file, const size_t line_number,
                      const enum CtLogLevel level, const wchar_t *fmt, ...) {
  if (LOGGER.level > level)
    return;

  wchar_t buffer[2048];
  va_list args;
  va_start(args, fmt);

  vswprintf(buffer, sizeof(buffer) / sizeof(buffer[0]), fmt, args);

  va_end(args);

  wchar_t level_buffer[10];
  switch (level) {
  case LOG_TRACE:
    swprintf(level_buffer, sizeof(level_buffer) / sizeof(level_buffer[0]),
             L"TRACE");
    break;
  case LOG_DEBUG:
    swprintf(level_buffer, sizeof(level_buffer) / sizeof(level_buffer[0]),
             L"DEBUG");
    break;
  case LOG_INFO:
    swprintf(level_buffer, sizeof(level_buffer) / sizeof(level_buffer[0]),
             L"INFO ");
    break;
  case LOG_WARN:
    swprintf(level_buffer, sizeof(level_buffer) / sizeof(level_buffer[0]),
             L"WARN ");
    break;
  case LOG_ERR:
    swprintf(level_buffer, sizeof(level_buffer) / sizeof(level_buffer[0]),
             L"ERROR");
    break;
  case LOG_CRIT:
    swprintf(level_buffer, sizeof(level_buffer) / sizeof(level_buffer[0]),
             L"CRIT ");
    break;
  }

  _write(level_buffer, buffer, file, line_number);
}

/* INTERNAL */

void _ensure_setup() {
  if (LOGGER.stream == NULL) {
    ct_logger_setup();
  }
}

void _logger_cleanup() {
  pthread_mutex_destroy(&LOGGER.lock);
  if (LOGGER.stream != stderr) {
    fclose(LOGGER.stream);
    LOGGER.stream = NULL;
  }
}

void _write(const wchar_t *level_str, const wchar_t *msg,
            const wchar_t *at_file, const size_t at_line) {
  assert(LOGGER.stream != NULL);

  pthread_mutex_lock(&LOGGER.lock);

  time_t current_time;
  struct tm *local_time;
  char time_str[1024];

  time(&current_time);
  local_time = localtime(&current_time);
  strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local_time);

  fprintf(LOGGER.stream,
          "LOG EVENT :: %s :: [ lvl=%ls file=%ls line=%zu msg='%ls' ]\n",
          time_str, level_str, at_file, at_line, msg);

  fflush(LOGGER.stream);

  pthread_mutex_unlock(&LOGGER.lock);
}
