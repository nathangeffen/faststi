#!/bin/sh
export FSTI_DATA=../data
valgrind --error-exitcode=1 --leak-check=full ./faststi -t

