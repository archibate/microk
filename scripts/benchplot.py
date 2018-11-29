#!/usr/bin/env python

print('generating data figure...')

import sys
import matplotlib.pyplot as plt
plt.title("performence of C4 syscalls")
plt.xlabel("starting time")
plt.ylabel("costed time")
plt.ylim(bottom=0, top=200)

fin = open(sys.argv[1]) if len(sys.argv) > 1 else sys.stdin

names = []
for line in fin.readlines():
    line = line.split(':')
    if len(line) == 3:
        name, clock, time = line
        time  = float(time)
        clock = float(clock)
        try:
            ni = names.index(name)
        except ValueError:
            ni = len(names)
            print('bgrcmykw'[ni], name)
            names.append(name)
        plt.plot(clock, time, 'bgrcmykw'[ni], marker='o', markersize=1)

plt.savefig(sys.argv[2] if len(sys.argv) > 2 else 'figure.png')
