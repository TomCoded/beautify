#!/bin/sh
for i in `cat $HOME/mach`; do ssh $i wall bmesg; done;
