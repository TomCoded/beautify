#!/bin/sh
mpirun -n 4 beautify -s scenes/sample.dat -n 8000 -N 40
cd scenes
mpirun -n 4 beautify -s largescene3.dat -n 20000 -N 40
cd ..
