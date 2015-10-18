#!/usr/bin/env python
# -*- coding: utf-8 -*-

import multiprocessing
import time
import os

def worker(interval):
    print "I am (%s) child process, my parent is %s." % (os.getpid(), os.getppid())
    n = 5
    while n > 0:
        print "The time is %s" % (time.ctime())
        time.sleep(interval)
        n -= 1

if __name__ == "__main__":
    print "Parent process %s." % os.getpid()
    p = multiprocessing.Process(target = worker, args = (3,))
    p.start()
    # wait for child process to finish
    #p.join()
    print "p.pid: ", p.pid
    print "p.name: ", p.name
    print "p.is_alive: ", p.is_alive()

