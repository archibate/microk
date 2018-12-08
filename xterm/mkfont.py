import sys, struct

with open(sys.argv[1], 'r') as f:
    with open(sys.argv[2], 'wb') as fout:
        for line in f.readlines():
            if len(line) >= 8 and line[0] != '#':
                res = 0
                for i in range(8):
                    res *= 2
                    res += '.*'.index(line[i])
                fout.write(struct.pack('B', res))
