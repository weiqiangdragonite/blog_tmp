#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 饼状图

import matplotlib.pyplot as plt


labels = 'A', 'B', 'C', 'D'
fracs = [15, 30, 45, 10]

plt.axes(aspect=1)  # x轴和y轴的比例为1比1
# autopct: 加入每一块所占比例的值， explode: 突出显示某一块，shadow: 加上阴影
explode = [0, 0.1, 0.1, 0]
plt.pie(x = fracs, labels = labels, autopct = '%.0f%%', explode = explode, shadow = True)
plt.show()



# 练习
labels = 'SH', 'BJ', 'SZ', 'GZ'
fracs = [20, 10, 30, 25]

plt.axes(aspect=1)  # x轴和y轴的比例为1比1
# autopct: 加入每一块所占比例的值， explode: 突出显示某一块，shadow: 加上阴影
explode = [0, 0, 0.1, 0]
plt.pie(x = fracs, labels = labels, autopct = '%.1f%%', explode = explode, shadow = True)
plt.show()
