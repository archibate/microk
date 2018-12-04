#!/usr/bin/env python


from htree import mkht_methods
from serqu import betterq, serializq, unserializq


def mkoccurtab(xs):
    dic = {}
    for x in xs:
        if x in dic:
            dic[x] += 1
        else:
            dic[x] = 1
    return dic


def mkprioque(xs):
    return betterq(sorted(mkoccurtab(xs).items(), key=lambda itm: itm[1]))


def bits2bytes(bs):
    res = []
    rtn = 0
    r = 0

    for b in bs:
        r |= int(b) << rtn
        rtn += 1
        if rtn == 8:
            res.append(r)
            rtn = 0
            r = 0

    if rtn != 0:
        res.append(r)

    return res



def main(fin, fout, meth):
    if meth is None:
        meth = 'mkpy'
    xs = fin.read()
    fin.close()
    que = mkprioque(xs)
    squ = serializq(que)
    fout.write(b'\x7fZ')
    fout.write(meth.upper().encode('utf-8'))
    fout.write(bytes(squ))
    bmp = mkht_methods[meth](que).tomap()
    bs = ''.join(bmp[x] for x in xs)
    res = bits2bytes(bs)
    fout.write(bytes(res))
    fout.close()

