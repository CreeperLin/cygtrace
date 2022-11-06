import time
import libdemo
import random
import cygtrace


arr = list(range(32))
random.shuffle(arr)


t0 = time.perf_counter()
libdemo.slowsort(arr)
print('without tracing:', time.perf_counter() - t0)

# th = 10000
th = 0
ret = cygtrace.enable_export(th)
t0 = time.perf_counter()
libdemo.slowsort(arr)
print('with tracing:', time.perf_counter() - t0)
cygtrace.event_disable()
cygtrace.export_json('result_pybind11.json')
