#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# 获取NGINX日志的IP、数量和位置:
# 先使用AWK获取NGINX日志里访问的IP和数量，然后利用Python获取IP的地理位置，
# 因为用的GeoIP库不太准，所以到百度获取
# 多线程版本


import os
from multiprocessing import Pool
import requests
from pyquery import PyQuery as pq


headers = { \
'User-Agent': 'Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/45.0.2924.87 Safari/537.36', \
'Accept-Encoding': 'gzip, deflate, sdch', \
'Accept-Language': 'zh-CN,zh;q=0.8', \
}


def query_ip_2(ip):
    info = {}
    url = "http://www.baidu.com/s?wd={ip}".format(ip=ip)
    try:
        res = requests.get(url, timeout = 30, headers = headers)
        page = pq(res.content)
        data = page("span.c-gap-right")

        if len(data):
            data = str(data)
            s = data[data.find('</span>')+7:]
            info[ip] = ' '.join(s.split())
        else:
            info[ip] = 'None'
    except Exception as e:
        info[ip] = str(e)
    #print(info)

    return info


def get_ip_addr(ip):
    print('Run task %s (%s)...' % (ip, os.getpid()))
    addr = query_ip_2(ip)
    #ip_d[ip]['addr'] = addr[ip]

    return addr[ip]


if __name__ == "__main__":
    ip_d = dict()
    with open('/tmp/zzjcy.txt', 'r') as f:
        lines = f.readlines()
    for li in lines:
        li = li.strip().split()
        num, ip = li[0], li[1]
        if ip not in ip_d:
            ip_d[ip] = {'num': 0, 'addr': ''}
        else:
            ip_d[ip]['num'] += int(num)
    #print(ip_d)

    p = Pool(8)
    for k, v in ip_d.items():
        ip_d[k]['addr'] = p.apply_async(get_ip_addr, args=(k,)).get()
    p.close()
    p.join()

    #print(ip_d)

    print("start writing file")
    with open('/tmp/result.csv', 'w') as f:
        for k, v in ip_d.items():
            try:
                f.write("%d,%s,%s\n" % (ip_d[k]['num'], k, ip_d[k]['addr']))
            except Exception as e:
                print(e)