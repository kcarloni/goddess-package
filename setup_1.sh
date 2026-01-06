#!/bin/bash

export GEANT4_DIR="/Users/kiara/software/csrc/geant4-v10.6.0-install"
export GODDESS="/Users/kiara/software/csrc/GODDeSS_4_3"

cd "${GEANT4_DIR}/bin/"
source "geant4.sh"

cd "$GODDESS/build"

rm CMakeCache.txt
cmake \
    -DGeant4_DIR="${GEANT4_DIR}/lib/Geant4-10.6.0" \
    -DBOOST_ROOT="/opt/homebrew/Cellar/boost/" \
    $GODDESS/source