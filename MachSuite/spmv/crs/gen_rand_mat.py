#!/usr/bin/env python
from scipy.sparse import random

ROWS = 2048
COLS = 128

S = random(ROWS, COLS, density=0.15)

vals = 0
for i in range(0, ROWS):
    for j in range(0, COLS):
        if S.A[i][j] != 0:
            vals += 1

print("%%MatrixMarket matrix coordinate real symmetric")
print("{0} {1} {2}".format(ROWS, COLS, vals))

for i in range(0, ROWS):
    for j in range(0, COLS):
        if S.A[i][j] != 0:
            print("{0} {1} {2}".format(i + 1, j + 1, S.A[i][j]))
