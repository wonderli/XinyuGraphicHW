#!/bin/sh
make clean
make 
./rt sphere3s.iv sphere3s.ppm 500 500 0 0
./rt mickeyspheres.iv mickeyspheres.ppm 500 500 0 0
./rt sphereCycloidLit.iv sphereCycloidLit.ppm 500 500 0 0
./rt sphereCycloid.iv sphereCycloid.ppm 500 500 0 0
./rt sphereTowerLit.iv sphereTowerLit.ppm 500 500 0 0
./rt sphere3s.iv sphere3s_shadow.ppm 500 500 1 0
./rt mickeyspheres.iv mickeyspheres_shadow.ppm 500 500 1 0
./rt sphereCycloidLit.iv sphereCycloidLit_shadow.ppm 500 500 1 0
./rt sphereCycloid.iv sphereCycloid_shadow.ppm 500 500 1 0
./rt sphereTowerLit.iv sphereTowerLit_shadow.ppm 500 500 1 0
./rt sphere3s.iv sphere3s_reflection.ppm 500 500 1 1
./rt mickeyspheres.iv mickeyspheres_reflection.ppm 500 500 1 1
./rt sphereCycloidLit.iv sphereCycloidLit_reflection.ppm 500 500 1 1
./rt sphereCycloid.iv sphereCycloid_reflection.ppm 500 500 1 1
./rt sphereTowerLit.iv sphereTowerLit_reflection.ppm 500 500 1 1





