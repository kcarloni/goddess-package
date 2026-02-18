#!/bin/bash

export GEANT4_INSTALL_DIR="/Users/kiara/home/software/built"
export GODDESS="/Users/kiara/home/research/tambo/g4_panel_sim/goddess-package"

cd "${GEANT4_INSTALL_DIR}/bin/"
source "geant4.sh"

export BUILDDIR="${GODDESS}/build/SimExample"
export SIMDIR="${GODDESS}/source/SimExample"

source "$SIMDIR/run.sh"