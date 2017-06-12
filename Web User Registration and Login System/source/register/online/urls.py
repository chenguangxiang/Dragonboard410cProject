from django.conf.urls import patterns, url
from online import views
import register.settings

 
urlpatterns = patterns('',
    url(r'^$', views.login, name='login'),
    url(r'^login/$',views.login,name = 'login'),
    url(r'^loginfail/$',views.login,name = 'loginfail'),
    url(r'^regist/$',views.regist,name = 'regist'),
    url(r'^registfaild/$',views.regist,name = 'regist'),
    url(r'^registfaild1/$',views.regist,name = 'regist'),
    url(r'^registsuccee/$',views.registsuccee,name = 'registsuccee'),
    url(r'^index/$',views.index,name = 'index'),
    url(r'^logout/$',views.logout,name = 'logout'),
    url(r'^static/(?P<path>.*)$', 'django.views.static.serve',{ 'document_root': register.settings.STATIC_URL }),
)