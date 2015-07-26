from django.shortcuts import render
from django.http import HttpResponse

# Create your views here.

def add(request):
    a = request.GET['a']
    b = request.GET['b']
    c = int(a) + int(b)
    return HttpResponse("%s + %s = %s" % (a, b, str(c)))

def add2(request, a, b):
    c = int(a) + int(b)
    return HttpResponse("%s + %s = %s" % (a, b, str(c)))

