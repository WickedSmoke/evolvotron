#!/bin/sh

echo "***"
echo "*** This script assumes Debian's Qt setup with a qtchooser supporting the -qt=5 option."
echo "*** On Debian Squeeze, change to use -qt=4 instead."
echo "*** Other Qt setups may require removing the -qt=5 option from qmake and setting QTDIR and PATH as usual."
echo "***"

VERSION_NUMBER=`./VERSION`

qmake -qt=5 "VERSION_NUMBER=$VERSION_NUMBER" main.pro && make -j 4
