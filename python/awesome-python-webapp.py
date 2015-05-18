
Day 1 - 搭建开发环境
http://www.liaoxuefeng.com/wiki/001374738125095c955c1e6d8bb493182103fac9270762a000/0013976177048818eb4187c05a84f9280169d58e22afa09000


	1.安装Python
	2.安装前端模板引擎jinja2
	yum install python-pip
	pip install Jinja2
	(或 easy_install Jinja2)
	3.安装数据库(这里我使用MariaDB)
	yum install mariadb mariadb-server mariadb-common mariadb-devel mariadb-libs mariadb-connect-engine
	使用MariaDB还要安装Python的MySQL驱动
	yum install mysql-connector-python MySQL-python
	( mysql-connector-python：是MySQL官方的纯Python驱动
	  MySQL-python：是封装了MySQL C驱动的Python驱动 )


	项目结构
	awesome-python-webapp/   <-- 根目录
	|
	+- backup/               <-- 备份目录
	|
	+- conf/                 <-- 配置文件
	|
	+- pack/                 <-- 打包目录
	|
	+- www/                  <-- Web目录，存放.py文件
	|  |
	|  +- static/            <-- 存放静态文件
	|  |
	|  +- templates/         <-- 存放模板文件
	|
	+- LICENSE               <-- 代码LICENSE


	在GitHub上建立仓库(原文打包目录名是 dist ，但不知为啥我用 dist 的话，git不会
	有记录，所以改成 pack 了)


--------------------------------------------------------------------------------

Day 2 - 编写数据库模块
http://www.liaoxuefeng.com/wiki/001374738125095c955c1e6d8bb493182103fac9270762a000/0013976160374750f95bd09087744569be5aae6160c8351000


我们决定自己设计一个封装基本的SELECT、INSERT、UPDATE和DELETE操作
的db模块：transwarp.db。

设计db接口
	设计底层模块的原则是，根据上层调用者设计简单易用的API接口，然后，
	实现模块内部代码。
	假设transwarp.db模块已经编写完毕，我们希望以这样的方式来调用它：

	首先，初始化数据库连接信息，通过create_engine()函数：
	from transwarp import db
	db.create_engine(user='root', password='password', database='test', host='127.0.0.1', port=3306)

	然后，就可以直接操作SQL了。
	如果需要做一个查询，可以直接调用select()方法，返回的是list，每一个元素
	是用dict表示的对应的行：
	users = db.select('select * from user')
	# users =>
	# [
	#     { "id": 1, "name": "Michael"},
	#     { "id": 2, "name": "Bob"},
	#     { "id": 3, "name": "Adam"}
	# ]

	如果要执行INSERT、UPDATE或DELETE操作，执行update()方法，返回受影响的行数：
	n = db.update('insert into user(id, name) values(?, ?)', 4, 'Jack')

	update()函数签名为：
	update(sql, *args)

	统一用?作为占位符，并传入可变参数来绑定，从根本上避免SQL注入攻击。
	每个select()或update()调用，都隐含地自动打开并关闭了数据库连接，这样，
	上层调用者就完全不必关心数据库底层连接。


	关于函数参数里的 *args 和 **kwargs ，*args 表示任何多个无名参数，它是
	一个 tuple；**kwargs 表示关键字参数，它是一个 dict。并且同时使用
	*args 和 **kwargs 时，*args 参数列必须要在 **kwargs 前。
	例子：
	def foo(*args, **kwargs):
		print 'args =', args
		print 'kwargs = ', kwargs
		print '-----------------------'

	if __name__ == '__main__':
		foo(1, 2, 3, 4)
		foo(a=1, b=2, c=3)
		foo(1,2,3,4, a=1, b=2, c=3)
		foo('a', 1, None, a=1, b='2', c=3)

	输出结果：
	args = (1, 2, 3, 4)
	kwargs =  {}
	-----------------------
	args = ()
	kwargs =  {'a': 1, 'c': 3, 'b': 2}
	-----------------------
	args = (1, 2, 3, 4)
	kwargs =  {'a': 1, 'c': 3, 'b': 2}
	-----------------------
	args = ('a', 1, None)
	kwargs =  {'a': 1, 'c': 3, 'b': '2'}
	-----------------------


	回到教程
	但是，如果要在一个数据库连接里执行多个SQL语句怎么办？我们
	用一个with语句实现：
	with db.connection():
		db.select('...')
		db.update('...')
		db.update('...')


	如果要在一个数据库事务中执行多个SQL语句怎么办？我们还是用一个with语句实现：
	with db.transaction():
		db.select('...')
		db.update('...')
		db.update('...')

	with 是对 try ... except ... finally 语法的一种简化

实现db模块
	由于模块是全局对象，模块变量是全局唯一变量，所以，有两个重要的模块变量：

	# db.py

	# 数据库引擎对象：
	class _Engine(object): # _xxx 不能用于 from module import * 导入
		def __init__(self, connect):    # __xxx__ 表示系统定义名字 __init__ 是初始化
			self._connect = connect # __var 表示私有变量； _var 表示"虽然我可以被访问，但是，请把我视为私有变量，不要随意访问"
		def connect(self):
			return self._connect()

	engine = None

	# 持有数据库连接的上下文对象：
	class _DbCtx(threading.local):
		def __init__(self):
			self.connection = None
			self.transactions = 0

		def is_init(self):
			return not self.connection is None

		def init(self):
			self.connection = _LasyConnection()
			self.transaction = 0

		def cleanup(self):
			self.connection.cleanup()
			self.connection = None

	_db_ctx = _DbCtx()

	由于_db_ctx是 threadlocal 对象，所以，它持有的数据库连接对于每个线程
	看到的都是不一样的。任何一个线程都无法访问到其他线程持有的数据库连接。
	(ThreadLocal 最常用的地方就是为每个线程绑定一个数据库连接，HTTP请求，
	用户身份信息等，这样一个线程的所有调用到的处理函数都可以非常方便地访问
	这些资源。)

	有了这两个全局变量，我们继续实现数据库连接的上下文，目的是自动获取和
	释放连接：
	class _ConnectionCtx(object):
		def __enter__(self):
			gloabl _db_ctx
			self.should_cleanup = False
			if not _db_ctx.is_init():
				_db_ctx.init()
				self.should_cleanup = True
			return self

		def __exit__(self, exctype, excvalue, traceback):
			global _db_ctx
			if self.should_cleanup:
				_db_ctx.cleanup()

		def connection():
			return _ConnectionCtx()

	定义了__enter__()和__exit__()的对象可以用于with语句，确保任何情况下
	__exit__()方法可以被调用。

	把_ConnectionCtx的作用域作用到一个函数调用上，可以这么写：
	with connection():
		do_some_db_operation()

	with通过__enter__方法初始化，然后在__exit__中做善后以及处理异常。对于
	一些需要预先设置，事后要清理的一些任务，with提供了一种非常方便的表达。























