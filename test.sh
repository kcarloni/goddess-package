#!/bin/bash

export GEANT4_DIR="/Users/kiara/software/csrc/geant4-v10.6.0-install"
export GODDESS="/Users/kiara/software/csrc/GODDeSS_4_3"

cd "${GEANT4_DIR}/bin/"
source "geant4.sh"

export BUILDDIR="${GODDESS}/build/SimExample"
export SIMDIR="${GODDESS}/source/SimExample"

# export SIMDIR="${GODDESS}/source/SimA"
# export BUILDDIR="${GODDESS}/build/SimA"

# export SIMDIR="${GODDESS}/source/SimB"
# export BUILDDIR="${GODDESS}/build/SimB"

source "$SIMDIR/run.sh"