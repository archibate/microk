#!/usr/bin/env python

from htree import mkht_methods, Node
from serqu import unserializq


def bytes2bits(res):
    bs = []

    for r in res:
        bs += [True if (r & (1 << i)) != 0 else False for i in range(8)]

    return bs


def unapplyhtr(bs, htr):
    res = []
    n = htr
    for b in bs:
        if not isinstance(n, Node):
            res.append(n)
            n = htr
        n = n.r if b else n.l
    return res


def main(fin, fout, meth):
    if fin.read(2) != b'\x7fZ':
        raise Exception('wrong file format!')
    meth = fin.read(4).decode('utf-8').lower()
    if meth not in mkht_methods:
        raise Exception('unknown compressing method!')
    que = unserializq(fin)
    htr = mkht_methods[meth](que)
    res = fin.read()
    fin.close()

    bs = bytes2bits(res)
    res = unapplyhtr(bs, htr)
    fout.write(bytes(res))
    fout.close()

