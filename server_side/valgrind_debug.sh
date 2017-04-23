#!/bin/sh

valgrind -v --track-origins=yes --leak-check=full ./LibALot.run > output.txt 2>&1
