#ifndef _CYGTRACE_EXPORT_H_
#define _CYGTRACE_EXPORT_H_

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAX_EVENT_NUM 1048576

struct event {
  char sname[256];
  char fname[256];
  pthread_t tid;
  struct timespec t_beg;
  struct timespec t_end;
};

int pt_event = -1;
int full = 0;
struct event events[MAX_EVENT_NUM];

void cygtrace_callback_export(void *this_func, void *call_site, const char *sname, const char *fname, pthread_t tid,
                              const struct timespec *t_beg, const struct timespec *t_end) {
  struct event *ev = &events[++pt_event];
  pt_event = pt_event == MAX_EVENT_NUM - 1 ? -1 : pt_event;
  full = full || pt_event == -1;
  strcpy(ev->sname, sname);
  strcpy(ev->fname, fname);
  ev->tid = tid;
  ev->t_beg = *t_beg;
  ev->t_end = *t_end;
}

double ts2double(const struct timespec *ts) { return 1e6 * ts->tv_sec + 1e-3 * ts->tv_nsec; }

int cygtrace_export_json(const char *filename) {
  printf("cygtrace: exporting %d entries\n", full ? MAX_EVENT_NUM : pt_event + 1);
  __pid_t pid = getpid();
  const char *ev_fmt =
      "{"
      "\"name\":\"%s\","
      "\"cat\":\"cygtrace\","
      "\"ph\":\"%s\","
      "\"ts\":%lf,"
      "\"pid\":%d,"
      "\"tid\":%ld"
      "},";
  FILE *fp = fopen(filename, "w");
  if (!fp) {
    fputs("cygtrace: failed to open file", stderr);
    return 1;
  }
  fputs("{\"traceEvents\":[", fp);
  int i = full ? pt_event : -1;
  while (i != pt_event) {
    const struct event *ev = &events[++i];
    fprintf(fp, ev_fmt, ev->sname, "B", ts2double(&(ev->t_beg)), pid, ev->tid);
    fprintf(fp, ev_fmt, ev->sname, "E", ts2double(&(ev->t_end)), pid, ev->tid);
    i = i == MAX_EVENT_NUM - 1 ? -1 : i;
  }
  if (pt_event != -1 || full) fseek(fp, -1, SEEK_CUR);
  fputs("]}", fp);
  fclose(fp);
  if (ferror(fp)) {
    fputs("cygtrace: I/O error occured", stderr);
    return 1;
  }
  return 0;
}

#endif
