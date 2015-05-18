#!/usr/bin/env python
# -*- coding: utf-8 -*-

from django.contrib import admin

# Register your models here.

from mysite.books.models import Publisher, Author, Book

# 自定义列表
class AuthorAdmin(admin.ModelAdmin):
    list_display = ('first_name', 'last_name', 'email')
    search_fields = ('first_name', 'last_name')

class BookAdmin(admin.ModelAdmin):
    list_display = ('title', 'publisher', 'publication_date')
    # list_filter这个字段元组创建过滤器，注意是tuple
    list_filter = ('publication_date', 'publisher')
    # 另外一种过滤日期的方式
    date_hierarchy = 'publication_date'
    ordering = ('-publication_date',)
    # 默认地，表单中的字段顺序是与模块中定义是一致的。 我们可以通过使用
    # ModelAdmin子类中的fields选项来改变它：
    fields = ('title', 'authors', 'publisher', 'publication_date')
    # 针对多对多字段
    filter_horizontal = ('authors',)
    # 默认地，管理工具使用`` 下拉框`` 来展现`` 外键`` 字段。
    # 如果下拉框记录太多的话，可以使用
    raw_id_fields = ('publisher',)


admin.site.register(Publisher)
admin.site.register(Author, AuthorAdmin)
admin.site.register(Book, BookAdmin)

