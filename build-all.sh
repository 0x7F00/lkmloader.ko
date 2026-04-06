#!/usr/bin/bash

ALL_VER=$(ls /opt/ddk/src)

for ver in $ALL_VER; do
    echo "building for ver $ver"
    export VER=$ver
    echo "make" | ddk shell $ver
    if [ $? -ne 0 ]; then
        echo "!!! failed to build for $ver"
        exit 1
    fi
done

echo "build all done!"
