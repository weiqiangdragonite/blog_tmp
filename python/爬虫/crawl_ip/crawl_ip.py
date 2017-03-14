#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import requests
from pyquery import PyQuery as pq


headers = { \
'User-Agent': 'Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/45.0.2924.87 Safari/537.36', \
'Accept-Encoding': 'gzip, deflate, sdch', \
'Accept-Language': 'zh-CN,zh;q=0.8', \
}


# 通过ip138
def query_ip(ip):
    info = {}
    url = "http://www.ip138.com/ips138.asp?ip={ip}".format(ip=ip)
    res = requests.get(url, timeout = 60, headers = headers)
    page = pq(res.content)
    data = page("ul.ul1")

    if len(data):
        data = str(data)
        s = data[data.find("：")+1:data.find("</li>")]
        info[ip] = ' '.join(s.split())
    else:
        info[ip] = ''
    print(info)
    return info

def query_ip_2(ip):
    info = {}
    url = "http://www.baidu.com/s?wd={ip}".format(ip=ip)
    res = requests.get(url, timeout = 60, headers = headers)
    page = pq(res.content)
    data = page("span.c-gap-right")

    if len(data):
        data = str(data)
        s = data[data.find('</span>')+7:]
        info[ip] = ' '.join(s.split())
    else:
        info[ip] = ''
    print(info)
    return info


if __name__ == "__main__":
    query_ip("61.140.126.121")
