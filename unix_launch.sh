#!/bin/bash

cmake -B ./build/ -S ./
cd ./build/
make
cp ./libdistricomp.a ../lib/libdistricomp.a