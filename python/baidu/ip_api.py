#!/usr/bin/env python
# -*- coding: utf-8 -*-

# http://developer.baidu.com/map/index.php?title=webapi/ip-api
"""
百度官网上的
$querystring_arrays = array (
	'address' => $address,
	'output' => $output,
	'ak' => $ak
);
$querystring = http_build_query($querystring_arrays);

<?php 
$data = array('foo'=>'bar', 
              'baz'=>'boom', 
              'cow'=>'milk', 
              'php'=>'hypertext processor'); 
echo http_build_query($data); 
/* 输出： 
       foo=bar&baz=boom&cow=milk&php=hypertext+processor 
*/ 
?>
"""

import socket, urllib, hashlib, json

def getsn(ak, sk, uri, query_arrays):
    querystring = []
    for key, val in query_arrays:
        val = val.replace(' ', '+')
        querystring.append(key + '=' + val)
    querystring = '&'.join(querystring)
    #print querystring

    url = urllib.quote(uri + '?' + querystring + sk)
    # 还需要把/转换成%2F
    url = url.replace('/', "%2F")
    #print "after encode: " + url

    md5 = hashlib.md5()
    md5.update(url)
    return md5.hexdigest()

# API控制台申请得到的ak
ak = "2d7e1a475468cd13ef37c6f4a8462cff"

# 应用类型为for server, 请求校验方式为sn校验方式时，系统会自动生成sk，
# 可以在应用配置-设置中选择Security Key显示进行查看
sk = "E5f90174225471f245267b5741073703"

#
url = "http://api.map.baidu.com/location/ip?ak=%s&sn=%s"

coor = "bd0911"
# get请求uri前缀
uri = "/location/ip"
query_arrays = [("ak", ak), ("coor", coor)]

# sn = 9e7fc96d9e06c667687455085e275ab2
sn = getsn(ak, sk, uri, query_arrays)
#print "sn = " + sn


get = "/location/ip?ak=%s&sn=%s&coor=%s" % (ak, sn, coor)
headers = \
    "GET %s HTTP/1.1\r\n" % (get) + \
    "Host: api.map.baidu.com\r\n" + \
    "Connection: Keep-Alive\r\n" + \
    "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n" + \
    "User-Agent: Mozilla/5.0 (X11; Linux) AppleWebKit/538.1 (KHTML, like Gecko) Chrome/18.0.1025.133 Safari/538.1 Midori/0.5\r\n" + \
    "Accept-Language: en-us;q=0.750\r\n" + \
    "\r\n"


s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(("api.map.baidu.com", 80))
s.send(headers)

buffer = []
while True:
    d = s.recv(1024)
    if d:
        buffer.append(d)
    else:
        break
data = ''.join(buffer)
s.close()

#print data
response = data.split("\r\n\r\n")
content = response[1].decode("unicode-escape")
# type = unicode
#print "before: type(content) = ", type(content)
print content

#content = json.loads(content)
# type = dict
#print "after: type(content) = ", type(content)
#print content
