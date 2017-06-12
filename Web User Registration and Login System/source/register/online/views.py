#coding=utf-8
from django.shortcuts import render,render_to_response
from django.http import HttpResponse,HttpResponseRedirect
from django.template import RequestContext
from django import forms
from online.models import User

#表单
class UserForm(forms.Form): 
    username = forms.CharField(label='用户名',max_length=100)
    password = forms.CharField(label='密码',widget=forms.PasswordInput())
 #   password1 = forms.CharField(label='重复密码',widget=forms.TextInput())
 #   loginaddr="http://127.0.0.1:8000/online/login/"
 #   registaddr="http://127.0.0.1:8000/online/regist/"

#注册
def regist(req):
    if req.method == 'POST':
        uf = UserForm(req.POST)
        if uf.is_valid():
            #获得表单数据
            username = uf.cleaned_data['username']
            password = uf.cleaned_data['password']
            password1 = req.POST['password1']
            #添加到数据库
            user = User.objects.filter(username__exact = username)
            if user:
                 return render_to_response('registfaild.html',{'uf':uf}, context_instance=RequestContext(req))
            else:
                if password1==password:
                     User.objects.create(username= username,password=password)
                     response = HttpResponseRedirect('/online/registsuccee/')
                     return response
                else:
                     return render_to_response('registfaild1.html',{'uf':uf}, context_instance=RequestContext(req))
    else:
        uf = UserForm()
    return render_to_response('regist.html',{'uf':uf}, context_instance=RequestContext(req))

#登陆
def login(req):
    if req.method == 'POST':
        uf = UserForm(req.POST)
        if uf.is_valid():
                #获取表单用户密码
            username = uf.cleaned_data['username']
            password = uf.cleaned_data['password']

                #获取的表单数据与数据库进行比较
            user = User.objects.filter(username__exact = username,password__exact = password)
            if user:
                #比较成功，跳转index
                response = HttpResponseRedirect('/online/index/')
                #将username写入浏览器cookie,失效时间为3600
                response.set_cookie('username',username,3600)
                return response
            else:
                 #比较失败，跳转loginfaild
                return render_to_response('loginfaild.html',{'uf':uf},context_instance=RequestContext(req))
    else:
        uf = UserForm()
    return render_to_response('login.html',{'uf':uf},context_instance=RequestContext(req))



#def loginfaild(req):
#    response = login(req)
#    return response
#
def registsuccee(req):
   if req.method == 'POST':
        uf = UserForm(req.POST)
        if uf.is_valid():
                #获取表单用户密码
            username = uf.cleaned_data['username']
            password = uf.cleaned_data['password']
                #获取的表单数据与数据库进行比较
            user = User.objects.filter(username__exact = username,password__exact = password)
            if user:
                #比较成功，跳转index
                response = HttpResponseRedirect('/online/index/')
                #将username写入浏览器cookie,失效时间为3600
                response.set_cookie('username',username,3600)
                return response
            else:
                 #比较失败，跳转loginfaild
                return render_to_response('loginfaild.html',{'uf':uf},context_instance=RequestContext(req))
   else:
        uf = UserForm()
        #return render_to_response('registfaild.html',{'uf':uf},context_instance=RequestContext(req))
   return render_to_response('registsuccee.html',{'uf':uf},context_instance=RequestContext(req))
#
#def registfaild(req):
#    response = regist(req)
#    return response
#
#def registfaild1(req):
#    response = regist(req)
#    return response


#登陆成功
def index(req):
    username = req.COOKIES.get('username','')
    return render_to_response('loginsuccee.html' ,{'username':username})

#退出
def logout(req):
    response = login(req)
    response.delete_cookie('username')
    return response
    #response = HttpResponse('logout !!')
    #清理cookie里保存username
   # response.delete_cookie('username')
   # return response