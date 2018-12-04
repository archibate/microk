#!/bin/sh
set -e
x=${1-catkbd}
./compress $x $x.z
