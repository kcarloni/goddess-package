#!/bin/bash

export GODDESS="/Users/kiara/home/research/tambo/g4_panel_sim/goddess-package"

mkdir -p "$GODDESS/build" && cd "$GODDESS/build"
make
