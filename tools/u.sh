#!/bin/sh
set -e
x=${1-catkbd}
make uncompress
./uncompress $x.z $x.out
