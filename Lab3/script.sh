#!/bin/sh
make clean
make -f Makefile.linux
size=$1
shadow=$2
reflection=$3
refraction=$4
depthfield=$5
#./rt sphere3t.iv sphere3t.ppm $size $size $shadow $refraction $refraction $depthfield
./rt refraction1.iv refraction1.ppm $size $size $shadow $reflection $refraction $depthfield
./rt refraction2.iv refraction2.ppm $size $size $shadow $reflection $refraction $depthfield
./rt refraction3.iv refraction3.ppm $size $size $shadow $reflection $refraction $depthfield

./rt cubesphere3.iv cubesphere3.ppm $size $size $shadow $reflection $refraction $depthfield
./rt cubesphere4.iv cubesphere4.ppm $size $size $shadow $reflection $refraction $depthfield
./rt cubesphere5.iv cubesphere5.ppm $size $size $shadow $reflection $refraction $depthfield
./rt sphereCycloid.iv sphereCycloid.ppm $size $size $shadow $reflection $refraction $depthfield

#./rt cube1.iv cube1.ppm 500 500 $shadow $reflection $refraction
#./rt cubesphere.iv cubesphere.ppm 500 500 1 0 $shadow $reflection $refraction

