#!/bin/sh
for i in `cat $HOME/mach`; do ssh $i killall -9 beautify; done;
