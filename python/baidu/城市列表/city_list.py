
获取全国的城市列表


1.从百度获取 http://passport.baidu.com/js/sitedata_bas.js

代码：
#!/usr/bin/env python
# -*- coding: utf-8 -*-

import urllib2

url = "http://passport.baidu.com/js/sitedata_bas.js"
response = urllib2.urlopen(url)

#
print response.geturl()
headers = response.info()
#print headers

data = response.read().decode("gbk").encode("utf-8")
#print data
    
with open("city_list_data", "w") as f:
    f.write(data)

################################################################################

2.使用中国天气网的开放平台



################################################################################

http://webservice.webxml.com.cn/WebServices/WeatherWS.asmx

################################################################################

