#!/usr/bin/env python
# -*- coding: utf-8 -*-

from multiprocessing import Process, Queue
import os, time, random

def write(q):
    for value in ['A', 'B', 'C']:
        print 'Put %s to queue...' % value
        q.put(value, block = False)
        time.sleep(random.random())

def read(q):
    while True:
        try:
            value = q.get(block = True,  timeout = 5)
            print 'Get %s from queue.' % value
        except BaseException, e:
            print "empty queue", e
            return

if __name__=='__main__':
    q = Queue()
    pw = Process(target=write, args=(q,))
    pr = Process(target=read, args=(q,))

    pw.start()
    pr.start()

    pw.join()
    pr.join()

