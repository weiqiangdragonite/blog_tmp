#!/usr/bin/env python
# -*- coding: utf-8 -*-


#######################################################
# 并发服务器 - webserver3c.py                          #
#                                                     #
# 使用 Python 2.7.9 或 3.4                             #
# 在 Ubuntu 14.04 及 Mac OS X 环境下测试通过            #
#                                                     #
# - 完成客户端请求处理之后，子进程会休眠 60 秒             #
# - 父子进程会关闭重复的描述符                           #
#                                                     #
#######################################################
import os
import socket
import time


SERVER_ADDRESS = (HOST, PORT) = '', 8888
REQUEST_QUEUE_SIZE = 5


def handle_request(client_connection):
    request = client_connection.recv(1024)
    print(
        'Child PID: {pid}. Parent PID {ppid}'.format(
            pid=os.getpid(),
            ppid=os.getppid(),
        )
    )
    print(request.decode())
    http_response = b"""\
HTTP/1.1 200 OK

Hello, World!
"""
    client_connection.sendall(http_response)
    time.sleep(60)

def serve_forever():
    listen_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    listen_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    listen_socket.bind(SERVER_ADDRESS)
    listen_socket.listen(REQUEST_QUEUE_SIZE)
    print('Serving HTTP on port {port} ...'.format(port=PORT))
    print('Parent PID (PPID): {pid}\n'.format(pid=os.getpid()))

    while True:
        client_connection, client_address = listen_socket.accept()
        pid = os.fork()
        if pid == 0:  ### 子进程
            listen_socket.close()  ### 关闭子进程中复制的套接字对象
            handle_request(client_connection)
            client_connection.close()
            os._exit(0)  ### 子进程在这里退出
        else:  ### 父进程
            client_connection.close()  ### 关闭父进程中的客户端连接对象，并循环执行

if __name__ == '__main__':
    serve_forever()