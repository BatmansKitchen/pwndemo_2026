#!/bin/sh
cd /app && exec stdbuf -i0 -o0 -e0 gdb -q /app/basic1