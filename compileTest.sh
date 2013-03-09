#!/bin/sh
export LIBRARY_PATH=`pwd`:$LIBRARY_PATH

gfortran -g -mtune=native -march=native -O3 -fall-intrinsics -fdefault-integer-8  -fdefault-real-8 -fdefault-double-8 -ffree-line-length-none -fno-second-underscore -fopenmp for_buf_tester.f90 -o for_buf_tester -lforbuf -lstdc++
gfortran -g -mtune=native -march=native -O3 -fall-intrinsics -fdefault-integer-8  -fdefault-real-8 -fdefault-double-8 -ffree-line-length-none -fno-second-underscore -fopenmp for_int_buf_tester.f90 -o for_int_buf_tester -lforbuf -lstdc++
