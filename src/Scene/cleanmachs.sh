#!/bin/sh
for i in `cat ../mach`; do ssh $i killall -9 beautify; done;
