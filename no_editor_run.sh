#!/bin/bash

cmake -B build -DUSE_EDITOR=OFF
cmake --build build --parallel 8
./build/game/Game
