#!/bin/bash

FSTI_BASE=`dirname "$0"`

echo $*

if [ "$#" -eq 0 ]; then
    echo "Usage:"
    echo " To create a faststi project:"
    echo "   $0 create <directory>"
    exit 0
fi

if [ "$1" == "create" ]; then
    if [ "$#" -ne 2 ]; then
        echo "Directory name missing"
        echo "Usage: S0 create <directory>"
        exit 1
    fi
    exit 0
fi

if [ "$1" == "release" ]; then
    COMPDIR=$FSTI_BASE/release/
    EXE=$FSTI_BASE/release/src/faststi
    echo Compiling and running release version
else
    COMPDIR=$FSTI_BASE/debug/
    if [ "$1" == "valgrind" ]; then
        VALGRIND=`which valgrind`
        EXE="$VALGRIND --leak-check=full $FSTI_BASE/debug/src/faststi"
    else
        EXE=$FSTI_BASE/debug/src/faststi
    fi
    echo Compiling and running debug version
fi

CURRENT_DIR=`pwd`

if ! [ -d "$COMPDIR" ]; then
    cd "$FSTI_BASE"
    if [ "$1" == "release" ]; then
        meson --buildtype release release
    else
        meson debug
    fi
fi

cd "$COMPDIR"
ninja -v
cd "$CURRENT_DIR"

if [ -z ${FSTI_DATA+x} ]; then
    FSTI_DATA="$FSTI_BASE/data"
    echo Setting FSTI_DATA to "$FSTI_DATA"
    FSTI_DATA="$FSTI_DATA" $EXE $*
else
    echo FSTI_DATA is "$FSTI_DATA"
    $EXE $*
fi
