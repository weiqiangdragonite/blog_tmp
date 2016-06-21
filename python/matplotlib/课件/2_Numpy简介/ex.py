#!/usr/bin/env python3
# -*- coding: utf-8 -*-


import matplotlib.pyplot as plt
import numpy as np

# create 30 random numbers from [0, 100)
x1 = np.random.randint(0, 100, 30)

# store to file
# 一定要添加格式，否则很难看
np.savetxt("numbers.txt", x1, fmt = "%2d")

# load from file
x2 = np.loadtxt("numbers.txt")

# sort
x2.sort()
print(x2)

print("max = %d" % x2.max())
print("min = %d" % x2.min())
print("mean = %.2f" % x2.mean())
print("variance = %.2f" % x2.var())
