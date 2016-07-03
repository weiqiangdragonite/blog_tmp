#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 折线图


import numpy as np
import matplotlib.pyplot as plt
import matplotlib.dates as mdates


'''
# 例子1 - 函数图（二次曲线图）
x = np.linspace(-10, 10, 100)   # x从-10到10分成100份
y = x ** 2
plt.plot(x, y)
plt.show()
'''


'''
# 例子2 - 股票时间序列图
# python3 运行下面会出错
date, open, close = np.loadtxt("000001.csv", delimiter=',', \
    converters={0:mdates.strpdate2num('%m/%d/%Y')}, skiprows=1, \
    usecols=(0, 1, 4), unpack=True)
#plt.plot(date, open)    # date是浮点数
plt.plot_date(date, open, linestyle='-', color='red', marker='o')
plt.plot_date(date, close, linestyle='--', color='green', marker='<')
plt.show()
'''


# 正弦函数
x = np.linspace(0, 10, 100)
y = np.sin(x)
plt.plot(x, y)
plt.show()



date, open, high, low, close = np.loadtxt("000001.csv", delimiter=',', \
    converters={0:mdates.strpdate2num('%m/%d/%Y')}, skiprows=1, \
    usecols=(0, 1, 2, 3, 4), unpack=True)
plt.plot_date(date, open, 'y-')
plt.plot_date(date, close, 'g-')
plt.plot_date(date, high, 'b-')
plt.plot_date(date, low, 'r-')
plt.show()


