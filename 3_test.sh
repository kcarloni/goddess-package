#!/bin/bash
#
# Run the bundled SimExample: a square scintillator tile with a looped fibre
# and a SiPM on the tile end. With a Geant4 built for visualisation this opens
# the viewer, where `/run/beamOn 1` fires a single muon at the tile.
#
# Set SIMDIR/BUILDDIR to point at a different sub-project, e.g.
#   SIMDIR="$GODDESS/source/SimA" BUILDDIR="$GODDESS/build/SimA" bash 3_test.sh

source "$(dirname "${BASH_SOURCE[0]}")/setup_paths.sh" || exit 1

export SIMDIR="${SIMDIR:-$GODDESS/source/SimExample}"
export BUILDDIR="${BUILDDIR:-$GODDESS/build/SimExample}"

if [ ! -d "$BUILDDIR" ]; then
    echo "3_test: $BUILDDIR does not exist — run 1_setup.sh and 2_compile.sh first." >&2
    exit 1
fi

source "$SIMDIR/run.sh"
