#!/bin/sh
make clean
make -f Makefile.linux
#./rt sphere1.iv sphere1.ppm 500 500 
#./rt sphere3t.iv sphere3t.ppm 500 500 
./rt sphere3s.iv sphere3s.ppm 500 500 
./rt mickeyspheres.iv mickeyspheres.ppm 500 500
#./rt sphereCycloid.iv sphereCycloid.ppm 500 500
./rt sphereCycloidLit.iv sphereCycloidLit.ppm 500 500
./rt sphereTowerLit.iv sphereTowerLit.ppm 500 500 
