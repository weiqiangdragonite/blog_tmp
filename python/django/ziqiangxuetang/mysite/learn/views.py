#!/usr/bin/env python
# -*- coding: utf-8 -*-

from django.shortcuts import render
from django.http import HttpResponse

# Create your views here.

def index(request):
    return HttpResponse(u"欢迎光临 自强学堂!")

def home(request):
    str = u"我在自强学堂学习Django，用它来建网站"
    return render(request, 'home.html', {'string':str})

