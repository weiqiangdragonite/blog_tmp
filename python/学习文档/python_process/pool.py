#!/usr/bin/env python
# -*- coding: utf-8 -*-

import multiprocessing
import time

def func(msg):
    print "msg:", msg
    time.sleep(3)
    print "end"
    return "done " + msg

if __name__ == "__main__":
    pool = multiprocessing.Pool(processes = 3)
    result = []
    for i in xrange(4):
        msg = "hello %d" % i
        result.append(pool.apply_async(func, (msg,)))

    print "Mark~ Mark~ Mark~~~~~~~~~~~~~~~~~~~~~~"
    pool.close()
    pool.join()
    for res in result:
        print "res: ", res.get()
    print "Sub-process(es) done."

