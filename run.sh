#!/bin/bash

read -p "editor? [Y/n]" editor

case $editor in
	n ) cmake -B build -DUSE_EDITOR=OFF;;
	* ) cmake -B build -DUSE_EDITOR=ON;;
esac

cmake --build build --parallel 8
./build/game/Game
