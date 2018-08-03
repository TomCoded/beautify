#!/bin/sh
for i in `cat $HOME/mach`; do ssh $i "kill -9 \`ps aux | grep 04thw | awk '{print $2}'\`"; done;
