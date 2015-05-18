#!/usr/bin/env python
# -*- coding: utf-8 -*-

from app import app, db, lm, oid
from flask import render_template, flash, redirect, session, url_for, request, g
from flask.ext.login import login_user, logout_user, current_user, login_required
from forms import LoginForm, EditForm
from models import User, ROLE_USER, ROLE_ADMIN
from datetime import datetime

@app.route("/")
@app.route("/index")
@login_required
def index():
    # 我们增加了login_required装饰器。这样表明了这个页面只有登录用户才能访问
    # 把g.user传给了模板
    usr = g.user
    posts = [
        {
            "author": { "nickname": "John" },
            "body": "Beautiful day in Portland!"
        },
        {
            "author": { "nickname": "Susan" },
            "body": "The Avengers movie was so cool!"
        }
    ]
    return render_template("index.html",
        title = "Home",
        user = usr,
        posts = posts)


@app.route("/login", methods = ["GET", "POST"])
@oid.loginhandler
def login():
    # 检测是是否用户是已经经过登录认证的
    # 全局变量g是Flask设置的，在一个request生命周期中，用来存储和共享数据的变量。
    # 所以我猜你已经想到了，我们将把已经登录的用户放到g变量里。
    if g.user is not None and g.user.is_authenticated():
        # 我们有很好的理由让Flask为你构造url
        # http://flask.pocoo.org/docs/quickstart/#url-building
        return redirect(url_for(index))

    form = LoginForm()
    if form.validate_on_submit():
        # 一旦数据被保存到session中，它将在同一客户端发起的这次请求和这次以后的
        # 请求中永存而不会消亡。数据将保持在session中直到被明确的移除。为了做到
        # 这些，Flask为每个客户端建立各自的session。
        session["remember_me"] = form.remember_me.data
        # oid.try_login是通过Flask-OpenID来执行用户认证
        # Flask-OpenID will call a function that is registered with the
        # oid.after_login decorator if the authentication is successful. If the
        # authentication fails the user will be taken back to the login page.
        return oid.try_login(form.openid.data, ask_for = ["nickname", "email"])
    return render_template("login.html",
        title = "Sigin In",
        form = form,
        providers = app.config["OPENID_PROVIDERS"])

@app.route("/logout")
def logout():
    logout_user()
    return redirect(url_for("index"))

# 用户加载回调
# First, we have to write a function that loads a user from the database. 
# This function will be used by Flask-Login
@lm.user_loader
def load_user(id):
    # 记住Flask-Login里的user id一直是unicode类型的
    return User.query.get(int(id))

# 使用Flask提供的before_request事件。任何一个被before_request装饰器装饰的方法将
# 会在每次request请求被收到时提前与view方法执行
# Any functions that are decorated with before_request will run before the view
# function each time a request is received.
@app.before_request
def before_request():
    g.user = current_user
    if g.user.is_authenticated():
        g.user.last_seen = datetime.utcnow()
        db.session.add(g.user)
        db.session.commit()

# Flask-OpenID登录回调
@oid.after_login
def after_login(resp):
    # 传给after_login方法的resp参数包含了OpenID provider返回的一些信息
    # 第一个if声明仅仅是为了验证。我们要求一个有效的email，所以一个没有没
    # 提供的email我们是没法让他登录的。
    if resp.email is None or resp.email == "":
        flash("Invalid login. Please try again.")
        redirect(url_for("login"))
    # 如果email没有被找到我们就认为这是一个新的用户，所以我们将在数据库中增加
    # 一个新用户
    user = User.query.filter_by(email = resp.email).first()
    if user is None:
        nickname = resp.nickname
        if nickname is None or nickname == "":
            nickname = resp.email.split("@")[0]
        nickname = User.make_unique_nickname(nickname)
        user = User(nickname = nickname, email = resp.email, role = ROLE_USER)
        db.session.add(user)
        db.session.commit()
    remember_me = False
    # 从Flask session中获取remember_me的值，如果它存在，那它是我们之前
    # 在login view方法中保存到session中的boolean类型的值。
    if "remember_me" in session:
        remember_me = session["remember_me"]
        session.pop("remember_me", None)
    # 调用Flask-Login的login_user方法，来注册这个有效的登录
    login_user(user, remember = remember_me)
    # 在最后一行我们重定向到下一个页面，或者如果在request请求中没有提供下个
    # 页面时，我们将重定向到index页面。
    return redirect(request.args.get("next") or url_for("index"))
"""
跳转到下一页的这个概念很简单。比方说我们需要你登录才能导航到一个页面，但你现在
并未登录。在Flask-Login中你可以通过login_required装饰器来限定未登录用户。
如果一个用户想连接到一个限定的url，那么他将被自动的重定向到login页面。
Flask-Login将保存最初的url作为下一个页面，一旦登录完成我们便跳转到这个页面。
"""


@app.route("/user/<nickname>")
@login_required
def user(nickname):
    user = User.query.filter_by(nickname = nickname).first()
    if user == "None":
        flash("不存在用户: " + nickname + "!")
        return redirect(url_for("index"))
    posts = [
        { "author": user, "body": "Test post #1" },
        { "author": user, "body": "Test post #2" }]
    return render_template("user.html", user = user, posts = posts)


@app.route("/edit", methods = ["GET", "POST"])
@login_required
def edit():
    form = EditForm(g.user.nickname)
    if form.validate_on_submit():
        g.user.nickname = form.nickname.data
        g.user.about_me = form.about_me.data
        db.session.add(g.user)
        db.session.commit()
        flash("Your changes have been saved.")
        return redirect(url_for("edit"))
    else:
        form.nickname.data = g.user.nickname
        form.about_me.data = g.user.about_me
    return render_template("edit.html", form = form)


@app.errorhandler(404)
def internal_error(error):
    return render_template("404.html"), 404

@app.errorhandler(500)
def internal_error(error):
    # If the exception was triggered by a database error then the database
    # session is going to arrive in an invalid state, so we have to roll it
    # back in case a working session is needed for the rendering of the template
    # for the 500 error.
    db.session.rollback()
    return render_template("500.html"), 500


