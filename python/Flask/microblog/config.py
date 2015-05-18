#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os

CSRF_ENABLED = True
SECRET_KEY = 'you-will-never-guess'

OPENID_PROVIDERS = [
    { "name": "Google", "url": "https://www.google.com/accounts/o8/id" },
    { "name": "Yahoo", "url": "https://me.yahoo.com" },
    { "name": "AOL", "url": "http://openid.aol.com/<username>" },
    { "name": "Flickr", "url": "http://www.flickr.com/<username>" },
    { "name": "MyOpenID", "url": "https://www.myopenid.com" }]

# 获取当前脚本文件路径
basedir = os.path.abspath(os.path.dirname(__file__))

SQLALCHEMY_DATABASE_URI = 'sqlite:///' + os.path.join(basedir, 'app.db')
# 用来存储SQLAlchemy-migrate数据库文件的文件夹
SQLALCHEMY_MIGRATE_REPO = os.path.join(basedir, 'db_repository')


# mail server settings
MAIL_SERVER = "smtp.163.com"
MAIL_PORT = 25
MAIL_USERNAME = "meiguang_code@163.com"
MAIL_PASSWORD = "meiguang@123"

# administrator list
ADMINS = ["1654088319@qq.com"]

