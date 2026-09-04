#!/bin/bash
#
# Configure the GODDeSS build with CMake. Run 2_compile.sh afterwards.
#
# Requires GEANT4_INSTALL_DIR in the environment — see setup_paths.sh.

source "$(dirname "${BASH_SOURCE[0]}")/setup_paths.sh" || exit 1

mkdir -p "$GODDESS/build" && cd "$GODDESS/build"
if [ -f CMakeCache.txt ]; then rm CMakeCache.txt; fi

# Boost is discovered by find_package via the standard CMake search. Hint with
# -DBOOST_ROOT=... here if yours lives somewhere CMake will not find on its own
# (e.g. a non-default Homebrew prefix on macOS: -DBOOST_ROOT=/opt/homebrew).
cmake \
    -DGeant4_DIR="$Geant4_DIR" \
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
    "$GODDESS/source"
