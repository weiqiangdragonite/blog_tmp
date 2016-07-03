#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 箱线图：上边缘，上四分位数，中位数，下四分位数，下边缘，异常值

import numpy as np
import matplotlib.pyplot as plt



np.random.seed(100)
data = np.random.normal(size = 1000, loc = 0, scale = 1) # 大小为1000的数组
# sym: 异常值的形状  whis: 虚线的长度，虚线越大，上下边缘越大，虚线越小，上下边缘越小
plt.boxplot(data, sym = 'o', whis = 1.5)
plt.show()




data = np.random.normal(size = (1000, 4), loc = 0, scale = 1) # 4个1000的数组
labels = ['A', 'B', 'C', 'D']
plt.boxplot(data, labels = labels, sym = 'o')
plt.show()




data = np.random.normal(size = (100, 5), loc = 0, scale = 1) # 5个100的数组
labels = ['A', 'B', 'C', 'D', 'E']
plt.boxplot(data, labels = labels, sym = 'o', whis = 1.0)
plt.show()
