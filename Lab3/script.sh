#!/bin/sh
make clean
make -f Makefile.linux
#./rt sphere1.iv sphere1.ppm 500 500 
#./rt sphere3t.iv sphere3t.ppm 500 500 
#./rt sphereCycloid.iv sphereCycloid.ppm 500 500


#./rt refraction1.iv refraction1.ppm 500 500 1 1 > output.txt |tail -f output.txt
#./rt refraction1.iv refraction1.ppm 500 500 1 1
#./rt refraction2.iv refraction2.ppm 500 500 1 1
#./rt refraction3.iv refraction3.ppm 500 500 1 1

shadow=$1
reflection=$2
refraction=$3
./rt refraction1.iv refraction1.ppm 500 500 $shadow $reflection $refraction 
./rt refraction1.iv refraction1.ppm 500 500 $shadow $reflection $refraction
./rt refraction2.iv refraction2.ppm 500 500 $shadow $reflection $refraction
./rt refraction3.iv refraction3.ppm 500 500 $shadow $reflection $refraction

./rt cubesphere3.iv cubesphere3.ppm 500 500 $shadow $reflection $refraction
./rt cubesphere4.iv cubesphere4.ppm 500 500 $shadow $reflection $refraction
./rt cubesphere5.iv cubesphere5.ppm 500 500 $shadow $reflection $refraction
#./rt cube1.iv cube1.ppm 500 500 1 0
#./rt cubesphere.iv cubesphere.ppm 500 500 1 0

