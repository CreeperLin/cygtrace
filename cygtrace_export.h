#ifndef _CYGTRACE_EXPORT_H_
#define _CYGTRACE_EXPORT_H_

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "cygtrace.h"

#define MAX_EVENT_NUM 1048576

struct event {
  char sname[256];
  char fname[256];
  pthread_t tid;
  struct timespec t_beg;
  struct timespec t_end;
};

int cyg_event_pt = -1;
int cyg_event_full = 0;
struct event cyg_events[MAX_EVENT_NUM];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

const char *ev_fmt_duration =
    "{"
    "\"name\":\"%s\","
    "\"cat\":\"cygtrace\","
    "\"ph\":\"%s\","
    "\"ts\":%lf,"
    "\"pid\":%d,"
    "\"tid\":%ld"
    "},";

const char *ev_fmt_complete =
    "{"
    "\"name\":\"%s\","
    "\"cat\":\"cygtrace\","
    "\"ph\":\"X\","
    "\"ts\":%lf,"
    "\"dur\":%lf,"
    "\"pid\":%d,"
    "\"tid\":%ld"
    "},";

__attribute__((no_instrument_function)) void cygtrace_callback_export(void *this_func, void *call_site,
                                                                      const char *sname, const char *fname,
                                                                      pthread_t tid, const struct timespec *t_beg,
                                                                      const struct timespec *t_end) {
  pthread_mutex_lock(&mutex);
  struct event *ev = &cyg_events[++cyg_event_pt];
  cyg_event_pt = cyg_event_pt == MAX_EVENT_NUM - 1 ? -1 : cyg_event_pt;
  cyg_event_full = cyg_event_full || cyg_event_pt == -1;
  pthread_mutex_unlock(&mutex);
  strcpy(ev->sname, sname);
  strcpy(ev->fname, fname);
  ev->tid = tid;
  ev->t_beg = *t_beg;
  ev->t_end = *t_end;
}

__attribute__((no_instrument_function)) double ts2double(const struct timespec *ts) {
  return 1e6 * ts->tv_sec + 1e-3 * ts->tv_nsec;
}

__attribute__((no_instrument_function)) int cygtrace_export_json(const char *filename) {
  int num_exports = cyg_event_full ? MAX_EVENT_NUM : cyg_event_pt + 1;
  printf("cygtrace: exporting %d entries\n", num_exports);
  __pid_t pid = getpid();
  FILE *fp = fopen(filename, "w");
  if (!fp) {
    fputs("cygtrace: failed to open file", stderr);
    return -1;
  }
  fputs("{\"traceEvents\":[", fp);
  if (cyg_event_pt != -1) {
    int i = cyg_event_full ? cyg_event_pt : -1;
    do {
      i = i == MAX_EVENT_NUM - 1 ? 0 : i + 1;
      const struct event *ev = &cyg_events[i];
      fprintf(fp, ev_fmt_complete, ev->sname, ts2double(&(ev->t_beg)),
              ts2double(&(ev->t_end)) - ts2double(&(ev->t_beg)), pid, ev->tid);
    } while (i != cyg_event_pt);
    fseek(fp, -1, SEEK_CUR);
  }
  fputs("]}", fp);
  fclose(fp);
  if (ferror(fp)) {
    fputs("cygtrace: I/O error occured", stderr);
    return -1;
  }
  return num_exports;
}

__attribute__((no_instrument_function)) int cygtrace_enable_export(long threshold) {
  if (!cygtrace_is_available()) {
    fputs("cygtrace not available", stderr);
    return 1;
  }
  cygtrace_event_set_threshold_ns(threshold);
  cygtrace_event_set_callback(cygtrace_callback_export);
  cygtrace_event_enable();
  return 0;
}

#ifdef __cplusplus
#include <string>

__attribute__((no_instrument_function)) int cygtrace_export_json(std::string filename) {
  return cygtrace_export_json(filename.c_str());
}

#endif

#endif
