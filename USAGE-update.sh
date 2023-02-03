#!/bin/bash

echo "Rebuilding built-in user documentation..."
HTML=evolvotron.html
QTDOC=libevolvotron/usage_text.h

if command -v boron &> /dev/null ; then
	./text_to_markup.b -html USAGE >$HTML || echo "Couldn't build $HTML"
	./text_to_markup.b -qml -s USAGE >$QTDOC || echo "Couldn't build $QTDOC"
else
	./text_to_markup.py -html <USAGE >$HTML || echo "Couldn't build $HTML"
	./text_to_markup.py -qml -s <USAGE >$QTDOC || echo "Couldn't build $QTDOC"
fi

if ! test -s $QTDOC ; then
	echo "\"Full built-in user documentation not available due to problem during build configuration\"" > $QTDOC
	echo "Something went wrong, used built-in user documentation fallback plan"
fi
echo "...built built-in user documentation"
