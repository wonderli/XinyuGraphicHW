#!/bin/sh
make -f Makefile.mac clean
make -f Makefile.mac
size=$1
shadow=$2
reflection=$3
refraction=$4
depthfield=$5
#./rt sphere3.iv sphere3.ppm $size $size $shadow $refraction $refraction $depthfield
#./rt refraction1.iv refraction1.ppm $size $size $shadow $reflection $refraction $depthfield
#./rt refraction2.iv refraction2.ppm $size $size $shadow $reflection $refraction $depthfield
#./rt refraction3.iv refraction3.ppm $size $size $shadow $reflection $refraction $depthfield

#./rt cubesphere3.iv cubesphere3.ppm $size $size $shadow $reflection $refraction $depthfield
#./rt cubesphere4.iv cubesphere4.ppm $size $size $shadow $reflection $refraction $depthfield
#./rt cubesphere5.iv cubesphere5.ppm $size $size $shadow $reflection $refraction $depthfield
#./rt sphereCycloid.iv sphereCycloid.ppm $size $size $shadow $reflection $refraction $depthfield

#./rt cube1.iv cube1.ppm 500 500 $shadow $reflection $refraction
#./rt cubesphere.iv cubesphere.ppm 500 500 1 0 $shadow $reflection $refraction
#open .

./rt stretched2.iv stretched2.ppm  $size $size $shadow $reflection $refraction $depthfield
./rt stretched_sphere3.iv stretched_sphere3.ppm $size $size $shadow $reflection $refraction $depthfield



killall ToyViewer
source ~/.bashrc
toyview *.ppm


