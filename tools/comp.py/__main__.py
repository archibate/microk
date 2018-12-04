#!/usr/bin/env python

assert __name__ == '__main__'

from htree import mkht_methods
import sys, getopt


methods = mkht_methods.keys()
archive, file = None, None
uncomp = False
method = None

def usage():
    print('usage:', sys.argv[0], '<archive> -f <file> [-m {' + '|'.join(methods) + '}]')
    sys.exit()

argv = list(sys.argv)
program = argv.pop(0)

if len(argv):
    command = argv.pop(0)
else:
    usage()

try:
    if len(argv) and argv[0][0] != '-':
        archive = argv.pop(0)
except ValueError:
    pass

try:
    options, args = getopt.getopt(argv, 'hf:m:', ['help', 'file=', 'method='])
except getopt.GetoptError:
    usage()

for name, value in options:
    if name in ('-h', '--help'):
        usage()
    if name in ('-f', '--file'):
        file = value
    if name in ('-m', '--method'):
        method = value
        if method not in methods:
            usage()


if 'x' in command or 'u' in command:
    from uncompress import main
    fin = '/dev/stdin' if archive is None else archive
    fout = '/dev/stdout' if file is None else file
elif 'c' in command or 'm' in command:
    from compress import main
    fin = '/dev/stdin' if file is None else file
    fout = '/dev/stdout' if archive is None else archive
else:
    usage()
fin, fout = open(fin, 'rb'), open(fout, 'wb')
main(fin, fout, method)
