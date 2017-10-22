#!/usr/bin/env python
# -*- coding: utf-8 -*-

import socket
import StringIO
import sys
import time


class WSGIServer(object):
    '''
        doc
    '''
    address_family = socket.AF_INET
    socket_type = socket.SOCK_STREAM
    request_queue_size = 1

    def __init__(self, server_address):
        # create a listeing socket
        self.listen_socket = listen_socket = socket.socket(
            self.address_family,
            self.socket_type)
        # allow to reuse
        listen_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        # bind to address
        listen_socket.bind(server_address)
        # listen socket
        listen_socket.listen(self.request_queue_size)
        # get host and port
        host, port = self.listen_socket.getsockname()[:2]
        self.server_name = socket.getfqdn(host)
        self.server_port = port
        # 返回由 Web 框架/应用设定的响应头部字段
        self.headers_set = []

    def set_app(self, application):
        self.application = application

    def serve_forver(self):
        listen_socket = self.listen_socket
        while True:
            # get new client connect
            self.client_connection, client_address = listen_socket.accept()
            # handle one request and wait for next connection
            self.handle_one_request()

    def handle_one_request(self):
        self.request_data = request_data = self.client_connection.recv(1024)
        # 以 'curl -v' 的风格输出格式化请求数据
        print(''.join('< {line}\n'.format(line=line) 
            for line in request_data.splitlines()
        ))

        self.parse_request(request_data)

        # 根据请求数据构建环境变量字典
        env = self.get_environ()

        # 此时需要调用 Web 应用来获取结果，
        # 取回的结果将成为 HTTP 响应体
        result = self.application(env, self.start_response)

        # 构造一个响应，回送至客户端
        self.finish_response(result)

    def parse_request(self, text):
        request_line = text.splitlines()[0]
        request_line = request_line.rstrip("\r\n")
        # 将请求行分成几个部分
        (self.request_method,       # GET
         self.path,                 # /path
         self.request_version       # HTTP/1.0
        ) = request_line.split()

    def get_environ(self):
        env = {}
        ### 以下代码段没有遵循 PEP8 规则，但这样排版，是为了通过强调
        ### 所需变量及它们的值，来达到其展示目的。
        ###
        ### WSGI 必需变量
        env['wsgi.version']     = (1, 0)
        env['wsgi.url_scheme']  = 'http'
        env['wsgi.input']       = StringIO.StringIO(self.request_data)
        env['wsgi.errors']      = sys.stderr
        env['wsgi.multithread'] = False
        env['wsgi.multiprocess']= False
        env['wsgi.run_once']    = False
        ### CGI必需变量
        env['REQUEST_METHOD']   = self.request_method
        env['PATH_INFO']        = self.path
        env['SERVER_NAME']      = self.server_name      # localhost
        env['SERVER_PORT']      = str(self.server_port)

        return env


    def start_response(self, status, response_headers, exc_info = None):
        ### 添加必要的服务器头部字段
        server_headers = [
            ('Date', time.strftime("%a, %d %b %Y %H:%M:%S GMT", time.gmtime())),
            ('Server', 'WSGIServer 0.2'),
        ]
        self.headers_set = [status, response_headers + server_headers]
        ### 为了遵循 WSGI 协议，start_response 函数必须返回一个 'write'
        ### 可调用对象（返回值.write 可以作为函数调用）。为了简便，我们
        ### 在这里无视这个细节。
        ### return self.finish_response

    def finish_response(self, result):
        try:
            status, response_headers = self.headers_set
            response = 'HTTP/1.1 {status}\r\n'.format(status = status)
            for header in response_headers:
                response += '{0}: {1}\r\n'.format(*header)
            response += '\r\n'
            for data in result:
                response += data
            ### 以 'curl -v' 的风格输出格式化返回数据
            print(''.join('> {line}\n'.format(line=line) 
                for line in response.splitlines()
            ))
            self.client_connection.sendall(response)
        finally:
            self.client_connection.close()

SERVER_ADDRESS = (HOST, PORT) = '', 8888

def make_server(server_address, application):
    server = WSGIServer(server_address)
    server.set_app(application)
    return server


if __name__ == "__main__":
    if len(sys.argv) < 2:
        sys.exit("Provide a WSGI application object as module:callable")
    app_path = sys.argv[1]
    module, application = app_path.split(":")
    module = __import__(module)
    application = getattr(module, application)
    httpd = make_server(SERVER_ADDRESS, application)
    print("WSGIServer: Serving HTTP on port {port} ... \n".format(port = PORT))
    httpd.serve_forver()
