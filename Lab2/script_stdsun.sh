#!/bin/sh
make clean
make 
./rt sphere1.iv sphere1.ppm 500 500 
./rt sphere3.iv sphere3.ppm 500 500 
./rt mickeyspheres.iv mickeyspheres.ppm 500 500 
./rt sphereTower.iv sphereTower.ppm 500 500 
