#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import socket
import sys




def main():
    host = "git.dgni.net"
    port = 80

    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    except socket.error as err:
        print("Error creating socket: %s" % err)
        sys.exit(1)

    try:
        s.connect((host, port))
    except socket.gaierror as err:
        print("Address-related error connecting to server: %s" % err)
        sys.exit(1)
    except socket.error as err:
        print("Connection error: %s" % err)
        sys.exit(1)

    try:
        s.semdall("GET / HTTP/1.0\r\n\r\n")
    except socket.error as err:
        print("Error sending data: %s" % err)
        sys.exit(1)

    while True:
        try:
            buf = s.recv(1024)
        except socket.error as err:
            print("Error receiving data: %s" % err)
            sys.exit(1)
        if len(buf) is None:
            break
        sys.stdout.write(buf)


if __name__ == "__main__":
    main()




