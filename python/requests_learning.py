#!/usr/bin/env python3
# -*- coding: utf-8 -*-

'''
Requests 模块学习 http://cn.python-requests.org/zh_CN/latest/

安装
apt-get install python3-pip
pip3 install requests

'''

import requests


# -- 1、发送请求

# 尝试获取某个网页
r = requests.get('https://github.com/timeline.json')
# 现在，我们有一个名为 r 的 Response 对象。我们可以从这个对象中获取所有
# 我们想要的信息。

# 可以这样发送一个 HTTP POST 请求：
r = requests.post("http://httpbin.org/post")

# 其他 HTTP 请求类型：PUT，DELETE，HEAD 以及 OPTIONS 
r = requests.put("http://httpbin.org/put")
r = requests.delete("http://httpbin.org/delete")
r = requests.head("http://httpbin.org/get")
r = requests.options("http://httpbin.org/get")



# -- 2、传递 URL 参数

# 你也许经常想为 URL 的查询字符串(query string)传递某种数据。如果你是手工
# 构建 URL，那么数据会以键/值对的形式置于 URL 中，跟在一个问号的后面。
# 例如， httpbin.org/get?key=val。 Requests 允许你使用 params 关键字参数，
# 以一个字典来提供这些参数。举例来说，如果你想传递 key1=value1 和 key2=value2 
# 到 httpbin.org/get ，那么你可以使用如下代码：
payload = {'key1': 'value1', 'key2': 'value2'}
r = requests.get("http://httpbin.org/get", params=payload)

#通过打印输出该 URL，你能看到 URL 已被正确编码：
print(r.url)
# http://httpbin.org/get?key2=value2&key1=value1
# 注意字典里值为 None 的键都不会被添加到 URL 的查询字符串里。

