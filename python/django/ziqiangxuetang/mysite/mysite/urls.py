from django.conf.urls import patterns, include, url

from django.contrib import admin
admin.autodiscover()

urlpatterns = patterns('',
    # Examples:
    # url(r'^$', 'mysite.views.home', name='home'),
    # url(r'^blog/', include('blog.urls')),

    url(r'^admin/', include(admin.site.urls)),

    # my define
    url(r'^$', 'learn.views.index', name='index'),
    url(r'^home/$', 'learn.views.home', name='home'),
    url(r'^add/$', 'calc.views.add', name='add'),
    url(r'^add/(\d+)/(\d+)/', 'calc.views.add2', name='add2'),
)
