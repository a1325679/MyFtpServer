chcp 65001
mkdir build
mkdir file
cd build
del -rf *
cmake .. -G "MinGW Makefiles"
mingw32-make.exe