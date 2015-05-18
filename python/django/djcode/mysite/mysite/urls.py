#!/usr/bin/env python
# -*- coding: utf-8 -*-

from django.conf.urls import patterns, include, url

# Uncomment the next two lines to enable the admin:
from django.contrib import admin
admin.autodiscover()


# 首先，我们从模块 (在 Python 的 import 语法中， mysite/views.py 转译
# 为 mysite.views ) 中引入了 hello 视图。
from mysite.views import hello, homepage, current_datetime, hours_ahead, \
    datetime_template, display_request_info, login

# 直接将views模块import进来了
from mysite.books import views

"""
正则表达式
这里是一些基本的语法。

符号		匹配
. (dot)		任意单一字符
\d		任意一位数字
[A-Z]		A 到 Z中任意一个字符（大写）
[a-z]		a 到 z中任意一个字符（小写）
[A-Za-z]	a 到 z中任意一个字符（不区分大小写）
+		匹配一个或更多 (例如, \d+ 匹配一个或 多个数字字符)
[^/]+		一个或多个不为‘/’的字符
*		零个或一个之前的表达式（例如：\d? 匹配零个或一个数字）
*		匹配0个或更多 (例如, \d* 匹配0个 或更多数字字符)
{1,3}		介于一个和三个（包含）之前的表达式（例如，\d{1,3}匹配一个或两个或三个数字）
"""

# 调用 patterns() 函数并将返回结果保存到 urlpatterns 变量
urlpatterns = patterns('',
    # Examples:
    # url(r'^$', 'mysite.views.home', name='home'),
    # url(r'^blog/', include('blog.urls')),

    # Uncomment the next line to enable the admin:
    url(r'^admin/', include(admin.site.urls)),


    # (r'^mysite/', include('mysite.foo.urls')),
    # 如果想在URLconf中加入URL和view，只需增加映射URL模式和view功能的Python tuple即可
    # 所有指向 URL /hello/ 的请求都应由 hello 这个视图函数来处理
    # 元组中第一个元素是模式匹配字符串（正则表达式）
    # 正则表达式字符串的开头字母“r”。 它告诉Python这是个原始字符串，不需要处理
    # 里面的反斜杠（转义字符）
    # 另外需要注意的是，我们把hello视图函数作为一个对象传递，而不是调用它
    url('^hello/$', hello),
    # 模式包含了一个尖号(^)和一个美元符号($)。这些都是正则表达式符号，并且有
    # 特定的含义： 上箭头要求表达式对字符串的头部进行匹配，美元符号则要求
    # 表达式对字符串的尾部进行匹配。
    # 我们使用这两个符号以确保只有/hello/匹配，不多也不少。
    # 你可能会问：如果有人申请访问/hello（尾部没有斜杠/）会怎样。 因为我们
    # 的URL模式要求尾部有一个斜杠(/)，那个申请URL将不匹配。 然而，默认地，
    # 任何不匹配或尾部没有斜杠(/)的申请URL，将被重定向至尾部包含斜杠的
    # 相同字眼的URL。(这是受配置文件setting中APPEND_SLASH项控制的)

    # 根目录，URL模式 '^$' , 它代表一个空字符串
    url(r'^$', homepage),

    # 日期时间
    url(r'^time/$', current_datetime),
    url(r'^time_template/$', datetime_template),
    # 使用d+来匹配1个以上的数字, 这个URL模式将匹配类似 /time/plus/2/ , 
    # /time/plus/25/ ,甚至 /time/plus/100000000000/ 的任何URL
    #(r'^time/plus/\d+/$', hours_ahead),
    # 我们把它限制在最大允许99个小时， 这样我们就只允许一个或两个数字，
    # 正则表达式的语法就是 \d{1,2}
    # 我们使用圆括号把参数在URL模式里标识 出来，正则表达式也是用圆括号来从文本里 提取 数据的
    url(r'^time/plus/(\d{1,2})/$', hours_ahead),

    # url and request info
    url(r'^request_info/$', display_request_info),

    #
    #url(r'^search_form/$', views.search_form),
    url(r'^search/$', views.search),
    url(r'^contact/$', views.contact),
    url(r'^contact/thanks/$', views.thanks),
    url(r'^new_contact/$', views.new_contact),

    # login
    url(r'^login/$', login),
)
