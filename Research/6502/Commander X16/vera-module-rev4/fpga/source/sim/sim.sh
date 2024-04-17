#!/bin/bash
set -e
iverilog -Wall -Wno-timescale -Wno-implicit-dimensions -g2001 -gno-xtypes -gstrict-ca-eval -gstrict-expr-width -y. -y.. -y../video -y../graphics -y../uart -y../spi -y../audio tb.v
./a.out
rm -f a.out
