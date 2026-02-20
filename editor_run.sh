#!/bin/bash

cmake -B build -DUSE_EDITOR=ON
cmake --build build --parallel 8
./build/game/Game
