# Generated by Gareth Owen <gaz@athene.co.uk>

CC = gcc
CFLAGS = -g -O2               //aga:传递给编译器的编译参数，这里做变量替换 -O表示在编译时优化，级别最高位O3，-g表示产生debug的信息，
                              //使用-g+-O的做法可以更好的优化代码，例如未使用的变量会被消除，控制流会被精简等等
LDFLAGS =  					  //指定静态链接库的位置
LIBS = 						  //也是指定编译时需要链接的库文件，但是生成目标文件时不需要添加这些库文件
DESTDIR = /usr/local/lhttpd   //常量定义，指定最终生成的项目所在的目录为/usr/local/lhttpd

OBJ = main.o protocol.o util.o //常量定义，指定最后的生成obj文件

default: lhttpd

lhttpd: $(OBJ)                                //make命令，lhttpd是目标文件，$(OBJ)是目标所依赖的文件
	$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)     // gcc -g -O2 -o $@ main.o protocol.o util.o


install: lhttpd                               //make install时执行这里，make clean时执行下面那块
	mkdir -p $(DESTDIR)                       //在指定绝对路径的情况下，p帮助建立路径中的所有目录
	mkdir -p $(DESTDIR)/cgi-bin               //这里到时候放什么？？cgi文件
	mkdir -p $(DESTDIR)/htdocs
	cp -f lhttpd $(DESTDIR)/                  //f强制复制 lhttpd 到 目标目录下
	cp -f 404.html $(DESTDIR)/
	cp -f cgierror.html $(DESTDIR)/
	cp -f cmderror.html $(DESTDIR)/           //错误html都直接放在 顶层目录下
	cp -f lhttpd.conf /etc/                   //配置文件都需要放在etc
	cp -f index.html $(DESTDIR)/htdocs/       //index放在htdocs下，该目录存放html等资源文件
	cp -f light $(DESTDIR)/                   //light是一个脚本，用来根据参数 启动服务器 重启服务器 关闭服务器
	chmod +x $(DESTDIR)/light                 //赋予可执行的权限
	chown nobody.nobody $(DESTDIR)            //自定义用户的管理权限
	chown nobody.nobody $(DESTDIR)/*
	chown nobody.nobody $(DESTDIR)/htdocs/*

clean:
	rm -f *.o lhttpd

