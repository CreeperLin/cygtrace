#include "cygtrace.h"

#include <dlfcn.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef CYGTRACE_DEMANGLE
#include <cxxabi.h>
#endif

static int cygtrace_available = -1;
static int cygtrace_enabled = 0;
static void (*cyg_callback_enter)(CYGTRACE_CALLBACK_SIG) = NULL;
static void (*cyg_callback_exit)(CYGTRACE_CALLBACK_SIG) = NULL;

#define CYG_EV_MAX 65536
#define CYG_TH_MAX 128

static int cyg_ev_pts[CYG_TH_MAX] = {0};
static struct timespec cyg_ev_t_beg[CYG_TH_MAX][CYG_EV_MAX];

static int cygtrace_ev_enabled = 0;
static void (*cyg_ev_callback)(CYGTRACE_EV_CALLBACK_SIG) = NULL;
static long cyg_ev_threshold_ns = 0;

static inline void cyg_ev_cb_enter(void *this_func, void *call_site, const char *sname, const char *fname,
                                   pthread_t tid) __attribute__((no_instrument_function));
static inline void cyg_ev_cb_exit(void *this_func, void *call_site, const char *sname, const char *fname, pthread_t tid)
    __attribute__((no_instrument_function));

void cygtrace_enable(void) { cygtrace_enabled = 1; }

void cygtrace_disable(void) { cygtrace_enabled = 0; }

int cygtrace_set_callback_enter(void (*cb_enter)(CYGTRACE_CALLBACK_SIG)) {
  if (cyg_callback_enter) return 1;
  cyg_callback_enter = cb_enter;
  return 0;
}

int cygtrace_set_callback_exit(void (*cb_exit)(CYGTRACE_CALLBACK_SIG)) {
  if (cyg_callback_exit) return 1;
  cyg_callback_exit = cb_exit;
  return 0;
}

void cygtrace_unset_callback_enter(void) { cyg_callback_enter = NULL; }

void cygtrace_unset_callback_exit(void) { cyg_callback_exit = NULL; }

int cygtrace_is_enabled(void) { return cygtrace_enabled; }

void cyg_test_cb(void *this_func, void *call_site, const char *sname, const char *fname, pthread_t tid) {
  if (this_func) {
    cygtrace_available = 1;
  }
}

int cygtrace_is_available(void) {
  if (cygtrace_available != -1) return cygtrace_available;
  cygtrace_available = 0;
  cygtrace_set_callback_enter(cyg_test_cb);
  cygtrace_enable();
  cyg_test_cb(NULL, NULL, NULL, NULL, 0);
  cygtrace_disable();
  cygtrace_unset_callback_enter();
  return cygtrace_available;
}

#define GET_DLINFO              \
  if (dladdr(this_func, &di)) { \
    if (di.dli_sname) {         \
      sname = di.dli_sname;     \
    } else {                    \
      sname = "<unknown>";      \
    }                           \
    fname = di.dli_fname;       \
  }

#define DEMANGLE_SNAME                                   \
  int status;                                            \
  char *demangled;                                       \
  demangled = abi::__cxa_demangle(sname, 0, 0, &status); \
  if (!status) {                                         \
    sname = demangled;                                   \
  }

#define FREE_SNAME free(demangled);

#define INVOKE_ENTER                                                           \
  if (cygtrace_ev_enabled) {                                                   \
    cyg_ev_cb_enter(this_func, call_site, sname, fname, pthread_self());       \
  } else if (cyg_callback_enter) {                                             \
    (*cyg_callback_enter)(this_func, call_site, sname, fname, pthread_self()); \
  }

#define INVOKE_EXIT                                                           \
  if (cygtrace_ev_enabled) {                                                  \
    cyg_ev_cb_exit(this_func, call_site, sname, fname, pthread_self());       \
  } else if (cyg_callback_exit) {                                             \
    (*cyg_callback_exit)(this_func, call_site, sname, fname, pthread_self()); \
  }

void cygtrace_invoke_enter(void *this_func, void *call_site, const char *sname, const char *fname) {
  INVOKE_ENTER
}

