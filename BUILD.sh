#!/bin/sh

VERSION_NUMBER=`./VERSION`

# Assume Debian qtchooser available (wrapper implementing -qt 5 option).
qmake -qt=5 "VERSION_NUMBER=$VERSION_NUMBER" main.pro && make -j 4
