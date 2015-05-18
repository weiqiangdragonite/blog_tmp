#!/usr/bin/env python
# -*- coding: utf-8 -*-

from django.http import HttpResponse
from django.template.loader import get_template
from django.template import Context
from django.shortcuts import render_to_response
import datetime
from mysite.forms import LoginForm
from django.template import RequestContext  # 这个是用于 csrf 的


# 每个视图函数至少要有一个参数，通常被叫作request。
# 这是一个触发这个视图、包含当前Web请求信息的对象，是
# 类django.http.HttpRequest的一个实例。
# 这个函数第一个参数的类型是HttpRequest；它返回一个HttpResponse实例。
def hello(request):
    return HttpResponse("Hello World")

# 网站根目录
def homepage(request):
    return HttpResponse("This is Home Page")

# 返回日期时间
def current_datetime(request):
    """
    Django是有时区意识的，并且默认时区为America/Chicago
    需要在settings.py文件中更改这个值
    """
    now = datetime.datetime.now()
    html = "It is now %s." % now
    return HttpResponse(html)

# 使用模板
def datetime_template(request):
    now = datetime.datetime.now()

    # 下面3句请使用render_to_response()替代
    #t = get_template("current_datetime.html")
    #html = t.render(Context({"current_date": now}))
    #return HttpResponse(html)

    # 使用render_to_response()代替上面3句
    # 我们不再需要导入 get_template 、 Template 、 Context 和 HttpResponse
    # 相反，我们导入 django.shortcuts.render_to_response
    return render_to_response('current_datetime.html', {'current_date': now})
    # 把模板存放于模板目录的子目录中是件很轻松的事情
    #return render_to_response('dateapp/current_datetime.html', {'current_date': now})
    """
    locals() 技巧
    利用 Python 的内建函数 locals() 。它返回的字典对所有局部变量的名称与
    值进行映射。 因此，前面的视图可以重写成下面这个样子
    current_date = datetime.datetime.now()
    return render_to_response('current_datetime.html', locals())
    """

#  每一个视图 总是 以一个 HttpRequest 对象作为 它的第一个参数
# offset 是从匹配的URL里提取出来的。 例如：如果请求URL是/time/plus/3/，
# 那么offset将会是3。请注意：捕获值永远都是字符串（string）类型
def hours_ahead(request, offset):
    # 如果你在一个不能转换成整数类型的值上调用int()，Python将抛出一个ValueError异常
    try:
        offset = int(offset)
    except ValueError:
        # 转为抛出django.http.Http404异常——正如你想象的那样：
        # 最终显示404页面（提示信息：页面不存在）。
        raise Http404()
    # 用hours = offset是应该为了指定hours（小时）这个参数
    dt = datetime.datetime.now() + datetime.timedelta(hours=offset)
    html = "In %s hour(s), it will be %s." % (offset, dt)
    return HttpResponse(html)


# URL相信 和 Request信息
def display_request_info(request):
    html = []

    path = request.path
    host = request.get_host()
    full_path = request.get_full_path()
    is_secure = request.is_secure()

    html.append("Path: %s </br>" % path)
    html.append("Host: %s </br>" % host)
    html.append("Full Path: %s </br>" % full_path)

    values = request.META.items()
    values.sort()
    for k, v in values:
        html.append("%s --> %s </br>" % (k, v))

    return HttpResponse(html)


# 表单处理 见 books/views.py

#来一个模拟登录的
def login(request):
    errors = []
    if request.method == "POST":
        form = LoginForm(request.POST)
        if form.is_valid():
            data = form.cleaned_data
            username = data["username"]
            password = data["password"]
            if login_validate(username, password):
                return render_to_response("welcome.html", { "user": username })
            else:
                errors.append("Username or password is incorrect.")
        else:
            errors.append("Please input both username nad password.")
    else:
        form = LoginForm()
    return render_to_response("login_form.html", { "form": form , "errors": errors }, \
        context_instance=RequestContext(request))

login

def login_validate(username, password):
    if username != "admin":
        return False;
    if password != "admin":
        return False;
    return True;


