#!/bin/bash

cmake -G 2>&1 >/dev/null | grep -wq "$1" || {
    echo "Generator '$1' unsupported."
    exit 1
}

[[ -z "$1" ]] || {
    GENERATOR="-G$1"
}

[ -d "build" ] && {
    echo "Build directory already exists"
    exit 1
}

for BUILD_TYPE in Debug Release Sanitize; do
    BUILD_PATH=$(echo "build/$BUILD_TYPE" | tr '[:upper:]' '[:lower:]')

    cmake \
        $GENERATOR \
        -H. -B$BUILD_PATH \
        -DCMAKE_BUILD_TYPE=$BUILD_TYPE
done
