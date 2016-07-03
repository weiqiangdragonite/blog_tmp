#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 条形图


import matplotlib.pyplot as plt
import numpy as np

'''
# ex-1
# 垂直条形
N = 5
y = [20, 10, 30, 25, 15]
index = np.arange(N)
p1 = plt.bar(left=index, height=y, color='red', width=0.5)
plt.show()

# 水平条形
p1 = plt.bar(left=0, bottom=index, width=y, color='red', height=0.5, orientation='horizontal')
# 可以这样写
#pl = plt.barh(left=0, bottom=index, width=y, color='blue', height=0.5)
plt.show()
'''

'''
# ex-2
index = np.arange(4)
sales_BJ = [52, 55, 63, 53]
sales_SH = [44, 66, 55, 41]

bar_width = 0.25

# 垂直并列
plt.bar(index, sales_BJ, bar_width, color='b')
plt.bar(index+bar_width, sales_SH, bar_width, color='r')
plt.show()

# 垂直层叠
bar_width = 0.5
plt.bar(index, sales_BJ, bar_width, color='b')
plt.bar(index, sales_SH, bar_width, color='r', bottom=sales_BJ)
plt.show()
'''


# ex-3
index = np.arange(5)
sales_BJ = [52, 55, 63, 53, 58]
sales_SH = [44, 66, 55, 41, 50]

bar_width = 0.25

# 水平并列
plt.barh(left=0, bottom=index, width=sales_BJ, height=bar_width, color='b')
plt.barh(left=0, bottom=index+bar_width, width=sales_SH, height=bar_width, color='r')
plt.show()


# 水平层叠
bar_width = 0.5
plt.barh(left=0, bottom=index, width=sales_BJ, height=bar_width, color='b')
plt.barh(left=sales_BJ, bottom=index, width=sales_SH, height=bar_width, color='r')
plt.show()

