#!/usr/bin/env python
# -*- coding: utf-8 -*-

from django.db import models

# Create your models here.
# 添加号数据库类后，到settings设置，然后就可以创建数据库表了
# 用下面的命令验证模型的有效性:
# python manage.py validate
# 模型确认没问题了，运行下面的命令来生成SQL语句:
# python manage.py sqlall books
# 提交SQL语句到数据库:
# python manage.py syncdb

"""
一个作者有姓，有名及email地址。
出版商有名称，地址，所在城市、省，国家，网站。
书籍有书名和出版日期。 它有一个或多个作者（和作者是多对多的关联关系
[many-to-many]）， 只有一个出版商（和出版商是一对多的关联关系[one-to-many]，
也被称作外键[foreign key]）
"""

# 每个数据模型都是 django.db.models.Model 的子类
# 每个模型相当于单个数据库表，每个属性也是这个表中的一个字段。
class Publisher(models.Model):
    name = models.CharField(max_length = 30)
    address = models.CharField(max_length = 50)
    city = models.CharField(max_length = 60)
    state_province = models.CharField(max_length = 30)
    country = models.CharField(max_length = 50)
    website = models.URLField()

    # 添加打印对象时的格式
    # __unicode__() 方法可以进行任何处理来返回对一个对象的字符串表示
    def __unicode__(self):
        return self.name

    # 使用 Django 的数据库 API 去检索时，Publisher对象的相关返回值
    # 默认地都会按 name 字段排序
    class Meta:
        ordering = ['name']

class Author(models.Model):
    first_name = models.CharField(max_length = 30)
    last_name = models.CharField(max_length = 40)
    # 可选字段
    # 自定义标签
    email = models.EmailField(blank = True, verbose_name = "e-mail")

    def __unicode__(self):
        return u"%s %s" % (self.first_name, self.last_name)

# "每个数据库表对应一个类"这条规则的例外情况是多对多关系。 
class Book(models.Model):
    title = models.CharField(max_length = 100)
    # 字段允许为NULL --> null = True
    publication_date = models.DateField(blank = True)
    authors = models.ManyToManyField(Author)
    publisher = models.ForeignKey(Publisher)

    def __unicode__(self):
        return self.title


