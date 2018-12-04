#!/usr/bin/env python

import struct


def unserializq(fin):
    que = []
    while True:
        k, ki = struct.unpack('BB', fin.read(2))
        if ki == 0xff:
            break
        que.append((k, ki))
    return que


def serializq(que):
    squ = []
    maxi = 1
    for k, ki in que:
        if ki > maxi:
            ki = maxi + 1
            maxi = ki
            assert maxi < 0xff
        squ += [k, ki]
    squ += [0, 0xff]
    return squ

def betterq(que):
    maxi = 1
    for i, (k, ki) in enumerate(que):
        if ki > maxi:
            ki = maxi + 1
            maxi = ki
            assert maxi < 0xff
        que[i] = [k, ki]
    return que
