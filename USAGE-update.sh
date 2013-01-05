#!/bin/sh

echo "Rebuilding built-in user documentation..."
if ! ./text_to_markup.py -html < USAGE > evolvotron.html ; then echo "Couldn't build evolvotron.html" ; fi
if ! ./text_to_markup.py -qml -s < USAGE > libevolvotron/usage_text.h ; then echo "Couldn't build libevolvotron/usage_text.h" ; fi
if ! test -s libevolvotron/usage_text.h ; then echo "\"Full built-in user documentation not available due to problem during build configuration\"" > libevolvotron/usage_text.h ; echo "Something went wrong, used built-in user documentation fallback plan"; fi
echo "...built built-in user documentation"
