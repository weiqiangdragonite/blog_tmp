#!/usr/bin/env python
# -*- coding: utf-8 -*-

import web

# 第一部分是匹配URL的正则表达式，第二部分是接受请求的类名称
urls = (
    "/", "index"
)

# 创建一个列举这些url的application
app = web.application(urls, globals())
