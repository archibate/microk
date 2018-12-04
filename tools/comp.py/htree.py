#!/usr/bin/env python


class Node:

    def __init__(n, l, r):
        n.l = l
        n.r = r

    def __repr__(n):
        return '(' + repr(n.l) + ':' + repr(n.r) + ')'

    def tomap(n, bmp={}, b=''):

        if isinstance(n.l, Node):
            n.l.tomap(bmp, b + '0')
        else:
            bmp[n.l] = b + '0'

        if isinstance(n.r, Node):
            n.r.tomap(bmp, b + '1')
        else:
            bmp[n.r] = b + '1'

        return bmp


def mkhtree_cgen(que):

    #print('\n'.join(map(str, que)))

    assert len(que) < 257
    q = [[k, ki] for k, ki in que]
    q += [[0, 0xffffff] for i in range(514 - len(que))]

    poi = 257
    htr = None

    while True:
        m1 = m2 = 513
        for i in range(512):
            if q[i][1] <= q[m1][1]:
                m1, m2 = i, m1
            elif q[i][1] <= q[m2][1]:
                m2 = i

        if q[m2][1] == 0xffffff and isinstance(q[m1][0], Node):
            htr = q[m1][0]
            break

        q[poi] = [Node(q[m1][0], q[m2][0]), q[m1][1] + q[m2][1]]
        q[m1][1] = q[m2][1] = 0xffffff
        poi += 1

    inbalencize(htr)
    return htr


def mkhtree_mkpy(que):

    def locquepos(que, ni):
        for i, (k, ki) in enumerate(que):
            if ki >= ni:
                return i
        else:
            return -1

    while len(que) != 1:
        l, li = que.pop(0)
        r, ri = que.pop(0)
        n, ni = Node(l, r), (li + ri)

        pos = locquepos(que, ni)
        que.insert(pos, (n, ni))

    htr = que[0][0]
    inbalencize(htr)
    return htr


mkht_methods = {
        'cgen': mkhtree_cgen,
        'mkpy': mkhtree_mkpy,
}


def inbalencize(n):
    if not isinstance(n, Node):
        return 1

    ln = inbalencize(n.l)
    rn = inbalencize(n.r)

    if not (ln <= rn):
        n.r, n.l = n.l, n.r

    return ln + rn
