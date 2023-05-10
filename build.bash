#!/bin/bash
#Linux platform build project and run this file
mkdir build
mkdir file
cd build
rm -rf *
cmake .. && make
