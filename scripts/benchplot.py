#!/usr/bin/env python

print('generating data figure...')

import sys
import matplotlib.pyplot as plt
plt.title("performence of L4 syscalls")
plt.xlabel("starting time")
plt.ylabel("costed time")
#plt.subplot(121)
plt.ylim(bottom=0, top=120)
#plt.subplot(122)
#plt.ylim(bottom=0, top=1000)

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
        #plt.subplot(121 if name in 'it' else 122)
        plt.plot(clock, time, 'bgrcmykw'[ni], marker='o', markersize=1)

plt.show()
plt.savefig(sys.argv[2] if len(sys.argv) > 2 else 'figure.png')
