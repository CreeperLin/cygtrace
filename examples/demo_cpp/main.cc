#include <cygtrace.h>
#include <stdio.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

int add(int a, int b) { return a + b; }

int factorial(int n) { return n > 1 ? n * factorial(n - 1) : 1; }

#ifdef __cplusplus
}
#endif

int __mangled__(int x) { return x; }

double intv2double(const struct timespec *t_beg, const struct timespec *t_end) {
  return 1e6 * (t_end->tv_sec - t_beg->tv_sec) +
         1e-3 * (t_end->tv_nsec - t_beg->tv_nsec);
}

void callback(void *this_func, void *call_site, const char *sname,
              const char *fname, pthread_t tid, const struct timespec *t_beg,
              const struct timespec *t_end) {
  printf("name: %s\ttid: %ld\tduration (us): %f\n", sname, tid,
         intv2double(t_beg, t_end));
}

int main() {
  if (!cygtrace_is_available()) {
    printf("cygtrace not available\n");
    return 0;
  }
  // cygtrace_event_set_threshold_ns(0);
  cygtrace_event_set_callback(callback);
  cygtrace_event_enable();
  int a = __mangled__(2);
  int b = __mangled__(3);
  int c = add(a, b);
  int d = factorial(c);
  printf("%d\n", d);
  return 0;
}
