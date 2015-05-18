#!/usr/bin/env python
# -*- coding: utf-8 -*-

from django.shortcuts import render


# Create your views here.
from mysite.books.models import Publisher, Author, Book
from django.http import HttpResponse
from django.http import HttpResponseRedirect
from django.shortcuts import render_to_response
from django.core.mail import send_mail
from django.template import RequestContext  # 这个是用于 csrf 的
from mysite.forms import ContactForm


# 表单处理
# 这个view函数可以放到Python的搜索路径的任何位置。
def search_form(request):
    return render_to_response("search_form.html")

#
def search(request):
    #if 'q' in request.GET:
    #    message = 'You searched for: %r' % request.GET['q']
    #else:
    #    message = "You submitted an empty form"
    #return HttpResponse(message)

    errors = []
    if 'q' in request.GET:
        q = request.GET['q']
        if not q:
            errors.append("Enter a search term.")
        elif len(q) > 20:
            errors.append("Please enter at most 20 characters.")
        else:
            # icontains是一个查询关键字，可以理解为获取标题里包含q的书籍，不区分大小写
            books = Book.objects.filter(title__icontains = q)
            return render_to_response('search_results.html',
                { 'books': books, 'query': q })
    return render_to_response("search_form.html", { 'errors': errors })


#
"""
contact()视图是否要放在books/views.py这个文件里。 但是contact()视图与books应用
没有任何关联，那么这个视图应该可以放在别的地方？ 这毫无紧要，只要在URLconf里
正确设置URL与视图之间的映射，Django会正确处理的。 笔者个人喜欢创建一个contact的
文件夹，与books文件夹同级。这个文件夹中包括空的__init__.py和views.py两个文件。
"""
def contact(request):
    errors = []
    if request.method == "POST":
        # 使用request.POST.get()方法，并提供一个空的字符串作为默认值；
        if not request.POST.get("subject", ""):
            errors.append("Please enter a subject.")
        if not request.POST.get("message", ""):
            errors.append("Please enter a message.")
        # 邮箱非必须的
        if request.POST.get("email") and "@" not in request.POST["email"]:
            errors.append("Please enter a valid e-mail address")

        if not errors:
            # 使用了django.core.mail.send_mail函数来发送e-mail。 
            # 这个函数有四个必选参数： 主题，正文，寄信人和收件人列表。
            # send_mail('Subject here', 'Here is the message.', 'from@example.com',
            # ['to@example.com'], fail_silently=False)
            # 要发送邮件，请先在settings里设置
            # 注意，我认为 request.POST.get("email") 是多余的
            # 发送者的邮箱必须和settings的邮箱一致！
            send_mail(
                request.POST["subject"],
                request.POST["message"],
                request.POST.get("email", "meiguang_code@163.com"),
                ["1654088319@qq.com"],
                fail_silently = False
            )
            """
            若用户刷新一个包含POST表单的页面，那么请求将会重新发送造成重复。 这
            通常会造成非期望的结果，比如说重复的数据库记录；在我们的例子中，将
            导致发送两封同样的邮件。 如果用户在POST表单之后被重定向至另外的页面，
            就不会造成重复的请求了。我们应每次都给成功的POST请求做重定向。 
            这就是web开发的最佳实践。
            """
            # /contact/thanks -> 是地址哟，所以要到url注册哦
            return HttpResponseRedirect("/contact/thanks")
    return render_to_response("contact_form.html", \
        { "errors": errors, \
          "subject": request.POST.get("subject", ""), \
          "message": request.POST.get("message", ""),\
          "email": request.POST.get("email", "") }, \
        # 下面这个参数是用于 csrf 的
        context_instance=RequestContext(request))

def thanks(request):
    return HttpResponse("Thanks")

# use django form
def new_contact(request):
    if request.method == "POST":
        form = ContactForm(request.POST)
        # is_valid() 校验数据
        if form.is_valid():
            # 如果一个Form实体的数据是合法的，它就会有一个可用的cleaned_data属性。 
            # 这是一个包含干净的提交数据的字典。 Django的form框架不但校验数据，
            # 它还会把它们转换成相应的Python类型数据，这叫做清理数据。
            cd = form.cleaned_data
            send_mail(cd["subject"], cd["message"], \
                cd.get("email", "meiguang_code@163.com"), ["1654088319@qq.com"],)
            return HttpResponseRedirect("/contact/thanks/")
    else:
        # 设置初始值
        form = ContactForm(initial={ "subject": "I love your site!" })
    return render_to_response("new_contact_form.html", { "form": form }, \
        context_instance=RequestContext(request))





