# GODDeSS

A Geant4 extension providing object classes for scintillator tiles, optical
fibres, wrappings and photon detectors, together with a library of measured
material properties.

This is a **fork** of the original GODDeSS package by Erik Dietz-Laursonn
(RWTH Aachen), carrying fixes that are not upstream. See
[`ATTRIBUTION.md`](ATTRIBUTION.md) for the full provenance — what is upstream,
what was inherited from elsewhere, and what was changed here — plus the licence
terms (CC BY-NC-SA 3.0).

Upstream is unmaintained and distributes release tarballs rather than a source
tree, so the fixes here will not be merged back.

## Building

You need Geant4 (developed against 10.6), CMake, Boost and doxygen. Point at
your Geant4 install — the directory containing `bin/geant4.sh` — and run the
two build scripts:

```bash
export GEANT4_INSTALL_DIR=/path/to/your/geant4-install

bash 1_setup.sh      # cmake
bash 2_compile.sh    # make
```

Both scripts source [`setup_paths.sh`](setup_paths.sh), which derives the
package root from its own location and locates `Geant4Config.cmake` under your
install regardless of version or `lib`/`lib64` layout. Nothing in this
repository needs editing to build it.

Two things that may need a hint on your system:

- **Boost**, if CMake cannot find it on its own. Add `-DBOOST_ROOT=...` to the
  `cmake` call in `1_setup.sh` — on macOS with Homebrew that is usually
  `-DBOOST_ROOT=/opt/homebrew`.
- **doxygen**, which the default `make` target uses to build the documentation
  into `documentation/`. To skip it, build a specific target instead:
  `bash 2_compile.sh SimExample`.

## Running the example

```bash
bash 3_test.sh
```

`SimExample` is a square scintillator tile with a teal/blue fibre looped inside
it and a SiPM on one end. If your Geant4 was built with visualisation enabled
this opens the viewer; entering `/run/beamOn 1` at its command line fires a
single muon at the tile.

The sub-project's own run script, `source/SimExample/run.sh`, carries a large
number of adjustable options near the top of the file. For example, line 22
(`NumberOfEvents`) set to a positive value runs in batch mode without
visualisation, and line 33 (`TileDimensions`) changes the tile geometry.

To run a different sub-project, point `SIMDIR` and `BUILDDIR` at it:

```bash
SIMDIR="$PWD/source/SimA" BUILDDIR="$PWD/build/SimA" bash 3_test.sh
```

## Adding a sub-project

Copy `source/SimExample` to a new directory, e.g. `source/SimA`, then:

1. Register it in `source/CMakeLists.txt`:
   ```cmake
   message(STATUS "Added subproject: ${CMAKE_CURRENT_SOURCE_DIR}/SimA")
   add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/SimA)
   ```
2. Change the project name in `SimA/CMakeLists.txt` (line 12) to `project(SimA)`.
3. In `SimA/run.sh`, set the output directory (the commented
   `OutputDirectory` near line 89) and update the final run command to launch
   `$BUILDDIR/SimA`.

Sub-projects matching `source/Sim_*` are gitignored, so scratch work does not
end up in version control.

## Modifying a sub-project

The main lever is the detector construction: `G4VPhysicalVolume*
DetectorConstruction::Construct()` in the sub-project's
`src/Preparation/DetectorConstruction.cc`. `SimExample` ships three alternative
setups in that file — commenting out the default and swapping in another is a
good first experiment.

Changes to a sub-project's source require rebuilding that target, e.g.
`bash 2_compile.sh SimA`.

## Use as part of G4ScintKit

This package is also a submodule of
[G4ScintKit](https://github.com/kcarloni/G4ScintKit), which builds it in-tree as
a CMake subdirectory and drives it through its own generic Geant4 application
rather than through a hand-written sub-project. In that setting G4ScintKit's
`bash_scripts/` handle the build, and the scripts here are not used.
