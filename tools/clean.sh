#!/bin/zsh

DIRS=( build include lib share bin )
rm -rf ../$DIRS
install -dv download $DIRS