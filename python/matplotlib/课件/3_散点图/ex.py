#!/usr/bin/env python3
# -*- coding: utf-8 -*-


import matplotlib.pyplot as plt
import numpy as np


'''
# 例子1，显示散点图 （身高-体重） 可看出有明显的正相关性
height = [161, 170, 182, 175, 173, 165]
weight = [50, 58, 80, 70, 69, 55]

plt.scatter(height, weight)
plt.show()
'''


'''
# 例子2，显示相关性：正相关，负相关，不相关
N = 1000
x = np.random.randn(N)
y1 = np.random.randn(N) # 不相关
y2 = x + np.random.randn(N) * 0.5 # 正相关
y3 = -x + np.random.randn(N) * 0.5 # 负相关

plt.scatter(x, y3)
plt.show()
'''


'''
# 例子3，前一天和后一天的价格涨幅的散点图
# 例子4，外观调整，主要是颜色c，点大小s，透明度alpha，点形状marker
open, close = np.loadtxt('000001.csv', delimiter = ',', skiprows = 1, usecols = (1, 4), unpack = True)

change = close - open # 每天的价格变化
yesterday = change[:-1] #前一天的价格
today = change[1:] # 后一天的价格

# 点的大小默认值为20，颜色为蓝色，s是面积，具体查文档
plt.scatter(yesterday, today, s=500, c='r', marker='*', alpha=0.5)
plt.show()
'''


# 作业：计算最高价和开盘价之差diff
# 会出前后两天的diff散点图
open, high = np.loadtxt('000001.csv', delimiter = ',', skiprows = 1, usecols = (1, 2), unpack = True)

diff = high - open
yesterday = diff[:-1]
today = diff[1:]

plt.scatter(yesterday, today, s=500, c='r', alpha=0.5)
plt.show()
