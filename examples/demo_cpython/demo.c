#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>

void scan(PyObject *obj) {
  if (PyTuple_Check(obj) || PyList_Check(obj)) {
    Py_ssize_t size = PySequence_Size(obj);
    for (Py_ssize_t i = 0; i < size; ++i) {
      PyObject *item = PySequence_GetItem(obj, i);
      scan(item);
    }
  } else if (PyDict_Check(obj)) {
    scan(PyDict_Items(obj));
  } else {
    PyObject_Print(PyObject_Type(obj), stdout, 0);
    putchar('\t');
    PyObject_Print(obj, stdout, 0);
    putchar('\n');
  }
}

PyObject *libdemo_scan(PyObject *self, PyObject *args) {
  PyObject *obj;
  if (!PyArg_ParseTuple(args, "O", &obj)) return NULL;
  scan(obj);
  return PyLong_FromLong(0);
}

static PyObject *libdemo_system(PyObject *self, PyObject *args) {
  const char *command;
  int sts;

  if (!PyArg_ParseTuple(args, "s", &command)) return NULL;
  sts = system(command);
  return PyLong_FromLong(sts);
}

static PyMethodDef libdemo_methods[] = {
  {"system", libdemo_system, METH_VARARGS, ""},
  {"scan", libdemo_scan, METH_VARARGS, ""},
  {NULL, NULL, 0, NULL}
  };

static struct PyModuleDef libdemomodule = {PyModuleDef_HEAD_INIT, "libdemo", NULL, -1, libdemo_methods};

PyMODINIT_FUNC PyInit_libdemo(void) {
  PyObject *m;

  m = PyModule_Create(&libdemomodule);
  if (m == NULL) return NULL;

  return m;
}
