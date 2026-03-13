#!/bin/bash

read -p "editor? [Y/n]" editor

case $editor in
	n ) cmake -B build -DUSE_EDITOR=OFF -DCMAKE_BUILD_TYPE=Debug;;
	* ) cmake -B build -DUSE_EDITOR=ON -DCMAKE_BUILD_TYPE=Debug;;
esac

cmake --build build --parallel 8
