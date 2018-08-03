#!/bin/sh
for i in `cat ../mach`; do xterm -e ssh $i killall SceneTest; done;
