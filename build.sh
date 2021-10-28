#! /bin/bash

# Raylib
cd "third_party/raylib" || exit
mkdir build
cd build || exit
cmake -DBUILD_SHARED_LIBS=ON ..
make -j8
sudo make install
cd "../../.."

# fmt
cd "third_party/fmt" || exit
mkdir build
cd build || exit
cmake -DFMT_TEST=OFF ..
make -j8
sudo make install
cd "../../.."

# Build shooter

if [ ! -d build ]
then
    mkdir build
fi

mkdir build
cd build || exit
cmake ..
make -j8
cp "compile_commands.json" ..
