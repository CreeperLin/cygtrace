#include <cygtrace.h>
#include <cygtrace_export.h>
#include <time.h>

int fibonacci(int n) {
  int x;
  if (n == 0)
    x = 0;
  else if (n == 1)
    x = 1;
  else
    x = fibonacci(n - 1) + fibonacci(n - 2);
  printf("F(%d)=%d%c", n, x, n > 4 ? '\n' : '\t');
  return x;
}

double intv2double(const struct timespec *t_beg, const struct timespec *t_end) {
  return 1e6 * (t_end->tv_sec - t_beg->tv_sec) + 1e-3 * (t_end->tv_nsec - t_beg->tv_nsec);
}

int main() {
  cygtrace_enable_export(0);
  struct timespec t0, t1;
  int n = 10;
  clock_gettime(CLOCK_MONOTONIC, &t0);
  fibonacci(n);
  clock_gettime(CLOCK_MONOTONIC, &t1);
  cygtrace_event_disable();
  cygtrace_export_json("result.json");
  printf("w/ tracing: %lf\n", intv2double(&t0, &t1));
  clock_gettime(CLOCK_MONOTONIC, &t0);
  fibonacci(n);
  clock_gettime(CLOCK_MONOTONIC, &t1);
  printf("w/o tracing: %lf\n", intv2double(&t0, &t1));
  return 0;
}
