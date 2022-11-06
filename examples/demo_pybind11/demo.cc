#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

void _slowsort(std::vector<int> &v, int i, int j) {
  if (i >= j) return;
  int m = (i + j) / 2;
  _slowsort(v, i, m);
  _slowsort(v, m + 1, j);
  if (v[j] < v[m]) {
    int t = v[j];
    v[j] = v[m];
    v[m] = t;
  }
  _slowsort(v, i, j - 1);
}

void slowsort(std::vector<int> &v) { _slowsort(v, 0, v.size()); }

PYBIND11_MODULE(libdemo, m) {
  m.def("slowsort", [](std::vector<int> &v) { slowsort(v); });
}
