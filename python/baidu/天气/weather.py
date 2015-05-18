__author__ = 'saint'
 
import os
import urllib.request
import urllib.parse
import json
 
class weather(object):
 
    # 获取城市代码的uri - 天气查询_根据城市名称
    code_uri = "http://apistore.baidu.com/microservice/cityinfo?cityname="
 
    # 获取天气信息的uri - 天气查询_根据城市代码
    weather_uri = "http://apistore.baidu.com/microservice/weather?cityid="
 
    # 主处理逻辑
    def mainHandle(self):
        print("输入你要查询天气的城市名称：")
        city_name = input()
        uri = self.code_uri + urllib.parse.quote(city_name)
        ret = json.loads(urllib.request.urlopen(uri).read().decode("utf8"))
        if ret['errNum'] != 0:
            print(ret['retMsg'])
            return False
        else:
            weather_uri = self.weather_uri + ret['retData']['cityCode']
            data = json.loads(urllib.request.urlopen(weather_uri).read().decode("utf8"))
            if data['errNum'] == 0:
                ret_data = data['retData']
                output = "城市名:" + city_name + "\r\n"
                output += "更新时间:" + ret_data["date"] + " " + ret_data["time"] + "\r\n"
                output += "天气:" + ret_data["weather"] + " [" + ret_data["WD"] + ret_data["WS"] + "]\r\n"
                output += "当前温度:" + ret_data["temp"] + " (" + ret_data["h_tmp"] + " ---> " + ret_data["l_tmp"] + ")\r\n"
                print(output)
                return True
            else:
                print(data['errMsg'])
                return False
 
if __name__ == "__main__":
    weather = weather()
    weather.mainHandle()