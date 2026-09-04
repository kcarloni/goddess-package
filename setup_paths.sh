#!/bin/bash
#
# GODDeSS environment setup.
#
# Sourced by 1_setup.sh, 2_compile.sh and 3_test.sh. It derives the package
# root from its own location and reads your Geant4 install from the
# environment, so nothing in this repository needs editing to build it.
#
# Set GEANT4_INSTALL_DIR to the directory containing bin/geant4.sh:
#
#     export GEANT4_INSTALL_DIR=/path/to/your/geant4-install
#
# When this package is built as a submodule of G4ScintKit, its own
# bash_scripts/setup_paths.sh does all of this instead and these scripts are
# not used.

# Package root, derived from this script's own location.
#
# Locating "this file" is shell-dependent: ${BASH_SOURCE[0]} is bash-only and
# is empty under zsh (the default shell on macOS), where dirname "" yields "."
# and the root would silently resolve one directory too high. zsh's equivalent
# is %x, whose syntax bash cannot even parse, so it is reached through eval.
# Set GODDESS yourself before sourcing to bypass all of this.
if [ -z "${GODDESS:-}" ]; then
    if [ -n "${BASH_VERSION:-}" ]; then
        _gd_self="${BASH_SOURCE[0]}"
    elif [ -n "${ZSH_VERSION:-}" ]; then
        _gd_self="$(eval 'printf %s "${(%):-%x}"')"
    else
        _gd_self="$0"
    fi
    GODDESS="$(cd "$(dirname "$_gd_self")" && pwd)"
    unset _gd_self
fi

if [ ! -d "$GODDESS/source" ]; then
    echo "setup_paths: '$GODDESS' does not look like the GODDeSS package root" >&2
    echo "  (expected source/ inside it)." >&2
    echo "  Export GODDESS=/path/to/goddess-package and source this file again." >&2
    return 1 2>/dev/null || exit 1
fi
export GODDESS

if [ -z "${GEANT4_INSTALL_DIR:-}" ]; then
    echo "setup_paths: GEANT4_INSTALL_DIR is not set." >&2
    echo "  Export the directory containing bin/geant4.sh, e.g." >&2
    echo "    export GEANT4_INSTALL_DIR=/path/to/your/geant4-install" >&2
    return 1 2>/dev/null || exit 1
fi
if [ ! -f "$GEANT4_INSTALL_DIR/bin/geant4.sh" ]; then
    echo "setup_paths: $GEANT4_INSTALL_DIR/bin/geant4.sh not found." >&2
    echo "  GEANT4_INSTALL_DIR should be the Geant4 *install* directory." >&2
    return 1 2>/dev/null || exit 1
fi
export GEANT4_INSTALL_DIR

# geant4.sh expects to be sourced from its own directory.
_gd_orig="$(pwd)"
cd "$GEANT4_INSTALL_DIR/bin"
source "geant4.sh"
cd "$_gd_orig"
unset _gd_orig

# Locate the Geant4 CMake config so find_package(Geant4) works regardless of
# version or lib/lib64 layout. Covers both the old (lib*/Geant4-<version>/) and
# new (lib*/cmake/Geant4/) install layouts. Sourcing geant4.sh does not export
# Geant4_DIR, so resolve it explicitly. Override by setting Geant4_DIR before
# sourcing this file.
# Located with `find` rather than a lib*/ glob: an unmatched glob expands to
# itself in bash but is a fatal "no matches found" in zsh, which would abort
# this file the moment one of the two layouts is absent.
if [ -z "${Geant4_DIR:-}" ]; then
    _gd_cfg="$(find "$GEANT4_INSTALL_DIR" -maxdepth 4 -name Geant4Config.cmake -print 2>/dev/null | head -1)"
    [ -n "$_gd_cfg" ] && Geant4_DIR="$(dirname "$_gd_cfg")"
    unset _gd_cfg
fi
if [ -z "${Geant4_DIR:-}" ]; then
    echo "setup_paths: could not find Geant4Config.cmake under $GEANT4_INSTALL_DIR" >&2
    echo "  (looked for lib*/cmake/Geant4 and lib*/Geant4-*). Set Geant4_DIR manually." >&2
    return 1 2>/dev/null || exit 1
fi
export Geant4_DIR
