#!/bin/sh

if ! [ -x "$(command -v valgrind)" ]; then
	echo 'Valgrind not installed.' >&2
	exit 1
fi
export FSTI_DATA=../data
valgrind --error-exitcode=1 --leak-check=full ./faststi -t

