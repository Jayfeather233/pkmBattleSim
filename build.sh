#!/bin/bash

directories=("./build")  # Add your directories here

for directory in "${directories[@]}"
do
    if [ ! -d "$directory" ]; then
        mkdir "$directory"
    fi
done

cd ./build
cmake -DCMAKE_BUILD_TYPE=Release .. -DMODE="$1"
make -j$(( $(nproc) > 1 ? $(nproc) - 1 : 1 ))