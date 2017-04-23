#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# https://github.com/xchaoinfo/fuck-login
# https://github.com/xchaoinfo/fuck-login/tree/master/001%20zhihu
# https://foofish.net/python-auto-login-zhihu.html


# https://segmentfault.com/q/1010000004867192
# http://docs.python-requests.org/zh_CN/latest/user/advanced.html#advanced

# http://www.jianshu.com/p/c94de9f1ef7c

import time
from http import cookiejar
import requests
from bs4 import BeautifulSoup


headers = {
    "Host": "www.zhihu.com",
    "Referer": "https://www.zhihu.com/",
    'User-Agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_5) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/56.0.2924.87'
}


# 使用登录cookie信息
session = requests.session()
session.cookies = cookiejar.LWPCookieJar(filename='cookies')
try:
    session.cookies.load(ignore_discard=True)
    print("Cookie is loaded!")
    #print(session.cookies)
except:
    print("Cookie is not loaded!")


def get_xsrf():
    '''_xsrf 是一个动态变化的参数'''
    index_url = "https://www.zhihu.com"
    response = session.get(index_url, headers=headers)
    soup = BeautifulSoup(response.content, "html.parser")
    xsrf = soup.find('input', attrs={"name": "_xsrf"}).get("value")
    return xsrf


def get_captcha():
    """
    把验证码图片保存到当前目录，手动识别验证码
    :return:
    """
    t = str(int(time.time() * 1000))
    captcha_url = 'https://www.zhihu.com/captcha.gif?r=' + t + "&type=login"
    r = session.get(captcha_url, headers=headers)
    with open('captcha.jpg', 'wb') as f:
        f.write(r.content)
    captcha = input("please input the captcha: ")
    return captcha


def login(email, password):
    login_url = 'https://www.zhihu.com/login/email'
    data = {
        'email': email,
        'password': password,
        '_xsrf': get_xsrf(),
        "captcha": get_captcha(),
        'remember_me': 'true'}
    response = session.post(login_url, data=data, headers=headers)
    login_code = response.json()
    print(login_code['msg'])
    for i in session.cookies:
        print(i)
    session.cookies.save()


def is_login():
    # 通过查看用户个人信息来判断是否已经登录
    url = "https://www.zhihu.com/settings/profile"
    login_code = session.get(url, headers=headers, allow_redirects=False).status_code
    if login_code == 200:
        return True
    else:
        return False



if __name__ == '__main__':
    if is_login():
        print("You have logined")
    else:
        #email = input("please input your email: ")
        #password = input("please input your password: ")
        email = "weiqiangdragonite@gmail.com"
        password = "1234567890"
        login(email, password)
