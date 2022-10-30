# cygtrace

gcc instrumentation based tracer for c/c++/pybind11

## Installation

Requirements:

- cmake
- pybind11 (optional)

To build the library:

```bash
mkdir build
cd build
cmake ..
make
sudo make install # to install the library in /usr/local/lib
```

To build the examples:

```bash
cd examples
mkdir build
cd build
cmake ..
make
```

## Notes

- CMake could not find pybind11

```bash
pip3 install "pybind11[global]"
```

- "error while loading shared libraries: libcygtrace.so: cannot open shared object file: No such file or directory"

```bash
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
```

- To view JSON-formatted profiling results, go to ```chrome://tracing``` or [Perfetto UI](https://ui.perfetto.dev/)
