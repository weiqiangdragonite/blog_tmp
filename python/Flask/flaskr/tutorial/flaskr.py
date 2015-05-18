#!/usr/bin/env python
# -*- coding: utf-8 -*-

# http://dormousehole.readthedocs.org/en/latest/tutorial/index.html

# all the imports
import sqlite3
from contextlib import closing
from flask import Flask, request, session, g, redirect, url_for, \
    abort, flash, render_template

# configuration
DATABASE = "/tmp/flasker.db"
# DEBUG 标志用于开关交互调试器。因为调试模式允许用户执行 服务器上的代码，所以
# 永远不要在生产环境中打开调试模式 ！
DEBUG = True
# secret_key （密钥）用于保持客户端会话安全，请谨慎地选择密钥，
# 并尽可能的使它复杂而且不容易被猜到。
SECRET_KEY = "development key"
USERNAME = "admin"
PASSWORD = "default"

# create our little application :)
app = Flask(__name__)
app.config.from_object(__name__)
# from_object() 会查看给定的对象（如果该对象是一个字符串就会 直接导入它），
# 搜索对象中所有变量名均为大字字母的变量。在我们的应用中，已经将配置写在前面了。
# 你可以把这些配置放到一个独立的文件中。
# 为了方便初学者学习，我们把库的导入与相关配置放在了一起。对于小型应用来说，
# 可以把配置直接放在模块中。但是更加清晰的方案是把配置放在一个独立的 .ini
# 或 .py 文件中，并在模块中导入配置的值。

# 通常，从一个配置文件中导入配置是比较好的做法，我们使用 from_envvar() 来完成
# 这个工作，把上面的 from_object() 一行替换为:
# app.config.from_envvar("FLASKR_SETTINGS", silent = True)
# 这样做就可以设置一个 FLASKR_SETTINGS 的环境变量来指定一个配置文件，并根据
# 该文件来重载缺省的配置。 silent 开关的作用是告诉 Flask 如果没有这个环境变量
# 不要报错。


# 添加了一个方便连接指定数据库的方法。这个方法可以用于在请求时打开连接
def connect_db():
    return sqlite3.connect(app.config["DATABASE"])

# 初始化数据库表，需要在 Python Shell 调用
# >>> from flaskr import init_db
# >>> init_db()
def init_db():
    # closing() 可以让你不用显示去调用关闭数据库，如果有异常发生，close()会
    # 在 with 块结束后自动调用，等价于 try: xxx finally: close()
    with closing(connect_db()) as db:
        # open_resource() 这个函数打开一个位于来源位置（你的 flaskr 文件夹）的
        # 文件并允许你读取文件的内容。这里我们用于在数据库连接上执行代码。
        with app.open_resource("schema.sql", mode = "r") as file:
            db.cursor().executescript(file.read())
        db.commit()

# 我们会在每一个函数中用到数据库连接，因此有必要在请求之前初始化连接，并在
# 请求之后关闭连接。
# Flask 中可以使用 before_request() 、 after_request() 和 teardown_request()
# 装饰器达到这个目的:
@app.before_request
def before_request():
    g.db = connect_db()

@app.teardown_request
def teardown_request(exception):
    db = getattr(g, "db", None)
    if db is not None:
        db.close()
    g.db.close()

# 使用 before_request() 装饰的函数会在请求之前调用，且不传递参数。使用
# after_request() 装饰的函数会在请求之后调用，且传递发送给客户端响应对象。
# 它们必须传递响应对象，所以在出错的情况下就不会执行。 因此我们就要用到
# teardown_request() 装饰器了。这个装饰器下 的函数在响应对象构建后被调用。
# 它们不允许修改请求，并且它们的返回值被忽略。如果请求过程中出错，那么这个
# 错误会传递给每个函数；否则传递 None 。
# 我们把数据库连接保存在 Flask 提供的特殊的 g 对象中。这个对象与每一个请求
# 是一一对应的，并且只在函数内部有效。不要在其它对象中储存类似信息， 
# 因为在多线程环境下无效。这个特殊的 g 对象会在后台神奇的工作，保证系统正常运行。


# 这个视图显示所有数据库中的条目。它绑定应用的根地址，并从数据库中读取
# title 和 text 字段。 id 最大的记录（最新的条目）在最上面。从指针返回的记录集
# 是一个包含 select 语句查询结果的元组。
@app.route("/")
def show_entries():
    cur = g.db.execute("SELECT title, text FROM entries ORDER BY id DESC")
    entries = [dict(title = row[0], text = row[1]) for row in cur.fetchall()]
    return render_template("show_entries.html", entries = entries)

@app.route("/add", methods = ["POST"])
def add_entry():
    if not session.get("logged_in"):
        abort(401)
    g.db.execute("INSERT INTO entries (title, text) VALUES (?, ?)",
                 [request.form["title"], request.form["text"]])
    g.db.commit()
    flash("New entry was successfully posted")
    return redirect(url_for("show_entries"))

@app.route("/login", methods = ["GET", "POST"])
def login():
    error = None
    if request.method == "POST":
        if request.form["username"] != app.config["USERNAME"]:
            error = "Invalid username"
        elif request.form["password"] != app.config["PASSWORD"]:
            error = "Invalid password"
        else:
            session["logged_in"] = True
            flash("You were logged in")
            return redirect(url_for("show_entries"))
    return render_template("login.html", error = error)

@app.route("/logout")
def logout():
    session.pop("logged_in", None)
    flash("You were logged out")
    return redirect(url_for("show_entries"))

# 启动服务器
if __name__ == "__main__":
    app.run(host = "0.0.0.0", debug = app.config["DEBUG"])
