#!/bin/bash

export GEANT4_INSTALL_DIR="/Users/kiara/home/software/built"
export GODDESS="/Users/kiara/home/research/tambo/g4_panel_sim/goddess-package"

cd "${GEANT4_INSTALL_DIR}/bin/"
source "geant4.sh"

mkdir -p "$GODDESS/build" && cd "$GODDESS/build"
if [ -f CMakeCache.txt ]; then rm CMakeCache.txt; fi
cmake \
    -DGeant4_DIR="${GEANT4_INSTALL_DIR}/lib/Geant4-10.6.0" \
    -DBOOST_ROOT="/opt/homebrew/Cellar/boost/" \
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
    "$GODDESS/source"
