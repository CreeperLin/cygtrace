# cygtrace

gcc instrumentation based tracer for c/c++/pybind11

## Installation

Requirements:

- gcc
- cmake
- python3 with pybind11 (optional)

### Python package

```bash
pip install .
```

### System-wise library

To build/install the library:

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

## Usage

### With Python script

Run program/script/command with tracer

```bash
cygtrace ./path/to/executable
cygtrace python3 ./path/to/script.py
```

Demangle json trace file

```bash
cygtrace -d -f trace.json
```

Get compiling parameters (gcc)

```bash
gcc example.c $(cygtrace -I -L -p)
```

If compiling dynamically loaded library (e.g., pybind11), add -m flag (disables libcygtrace linking)

```bash
g++ example.cpp $(cygtrace -I -L -p -m)
```

### Manual instructions

To enable tracing in pybind11 code, add these to the compiler args (gcc):

```bash
-finstrument-functions -Wl,--export-dynamic
```

for c/cpp programs, also link the cygtrace library:

```bash
-finstrument-functions -Wl,--export-dynamic -lcygtrace
```

To enable tracing in python (pybind11), start the python interpreter like this (assuming library installed in /usr/local/lib):

```bash
LD_PRELOAD=/usr/local/lib/libcygtrace.so python3 xxx.py
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
