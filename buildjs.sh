#!/bin/bash

echo Clean old build

rm -rf build
mkdir build
pushd build

ls

echo Generate project files

emcmake cmake .. -G "Unix Makefiles" -DWASM=YES

echo Build project

emmake make
