#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
// #include <pybind11/stl.h>

#include <string>

#include "cygtrace.h"
#include "cygtrace_export.h"

#define BIND_CALLBACK_SETTER 0

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

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
  m.def("event_enable", &cygtrace_event_enable);
  m.def("event_disable", &cygtrace_event_disable);
  m.def("event_is_enabled", &cygtrace_event_is_enabled);
  m.def("is_available", &cygtrace_is_available);
  m.def("unset_callback_enter", &cygtrace_unset_callback_enter);
  m.def("unset_callback_exit", &cygtrace_unset_callback_exit);
  m.def("event_set_threshold_ns", &cygtrace_event_set_threshold_ns);
  m.def("event_unset_callback", &cygtrace_event_unset_callback);

#if BIND_CALLBACK_SETTER
  m.def("set_callback_enter",
        [](std::function<void(CYGTRACE_CALLBACK_SIG)> const &cb_enter) { cygtrace_set_callback_enter(cb_enter); });
  m.def("set_callback_exit",
        [](std::function<void(CYGTRACE_CALLBACK_SIG)> const &cb_exit) { cygtrace_set_callback_exit(cb_exit); });
  m.def("event_set_callback",
        [](std::function<void(CYGTRACE_EV_CALLBACK_SIG)> const &cb_event) { cygtrace_event_set_callback(cb_event); });
#endif

  m.def("enable_export", &cygtrace_enable_export);
  m.def("export_json", [](const std::string &s) { return cygtrace_export_json(s); });

#ifdef VERSION_INFO
  m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
  m.attr("__version__") = "dev";
#endif
}
