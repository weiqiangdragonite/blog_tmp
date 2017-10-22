#!/usr/bin/env python
# -*- coding: utf-8 -*-

def app(environ, start_response):
    """一个最简单的 WSGI 应用。
    这是你自己的 Web 框架的起点 ^_^
    """
    status = '200 OK'
    response_headers = [('Content-Type', 'text/plain')]
    start_response(status, response_headers)
    return ['Hello world from a simple WSGI application!\n']