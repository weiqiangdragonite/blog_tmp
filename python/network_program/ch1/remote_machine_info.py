#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import socket

def get_remote_machine_info():
    remote_host = "www.pytgo.org"
    try:
        ip_address = socket.gethostbyname(remote_host)
        print("IP address: %s" % ip_address)
    except socket.error as err_msg:
        print("%s : %s" % (remote_host, err_msg))

if __name__ == "__main__":
    get_remote_machine_info()

