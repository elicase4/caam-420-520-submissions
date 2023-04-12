#!/bin/sh
#
# halo_exchange.sh
# Created:      Apr, 10, 2023 15:19:15 by Eli Case

# Input Variables
# $1 = NX
# $2 = NY
# $3 = HALO_RADIUS

set -eu

export NX=$1;
export NY=$2;
export HALO_RADIUS=$3;
mpirun -n $(($NX*$NY)) ./halo;
