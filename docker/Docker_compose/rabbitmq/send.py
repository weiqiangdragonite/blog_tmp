#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import pika, sys

arg = "hello world"
if len(sys.argv) == 2:
    arg = sys.argv[1]

connection = pika.BlockingConnection(pika.ConnectionParameters(host="some-rabbit"))
channel = connection.channel()

channel.queue_declare(queue="hello")

channel.basic_publish(exchange="", routing_key="hello", body=arg)
print("[S] Sent %r" % arg)
connection.close()

