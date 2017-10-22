#!/usr/bin/env python
# -*- coding: utf-8 -*-

#######################################################
# 并发服务器 - webserver3d.py                          #
#                                                     #
# 使用 Python 2.7.9 或 3.4                             #
# 在 Ubuntu 14.04 及 Mac OS X 环境下测试通过            #
#######################################################
import os
import socket


SERVER_ADDRESS = (HOST, PORT) = '', 8888
REQUEST_QUEUE_SIZE = 5


def handle_request(client_connection):
    request = client_connection.recv(1024)
    http_response = b"""\
HTTP/1.1 200 OK

Hello, World!
"""
    client_connection.sendall(http_response)

def serve_forever():
    listen_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    listen_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    listen_socket.bind(SERVER_ADDRESS)
    listen_socket.listen(REQUEST_QUEUE_SIZE)
    print('Serving HTTP on port {port} ...'.format(port=PORT))

    clients = []
    while True:
        client_connection, client_address = listen_socket.accept()
        ### 将引用存储起来，否则在下一轮循环时，他们会被垃圾回收机制销毁
        clients.append(client_connection)
        pid = os.fork()
        if pid == 0:  ### 子进程
            listen_socket.close()  ### 关闭子进程中多余的套接字
            handle_request(client_connection)
            client_connection.close()
            os._exit(0)  ### 子进程在这里结束
        else:  ### 父进程
            # client_connection.close()
            print(len(clients))


if __name__ == '__main__':
    serve_forever()