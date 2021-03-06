#ifndef CYGTRACE_H_
#define CYGTRACE_H_

#ifndef __cplusplus
#define _GNU_SOURCE
#endif

#include <pthread.h>
#include <time.h>

#define CYGTRACE_CALLBACK_SIG \
  void *, void *, const char *, const char *, pthread_t

#define CYGTRACE_EV_CALLBACK_SIG                         \
  void *, void *, const char *, const char *, pthread_t, \
      const struct timespec *, const struct timespec *

#ifdef __cplusplus
extern "C" {
#endif

void __cyg_profile_func_enter(void *this_func, void *call_site)
    __attribute__((no_instrument_function));

void __cyg_profile_func_exit(void *this_func, void *call_site)
    __attribute__((no_instrument_function));

void cygtrace_enable(void) __attribute__((no_instrument_function));

void cygtrace_disable(void) __attribute__((no_instrument_function));

int cygtrace_is_enabled(void) __attribute__((no_instrument_function));

int cygtrace_set_callback_enter(void (*cb_enter)(CYGTRACE_CALLBACK_SIG))
    __attribute__((no_instrument_function));

int cygtrace_set_callback_exit(void (*cb_exit)(CYGTRACE_CALLBACK_SIG))
    __attribute__((no_instrument_function));

void cygtrace_event_enable(void) __attribute__((no_instrument_function));

void cygtrace_event_disable(void) __attribute__((no_instrument_function));

int cygtrace_event_is_enabled(void) __attribute__((no_instrument_function));

int cygtrace_is_available(void) __attribute__((no_instrument_function));

void cygtrace_unset_callback_enter(void)
    __attribute__((no_instrument_function));

void cygtrace_unset_callback_exit(void) __attribute__((no_instrument_function));

void cygtrace_event_set_threshold_ns(long th)
    __attribute__((no_instrument_function));

int cygtrace_event_set_callback(void (*cb_event)(CYGTRACE_EV_CALLBACK_SIG))
    __attribute__((no_instrument_function));

void cygtrace_event_unset_callback(void)
    __attribute__((no_instrument_function));

#ifdef __cplusplus
};
#endif

#endif
