#!/bin/bash
#
# Build GODDeSS. Run 1_setup.sh first.
#
# Note that the default target also builds the doxygen documentation; pass a
# specific target (e.g. `bash 2_compile.sh SimExample`) to skip it.

source "$(dirname "${BASH_SOURCE[0]}")/setup_paths.sh" || exit 1

if [ ! -d "$GODDESS/build" ]; then
    echo "2_compile: $GODDESS/build does not exist — run 1_setup.sh first." >&2
    exit 1
fi

cd "$GODDESS/build"
make "$@"
