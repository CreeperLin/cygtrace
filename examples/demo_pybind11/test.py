import time
import libdemo
import random


arr = list(range(32))
random.shuffle(arr)


t0 = time.perf_counter()
libdemo.slowsort(arr)
print('without tracing:', time.perf_counter() - t0)

# th = 10000
th = 0
ret = libdemo.enable_trace(th)
t0 = time.perf_counter()
libdemo.slowsort(arr)
print('with tracing:', time.perf_counter() - t0)
libdemo.disable_trace()
libdemo.export_trace("result_pybind11.json")
