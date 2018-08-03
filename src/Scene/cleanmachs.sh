#!/bin/sh
for i in `cat ../mach`; do ssh $i killall beautify; done;
