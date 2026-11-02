#!/bin/bash

CURDIR=$(dirname $0)
echo $CURDIR

g++ "$CURDIR/"*.cpp $(allegro-config --libs) -O3 -o "$CURDIR/palavras"