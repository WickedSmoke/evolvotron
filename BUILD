#!/bin/sh

export PATH=$QTDIR/bin:$PATH

# Can't parallelize make at top level... doesn't handle dependencies.
# OK in leaves though.
./configure && make MAKE="make -j 8"

