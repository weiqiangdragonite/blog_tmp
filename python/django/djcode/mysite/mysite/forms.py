#!/usr/bin/env python
# -*- coding: utf-8 -*-

from django import forms


#
class ContactForm(forms.Form):
    subject = forms.CharField(max_length = 100)
    # 自定义标签 label，和 model的那个 verbose_name 一样
    email = forms.EmailField(required = False, label = "Your E-Mail Address")
    message = forms.CharField(widget = forms.Textarea)

    # 自定义校验规则
    # Django的form系统自动寻找匹配的函数方法，该方法名称以clean_开头，
    # 并以字段名称结束。 如果有这样的方法，它将在校验时被调用。
    def clean_message(self):
        message = self.cleaned_data["message"]
        num_words = len(message.split())
        if num_words < 4:
            raise forms.ValidationError("Not enough words!")
        return message

# register
class RegisterForm(forms.Form):
    username = forms.CharField()
    email = forms.CharField()
    password = forms.CharField(widget = forms.PasswordInput)
    password2 = forms.CharField(widget = forms.PasswordInput)

    # 自定义验证输入的两次密码


# login
class LoginForm(forms.Form):
    username = forms.CharField();
    password = forms.CharField(widget = forms.PasswordInput)




