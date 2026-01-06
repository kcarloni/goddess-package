
This is a fork of the original GODDeSS package, from https://git.rwth-aachen.de/3pia/forge/goddess-package.

## Compiling the package:
To compile the package, I use a setup script that looks something like this:
```bash
#!/bin/bash

# --- should point to the top-level GEANT4 directory.
export GEANT4_DIR="/Users/kiara/software/csrc/geant4-v10.6.0-install"

# --- points to the top-level of this package's directory
export GODDESS="/Users/kiara/software/csrc/goddess-package"

cd "${GEANT4_DIR}/bin/"
source "geant4.sh"

# --- setup the build directory
mkdir -p "$GODDESS/build"
cd "$GODDESS/build"

# --- run cmake: 
rm -f CMakeCache.txt
cmake \
    -DGeant4_DIR="${GEANT4_DIR}/lib/Geant4-10.6.0" \
    -DBOOST_ROOT="/opt/homebrew/Cellar/boost/" \
    $GODDESS/source

# --- now make: 
# make clean
make 
# make SimExample
# make SimA
# make SimB
```
The above setup script is included in two parts as `setup_1.sh` and `setup_2.sh`. 


## Adding sub-projects:
For now, I add a new subproject by copying `SimExample` to a new directory, e.g. `SimA`, and making the following modifications:

1. modify `CMakeLists.txt` to add the new subproject:
```bash
message(STATUS "Added subproject: ${CMAKE_CURRENT_SOURCE_DIR}/SimA")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/SimA)
```

2. modify the project name in `SimA/CMakeLists.txt`, line 12:
```bash
# Set project name:
project(SimA)
```

3. modify the final run command inside of `SimA/run.sh`. 
```bash
# finally, run the program
$BUILDDIR/SimA $InitFileString
```

## Running a simulation:

To run a simulation, you can source a script of the following form:
```bash
#!/bin/bash

export GEANT4_DIR="/Users/kiara/software/csrc/geant4-v10.6.0-install"
export GODDESS="/Users/kiara/software/csrc/GODDeSS_4_3"

cd "${GEANT4_DIR}/bin/"
source "geant4.sh"

# --- variables needed by the internal `run.sh` script. 
export BUILDDIR="${GODDESS}/build/SimExample"
export SIMDIR="${GODDESS}/source/SimExample"

# export SIMDIR="${GODDESS}/source/SimA"
# export BUILDDIR="${GODDESS}/build/SimA"

source "$SIMDIR/run.sh"
```
The above script is included as `test.sh`. If your `GEANT4` package was build with visualization enabled, the example script should produce a visualization of a square scintillator tile with a teal/blue fiber looped inside it and a SiPM on the end of the tile. To then shoot a single muon at the tile, you can enter `/run/beamOn 1` in the visualizer command line. 

The subpackage run script, e.g. `source/SimExample/run.sh`, have a large number of adjustable program options. A few examples:
- Line 22: You can set `NumberOfEvents=1` to run in batch mode (without visualization).

## Modifying sub-projects:

The main way to alter a sub-project is by making changes to the detector construction, by modifying the definition of `G4VPhysicalVolume* DetectorConstruction::Construct()` inside of the `src/Preparation/DetectorConstruction.cc` file.

The example setup in `SimExample` contains three possible setups inside the `DetectorConstruction.cc` file. As a first test, comment out the default setup and swap in a different setup.