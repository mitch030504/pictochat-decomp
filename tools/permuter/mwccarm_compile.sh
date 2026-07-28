#!/bin/bash
# decomp-permuter compile wrapper for our mwccarm toolchain (native Windows, no wine).
# The permuter invokes this as:  mwccarm_compile.sh <C_FILE> -o <O_FILE>
#
# Flags are per-function (arm946e vs arm7tdmi proc, -thumb vs not - this
# project mixes both, unlike a single-CPU game), so tools/permuter/import_func.py
# writes a `flags.txt` sidecar next to base.c in each permuter workdir; this
# wrapper just reads it. A source whose first line is "//cpp" is C++
# (matches tools/templates.py's //cpp convention); compile it with the C++
# frontend regardless of what flags.txt says for -lang.
set -e
REPO="$(cd "$(dirname "$0")/../.." && pwd)"
CC="$REPO/tools/mwccarm/dsi/1.3/mwccarm.exe"
export LM_LICENSE_FILE="$REPO/tools/mwccarm/license.dat"
C_FILE="$1"   # $2 is "-o", $3 is the output object
O_FILE="$3"
DIR="$(dirname "$C_FILE")"

FLAGS="-O4,p -enum int -lang c99 -char signed -interworking -thumb -proc arm946e -gccext,on -msgstyle gcc"
if [ -f "$DIR/flags.txt" ]; then
    FLAGS="$(cat "$DIR/flags.txt")"
fi
if head -1 "$C_FILE" | grep -q '^//cpp'; then
    FLAGS="$(echo "$FLAGS" | sed 's/-lang c99/-lang c++/')"
fi
"$CC" $FLAGS -c -o "$O_FILE" "$C_FILE"
