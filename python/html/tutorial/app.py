#!/usr/bin/env python
# -*- coding: utf-8 -*-

# http://www.revillweb.com/tutorials/bootstrap-tutorial/

from flask import Flask, request, session, g, redirect, url_for, \
    abort, flash, render_template

app = Flask(__name__)


#
@app.route("/")
def index():
    return render_template("index.html")

@app.route("/browser")
def browser():
    user_agent = request.headers.get("User-Agent")
    return "<p>Your browser is %s</p>" % user_agent

@app.route("/login", methods = ["GET", "POST"])
def login():
    if request.method == "POST":
        return "POST"
    elif request.method == "GET":
        return "GET"



# 启动服务器
if __name__ == "__main__":
    app.run(host = "0.0.0.0", debug = True)
