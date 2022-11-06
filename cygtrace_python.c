#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "cygtrace.h"
#include "cygtrace_export.h"

#define ELEVENTH_ARGUMENT(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, ...) a11
#define COUNT_ARGUMENTS(...) ELEVENTH_ARGUMENT(dummy, ##__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

#define CAT(a, ...) PRIMITIVE_CAT(a, __VA_ARGS__)
#define PRIMITIVE_CAT(a, ...) a##__VA_ARGS__

#define GET_N(fn, ...) CAT(fn##_, COUNT_ARGUMENTS(__VA_ARGS__))
#define CALL_N(fn, ...) GET_N(fn, __VA_ARGS__)(__VA_ARGS__)

#define FMT_str s
#define FMT_int i
#define FMT_long l

#define TYPE_str const char *
#define TYPE_int int
#define TYPE_long long

#define GEN_FMT_0() ""
#define GEN_FMT_1(T) MACRO_STRINGIFY(FMT_##T)
#define GEN_FMT_2(T1, T2) GEN_FMT_1(T1) GEN_FMT_1(T2)
#define GEN_FMT_3(T1, T2, T3) GEN_FMT_1(T1) GEN_FMT_2(T2, T3)

#define GEN_TYPE(T, x) TYPE_##T a##x;
#define GEN_TYPE_0()
#define GEN_TYPE_1(T0) GEN_TYPE(T0, 1)
#define GEN_TYPE_2(T1, T2) GEN_TYPE(T1, 2) GEN_TYPE_1(T2)
#define GEN_TYPE_3(T1, T2, T3) GEN_TYPE(T1, 3) GEN_TYPE_2(T2, T3)

#define GEN_RARGS_0()
#define GEN_RARGS_1() , &a1
#define GEN_RARGS_2() , &a2 GEN_RARGS_1()
#define GEN_RARGS_3() , &a3 GEN_RARGS_2()

#define GEN_ARGS_0()
#define GEN_ARGS_1() a1
#define GEN_ARGS_2() a2, GEN_ARGS_1()
#define GEN_ARGS_3() a3, GEN_ARGS_2()

#define GEN_RET_void  \
  Py_INCREF(Py_None); \
  return Py_None;

#define GEN_RET_int return PyLong_FromLong(ret);

#define GEN_PYC_FUNC_DEF(fn) static PyObject *gen_pyc_##fn(PyObject *self, PyObject *args)

#define GEN_PYC_FUNC_PRE(...)                     \
  CALL_N(GEN_TYPE, __VA_ARGS__)                   \
  const char *fmt = CALL_N(GEN_FMT, __VA_ARGS__); \
  if (*fmt != '\0' && !PyArg_ParseTuple(args, fmt GET_N(GEN_RARGS, __VA_ARGS__)())) return NULL;

#define GEN_PYC_FUNC(T_ret, fn, ...)                \
  GEN_PYC_FUNC_DEF(fn) {                            \
    GEN_PYC_FUNC_PRE(__VA_ARGS__)                   \
    T_ret ret = fn(GET_N(GEN_ARGS, __VA_ARGS__)()); \
    CAT(GEN_RET_, T_ret)                            \
  }

#define GEN_PYC_FUNC_VOID(fn, ...)      \
  GEN_PYC_FUNC_DEF(fn) {                \
    GEN_PYC_FUNC_PRE(__VA_ARGS__)       \
    fn(GET_N(GEN_ARGS, __VA_ARGS__)()); \
    GEN_RET_void                        \
  }

#define BIND_CALLBACK_SETTER 0

GEN_PYC_FUNC_VOID(cygtrace_enable)
GEN_PYC_FUNC_VOID(cygtrace_disable)
GEN_PYC_FUNC(int, cygtrace_is_enabled)
GEN_PYC_FUNC_VOID(cygtrace_event_enable)
GEN_PYC_FUNC_VOID(cygtrace_event_disable)
GEN_PYC_FUNC(int, cygtrace_event_is_enabled)
GEN_PYC_FUNC(int, cygtrace_is_available)
GEN_PYC_FUNC_VOID(cygtrace_unset_callback_enter)
GEN_PYC_FUNC_VOID(cygtrace_unset_callback_exit)
GEN_PYC_FUNC_VOID(cygtrace_event_set_threshold_ns, long)
GEN_PYC_FUNC_VOID(cygtrace_event_unset_callback)
GEN_PYC_FUNC(int, cygtrace_enable_export, long)
GEN_PYC_FUNC(int, cygtrace_export_json, str)

#if BIND_CALLBACK_SETTER
GEN_PYC_FUNC(int, cygtrace_set_callback_enter)
GEN_PYC_FUNC(int, cygtrace_set_callback_exit)
GEN_PYC_FUNC(int, cygtrace_event_set_callback)
#endif

static PyMethodDef cygtrace_methods[] = {
    {"enable", gen_pyc_cygtrace_enable, METH_VARARGS, ""},
    {"disable", gen_pyc_cygtrace_disable, METH_VARARGS, ""},
    {"is_enabled", gen_pyc_cygtrace_is_enabled, METH_VARARGS, ""},
    {"event_enable", gen_pyc_cygtrace_event_enable, METH_VARARGS, ""},
    {"event_disable", gen_pyc_cygtrace_event_disable, METH_VARARGS, ""},
    {"event_is_enabled", gen_pyc_cygtrace_event_is_enabled, METH_VARARGS, ""},
    {"is_available", gen_pyc_cygtrace_is_available, METH_VARARGS, ""},
    {"unset_callback_enter", gen_pyc_cygtrace_unset_callback_enter, METH_VARARGS, ""},
    {"unset_callback_exit", gen_pyc_cygtrace_unset_callback_exit, METH_VARARGS, ""},
    {"event_set_threshold_ns", gen_pyc_cygtrace_event_set_threshold_ns, METH_VARARGS, ""},
    {"event_unset_callback", gen_pyc_cygtrace_event_unset_callback, METH_VARARGS, ""},
    {"enable_export", gen_pyc_cygtrace_enable_export, METH_VARARGS, ""},
    {"export_json", gen_pyc_cygtrace_export_json, METH_VARARGS, ""},
#if BIND_CALLBACK_SETTER
    {"set_callback_enter", gen_pyc_cygtrace_set_callback_enter, METH_VARARGS, ""},
    {"set_callback_exit", gen_pyc_cygtrace_set_callback_exit, METH_VARARGS, ""},
    {"event_set_callback", gen_pyc_cygtrace_event_set_callback, METH_VARARGS, ""},
#endif
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef cygtracemodule = {PyModuleDef_HEAD_INIT, "cygtrace", NULL, -1, cygtrace_methods};

PyMODINIT_FUNC PyInit_cygtrace(void) {
  PyObject *m;

  m = PyModule_Create(&cygtracemodule);
  if (m == NULL) return NULL;

  return m;
}
