#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import logging


def init_logger(name = "default", level = "DEBUG"):
    logger = logging.getLogger(name)
    formatter = logging.Formatter(
        "[%(asctime)s - %(name)s - %(levelname)s]: %(message)s")
    file_handler = logging.FileHandler("/tmp/%s.log" % name)
    file_handler.setFormatter(formatter)
    logger.addHandler(file_handler)
    logger.setLevel(logging.DEBUG)
    return logger

#    logger.debug('this is debug info')
#    logger.info('this is information')
#    logger.warn('this is warning message')
#    logger.error('this is error message')
#    logger.fatal('this is fatal message, it is same as logger.critical')
#    logger.critical('this is critical message')


def init_console_logger(name = "default"):
    logger = logging.getLogger(name)
    formatter = logging.Formatter(
        "[%(asctime)s - %(name)s - %(levelname)s]: %(message)s")
    console_handler = logging.StreamHandler()
    console_handler.setFormatter(formatter)
    logger.addHandler(console_handler)
    logger.setLevel(logging.DEBUG)
    return logger
    


if __name__ == "__main__":
    #logging.basicConfig(
    #    level = logging.WARNING,
    #    format = "[%(asctime)s - %(name)s - %(levelname)s]: %(message)s",
    #    #datefmt = "%Y-%m-%d %H:%M:%S",
    #    filename = "/tmp/example.log",
    #    filemode = 'a'
    #)

    #logging.debug("this is debug")
    #logging.info("this is info")
    #logging.warning("this is warning")
    #logging.error("this is error")
    #logging.critical("this is critical")
    
    test_logger = init_logger("test")
    exam_logger = init_logger("exam")
    cons_logger = init_console_logger("cons")

    test_logger.debug("this is test logger DEBUG")
    exam_logger.debug("this is exam logger DEBUG")
    cons_logger.warning("this is console warning logger")
