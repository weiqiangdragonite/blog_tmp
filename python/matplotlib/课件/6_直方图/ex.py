#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 直方图

import numpy as np
import matplotlib.pyplot as plt

'''
# 例子1
mu = 100 # mean of distribution 均值
sigma = 20 # standard deviation of distribution 标准差
x = mu + sigma * np.random.randn(2000) # 生成一个含有2000个数据的数组

# 画出直方图
# bins: 有多少直方，color，normed: 是否要对数据标本化
plt.hist(x, bins=10, color='red', normed=True)
plt.show()
'''


'''
# 例子2 - 双变量的直方图
x = np.random.randn(1000) + 2 # 中心在2
y = np.random.randn(1000) + 3 # 中心在3

plt.hist2d(x, y, bins = 40)
plt.show()
'''

'''
# 练习1
mu = 10 # mean of distribution 均值
sigma = 3 # standard deviation of distribution 标准差
x = mu + sigma * np.random.randn(2000) # 生成一个含有2000个数据的数组

# 画出直方图
# bins: 有多少直方，color，normed: 是否要对数据标本化
plt.hist(x, bins=10, color='red', normed=True)
plt.show()
plt.hist(x, bins=50, color='blue', normed=False)
plt.show()
'''


# 练习2 - 双变量的直方图
x = np.random.randn(2000) + 1 # 均值为1
y = np.random.randn(2000) + 5 # 均值为5

plt.hist2d(x, y, bins = 40)
plt.show()

