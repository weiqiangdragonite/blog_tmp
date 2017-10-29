#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import pika

connection = pika.BlockingConnection(pika.ConnectionParameters(host="rabbit"))
channel = connection.channel()

channel.queue_declare(queue="hello")

def callback(ch, method, properties, body):
    print("[R] Received %r" % body)

channel.basic_consume(callback, queue="hello", no_ack=True)
print("[*] Waiting for message. To exit press CTRL+C")
channel.start_consuming()
