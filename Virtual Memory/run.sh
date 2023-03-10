#!/bin/bash

set -e -u -o pipefail


# Run valgrind to produce the trace
valgrind --tool=lackey --trace-mem=yes "./${1}" ${@:2} >& "tmp"

# Trim the trace so that it only includes the algorithm part of the program
mkdir -p "traces"
./trimtrace.py "${1}.marker" "tmp" > "traces/addr-${1}.ref"
rm -f "tmp"

# Reduce the trace size to page level accesses
#./fastslim.py --keepcode --buffersize 8 < "traces/addr-${1}.ref" > "traces/page-${1}.ref"

# Reduce the trace size of page level accesses keeping the page offset. Then
# transform the trace to reduce the range of the page offset and generate
# values for the store/modify and load/ifetch operations.
./fastslim-with-offset.py --keepcode --buffersize 8 < "traces/addr-${1}.ref" | ./simify-trace.py --simpagesize 16 > "traces/simvaddr-${1}.ref"
