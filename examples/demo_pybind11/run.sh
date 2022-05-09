#!/bin/sh
LD_VERBOSE=1 LD_PRELOAD=/usr/local/lib/libcygtrace.so python3 $(dirname $0)/test.py
