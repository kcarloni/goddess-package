
This is a fork of the original GODDeSS package, from https://git.rwth-aachen.de/3pia/forge/goddess-package.


### Adding sub-projects:
For now, I add a new subproject by copying `SimExample` to a new directory, e.g. `SimA`, and making the following modifications:

1. modify `CMakeLists.txt` to add the new subproject:
```cpp
message(STATUS "Added subproject: ${CMAKE_CURRENT_SOURCE_DIR}/SimA")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/SimA)
```

2. modify the project name in `SimA/CMakeLists.txt`, line 12:
```cpp
# Set project name:
project(SimA)
```

3. modify the final run command inside of `SimA/run.sh`. 
```cpp
# finally, run the program
$BUILDDIR/SimA $InitFileString
```

### Compiling the package:
To compile the package, I use a setup script that looks something like this:
```bash
#!/bin/bash

# should point to the top-level GEANT4 directory.
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

cd "$GODDESS/build"

# make clean
# make 

# make SimExample
# make SimA
make SimB
```