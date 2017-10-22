#!/usr/bin/env python
# -*- coding: utf-8 -*-


#######################################################
# 并发服务器 - webserver3g.py                          #
#                                                     #
# 使用 Python 2.7.9 或 3.4                             #
# 在 Ubuntu 14.04 及 Mac OS X 环境下测试通过            #
#######################################################
import errno
import os
import signal
import socket


SERVER_ADDRESS = (HOST, PORT) = '', 8888
REQUEST_QUEUE_SIZE = 1024


def grim_reaper(signum, frame):
    while True:
        try:
            pid, status = os.waitpid(
                -1,          ### 等待所有子进程
                 os.WNOHANG  ### 无终止进程时，不阻塞进程，并抛出 EWOULDBLOCK 错误
            )
        except OSError:
            return
        if pid == 0:  ### 没有僵尸进程存在了
            return


def handle_request(client_connection):
    request = client_connection.recv(1024)
    print(request.decode())
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

    signal.signal(signal.SIGCHLD, grim_reaper)

    while True:
        try:
            client_connection, client_address = listen_socket.accept()
        except IOError as e:
            code, msg = e.args
            ### 若 'accept' 被打断，那么重启它
            if code == errno.EINTR:
                continue
            else:
                raise

        pid = os.fork()
        if pid == 0:  ### 子进程
            listen_socket.close()  ### 关闭子进程中多余的描述符
            handle_request(client_connection)
            client_connection.close()
            os._exit(0)
        else:  ### 父进程
            client_connection.close()  ### 关闭父进程中多余的描述符，继续下一轮循环

if __name__ == '__main__':
    serve_forever()