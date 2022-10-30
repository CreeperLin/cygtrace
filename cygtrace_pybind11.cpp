#include <cygtrace.h>
#include <cygtrace_export.h>
#include <pybind11/pybind11.h>
// #include <pybind11/stl.h>

#include <string>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

int enable_trace(long threshold) {
  if (!cygtrace_is_available()) {
    fputs("cygtrace not available", stderr);
    return 1;
  }
  cygtrace_event_set_threshold_ns(threshold);
  cygtrace_event_set_callback(cygtrace_callback_export);
  cygtrace_event_enable();
  return 0;
}

int export_trace(std::string filename) { return cygtrace_export_json(filename.c_str()); }

PYBIND11_MODULE(cygtrace, m) {
  m.doc() = R"pbdoc(
        cygtrace python library
        -----------------------
        .. currentmodule:: cygtrace
        .. autosummary::
           :toctree: _generate
    )pbdoc";
  m.def("enable", &cygtrace_enable);
  m.def("disable", &cygtrace_disable);
  m.def("is_enabled", &cygtrace_is_enabled);
  m.def("set_callback_enter", &cygtrace_set_callback_enter);
  m.def("set_callback_exit", &cygtrace_set_callback_exit);
  m.def("event_enable", &cygtrace_event_enable);
  m.def("event_disable", &cygtrace_event_disable);
  m.def("event_is_enabled", &cygtrace_event_is_enabled);
  m.def("is_available", &cygtrace_is_available);
  m.def("unset_callback_enter", &cygtrace_unset_callback_enter);
  m.def("unset_callback_exit", &cygtrace_unset_callback_exit);
  m.def("event_set_threshold_ns", &cygtrace_event_set_threshold_ns);
  m.def("event_set_callback", &cygtrace_event_set_callback);
  m.def("event_unset_callback", &cygtrace_event_unset_callback);

  m.def("enable_trace", &enable_trace);
  m.def("disable_trace", &cygtrace_event_disable);
  m.def("export_trace", &export_trace);

#ifdef VERSION_INFO
  m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
  m.attr("__version__") = "dev";
#endif
}
