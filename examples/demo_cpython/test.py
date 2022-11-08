import time
import libdemo
import cygtrace


obj = [
    [2, 3, 5, 7, 11],
    {'a': 13},
    137,
    '42',
]

# th = 10000
th = 0
ret = cygtrace.enable_export(th)
t0 = time.perf_counter()
libdemo.scan(obj)
libdemo.system('ls -l')
print('with tracing:', time.perf_counter() - t0)
cygtrace.event_disable()
cygtrace.export_json('result_cpython.json')
