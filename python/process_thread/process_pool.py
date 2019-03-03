#!/usr/bin/env python
# -*- coding: utf-8 -*-

import multiprocessing
import random

NUM = 100000000

def func(msg):
    res_0 = 0
    res_1 = 0
    random.seed()
    for i in xrange(NUM):
        n = random.randint(0,1)
        #print msg + "[%d]" % n
        if n == 0:
            res_0 += 1
        else:
            res_1 += 1
    print msg + "0: [%d], 1: [%d]" % (res_0, res_1)
    return {"0": res_0, "1": res_1}


if __name__ == "__main__":
    pool = multiprocessing.Pool(processes = 2)
    result = []
    for i in xrange(2):
        msg = "p%d: " % (i)
        result.append(pool.apply_async(func, (msg,)))
    pool.close()
    pool.join()
    dig_num = {"0": 0, "1": 0, "res": 0}
    for res in result:
        #print ":::", res.get()
        dig_num["0"] += res.get()["0"]
        dig_num["1"] += res.get()["1"]
    if dig_num["0"] >= dig_num["1"]:
        dig_num["res"] = float(dig_num["1"]) / dig_num["0"]
    else:
        dig_num["res"] = float(dig_num["0"]) / dig_num["1"]
    print dig_num
    print "done"
