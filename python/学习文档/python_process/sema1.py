#!/usr/bin/env python
# -*- coding: utf-8 -*-

import multiprocessing
import time
import random

def worker(s, i):
    name = multiprocessing.current_process().name
    s.acquire()
    print "%s acquire" % name
    time.sleep(i)
    print "%s release" % name
    s.release()

if __name__ == "__main__":
    s = multiprocessing.Semaphore(2)
    for i in range(5):
        p = multiprocessing.Process(target = worker, args = (s, random.randint(1, 5)))
        p.start()