void cygtrace_invoke_exit(void *this_func, void *call_site, const char *sname, const char *fname) {
  INVOKE_EXIT
}

void __cyg_profile_func_enter(void *this_func, void *call_site) {
  if (!cygtrace_enabled) return;
  cygtrace_enabled = 0;
  Dl_info di;
  const char *fname = NULL;
  const char *sname = NULL;
  GET_DLINFO
#ifdef CYGTRACE_DEMANGLE
  DEMANGLE_SNAME
#endif
  INVOKE_ENTER
#ifdef CYGTRACE_DEMANGLE
  FREE_SNAME
#endif
  cygtrace_enabled = 1;
}

void __cyg_profile_func_exit(void *this_func, void *call_site) {
  if (!cygtrace_enabled) return;
  cygtrace_enabled = 0;
  Dl_info di;
  const char *sname = NULL;
  const char *fname = NULL;
  GET_DLINFO
#ifdef CYGTRACE_DEMANGLE
  DEMANGLE_SNAME
#endif
  INVOKE_EXIT
#ifdef CYGTRACE_DEMANGLE
  FREE_SNAME
#endif
  cygtrace_enabled = 1;
}

__attribute__((no_instrument_function)) inline int cyg_get_ev_idx(pthread_t tid) { return tid % (CYG_TH_MAX - 1); }

static inline void cyg_ev_cb_enter(void *this_func, void *call_site, const char *sname, const char *fname,
                                   pthread_t tid) {
  int idx = cyg_get_ev_idx(tid);
  int *cyg_ev_pt = &cyg_ev_pts[idx];
  ++(*cyg_ev_pt);
  int pt = *cyg_ev_pt < CYG_EV_MAX ? *cyg_ev_pt : CYG_EV_MAX - 1;
  clock_gettime(CLOCK_MONOTONIC, &cyg_ev_t_beg[idx][pt]);
}

static inline void cyg_ev_cb_exit(void *this_func, void *call_site, const char *sname, const char *fname,
                                  pthread_t tid) {
  struct timespec t_end;
  clock_gettime(CLOCK_MONOTONIC, &t_end);
  int idx = cyg_get_ev_idx(tid);
  int *cyg_ev_pt = &cyg_ev_pts[idx];
  int pt = *cyg_ev_pt < CYG_EV_MAX ? *cyg_ev_pt : CYG_EV_MAX - 1;
  if (pt < 1) return;
  const struct timespec *t_beg = &cyg_ev_t_beg[idx][pt];
  *cyg_ev_pt = *cyg_ev_pt > 0 ? *cyg_ev_pt - 1 : 0;
  if (cyg_ev_threshold_ns && t_end.tv_sec == t_beg->tv_sec && t_end.tv_nsec - t_beg->tv_nsec < cyg_ev_threshold_ns)
    return;
  if (cyg_ev_callback) {
    cyg_ev_callback(this_func, call_site, sname, fname, tid, t_beg, &t_end);
  }
}

void cygtrace_event_enable(void) {
  memset(cyg_ev_pts, 0, CYG_TH_MAX * sizeof(int));
  clock_gettime(CLOCK_MONOTONIC, &cyg_ev_t_beg[cyg_get_ev_idx(pthread_self())][0]);
  cygtrace_enable();
  cygtrace_ev_enabled = 1;
}

void cygtrace_event_disable(void) {
  cygtrace_ev_enabled = 0;
  cygtrace_disable();
  cygtrace_unset_callback_enter();
  cygtrace_unset_callback_exit();
}

int cygtrace_event_is_enabled(void) { return cygtrace_ev_enabled; }

void cygtrace_event_set_threshold_ns(long th) { cyg_ev_threshold_ns = th; }

int cygtrace_event_set_callback(void (*cb_event)(CYGTRACE_EV_CALLBACK_SIG)) {
  if (cyg_ev_callback) return 1;
  cyg_ev_callback = cb_event;
  return 0;
}

void cygtrace_event_unset_callback(void) { cyg_ev_callback = NULL; }
