#!/usr/bin/env python
# -*- coding: utf-8 -*-

import socket
import StringIO
import sys


class WSGIServer(object):
    '''
        doc
    '''
    address_family = socket.AF_INET
    socket_type = socket.SOCK_STREAM
    request_quene_size = 1

    def __init__(self, server_address):
        # create a listeing socket
        self.listen_socket = listen_socket = socket.socket(
            self.address_family,
            self.socket_type)
        # allow to reuse









