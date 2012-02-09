#!/bin/sh
make clean
make -f Makefile.linux
#./rt sphere1.iv sphere1.ppm 500 500 
./rt ssphere3t.iv sphere3t.ppm 500 500 
./rt mickeyspheres.iv mickeyspheres.ppm 100 100
./rt sphereCycloid.iv sphereCycloid.ppm 100 100
#./rt sphereTower.iv sphereTower.ppm 500 500 
