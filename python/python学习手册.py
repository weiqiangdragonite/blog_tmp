
参考书籍主要是
Think Python - How to Think Like a Computer Scientist
Python Essential Reference
http://learnpythonthehardway.org/

请使用"notepad++"打开此文档,"alt+0"将函数折叠后方便查阅

编写Python程序，社区更推荐使用space来缩进

--------------------------------------------------------------------------------

第一章

1.Running Python

	Python 2 和 Python 3 有部分语法不同，主要是IO方面。
	Python是解释型语言，IDE有Spyder(需要Qt)、IDLE(官方自带)、PyCharm(需要JAVA)
	Python在UNIX/Linux下可直接使用交互式命令行，在终端下输入python或python3
	即可进入，输入exit()即可退出。

	Python2:
	>>> print "hello, world"
	Python3:
	>>> print("hello, world")

	程序可通过抛出(raise)系统退出异常(SystemExit exception)来退出程序。
	>>> raise SystemExit

	Python源文件以.py结尾，执行时只需输入python XXX.py 即可。
	在UNIX/Linux下，源文件一般开头如下：
	#!/usr/bin/env python
	# -*- coding: utf-8 -*-
	第一行说明指定用什么解释器运行脚本以及解释器所在的位置，第二行用来指定
	文件编码为utf-8

2.Variables and Arithmetic Expressions

	例1.1

	principal = 1000	# Initial amount
	rate = 0.05		# Interest rate
	numyears = 5		# Number of years
	year = 1
	while year <= numyears:
		principal = principal * (1 + rate)
		print year, principal
		# Reminder: print(year, principal) in Python 3
	year += 1

	Python是动态类型语言，也就是在程序运行期间，变量名可以指向不同类型的值。
	比如
	var = "hello"
	var = 1
	var = 1.5
	一开始var指向字符串值"hello"，接着指向整型1，最后指向浮点数1.5。

	Python每行一条语句，不用添加分号，如果需要在同一行输入多条语句，可使用
	分号隔开，如
	a = 'a'; b = 'b'; c = 'c'

	格式化输出：
	print "%3d %0.2f" % (year, principal)
	print("%3d %0.2f" % (year, principal)) # Python 3

	字符串格式化输出也可以使用format()函数：
	print format(year, "3d"), format(principal, "0.2f")
	print(format(year, "3d"), format(principal, "0.2f")) # Python 3

	也可以使用字符串自带的format()方法：
	print "{0:3d} {1:0.2f}".format(year, principal)
	print("{0:3d} {1:0.2f}".format(year, principal)) # Python 3
	
3.Conditionals

	if a < b:
		print 'a'
	elif a > b:
		print 'b'
	else
		pass # Do nothing

	此外还有 or, and, not，相当于C语言的 ||, &&, !
	布尔类型有True和False

	还有 in，用于查看一个值是否存在于另外一个值中(The in operator is
	commonly used to check whether a value is contained inside of another
	object such as a string, list, or dictionary.)

	if 'spam' in s:
		has_spam = True
	else:
		has_spam = False

	或者更简单点：
	has_spam = 'spam' in s

4.File Input and Output

	下面例子打开文件并读出每行内容

	f = open("foo.txt") # Returns a file object
	line = f.readline() # Invokes readline() method on file
	while line:
		print line, # trailing ',' omits newline character
		# print(line,end='') # Use in Python 3
		line = f.readline()
	f.close()

	迭代可使用更简单的形式
	for line in open("foo.txt"):
		print line,

	将程序的输出存到文件中，Python 2中我们可通过print使用>>
	如：
	f = open("out", "w") # Open file for writing
	while year <= numyears:
		principal = principal * (1 + rate)
		print >> f, "%3d %0.2f" % (year, principal)
		year += 1
	f.close()

	在Python 3中，我们可直接使用
	print("%3d %0.2f" % (year,principal), file=f)

	此外，文件对象可以直接使用write()方法写入原始数据，如
	f.write("%3d %0.2f\n" % (year,principal))

	我们也可以用于输入输出流中，如
	import sys
	sys.stdout.write("Enter your name :")
	name = sys.stdin.readline()

	在Python 2, 可写成
	name = raw_input("Enter your name :")
	在Python 3，是
	name = input("Enter your name :")

5.Strings - immutable

	字符串可以使用单引号，双引号，或者三个单引号，如
	a = "Hello World"
	b = 'Python is groovy'
	c = """Computer says 'No'"""

	不过单引号和双引号只能用于一行的字符串，三引号可用于多行以上，所以三引号
	一般用于文档的注释或者超过两行的字符串。

	显示字符串里的某个字符可使用s[i]，如
	a = "Hello World"
	b = a[4] # b = 'o'

	显示子字符串可使用s[i:j]的形式，(注意从i到j，但不含j)，如
	c = a[:5] # c = "Hello"
	d = a[6:] # d = "World"
	e = a[3:8] # e = "lo Wo"

	字符串可用 + 来连接，如
	g = a + "This is a test"

	数字字符串可使用int()或float()来转变成整型或浮点型，如
	x = "37"
	y = "42"
	z = int(x) + int(y) # z = 79 (Integer +)

	数字可以使用str(), repr() 和 format() 转化为字符串，如
	x = 37
	s = "The value of x is " + str(x)
	s = "The value of x is " + repr(x)
	s = "The value of x is " + format(x, "4d")

	str() 和 repr() 有点区别：
	str() produces the output that you get when you use the print statement,
	whereas repr() creates a string that you type into a program to exactly
	represent the value of an object. For example:
	>>> x = 3.4
	>>> str(x)
	'3.4'
	>>> repr(x)
	'3.3999999999999999'

	format()函数把值转化为特定的字符串格式，如
	>>> format(x, "0.5f")
	'3.40000'

6.Lists - mutable

	Lists是一组任意对象的列表。
	names = [ "Dave", "Mark", "Ann", "Phil" ]
	a = names[2] # Returns the third item of the list, "Ann"
	names[0] = "Jeff" # Changes the first item to "Jeff"

	通过append()方法可在列表最后添加新的对象。
	names.append("Paula")

	使用insert()方法可在列表任意位置插入新的对象。
	names.insert(2, "Thomas")

	通过 切片操作(slicing operator)，可以提取或再分配一个子列表。
	names = [ "Jeff", "Mark", "Thomas", "Ann", "Phil", "Paula" ]
	b = names[0:2] # Returns [ "Jeff", "Mark" ]，注意不含2
	c = names[2:] # Returns [ "Thomas", "Ann", "Phil", "Paula" ]
	names[1] = 'Jeff' # Replace the 2nd item in names with 'Jeff'
	names[0:2] = ['Dave','Mark','Jeff'] # Replace the first two items of
	# the list with the list on the right.

	使用 + 可以将两个列表连接起来：
	a = [1,2,3] + [4,5] # Result is [1,2,3,4,5]

	创建空列表有两种方式：
	names = [] # An empty list
	names = list() # An empty list

	列表可以包含任意类型的Python对象/物体，比如列表中含有列表：
	a = [ 1, "Dave", 3.14, [ "Mark", 7, 9, [ 100, 101 ] ], 10 ]

	通过多个下标可以查询相应的嵌套列表里的项
	a[1] # Returns "Dave"
	a[3][2] # Returns 9
	a[3][3][1] # Returns 101

	例 1.2

	import sys # Load the sys module
	if len(sys.argv) != 2 # Check number of command line arguments :
		print "Please supply a filename"
		raise SystemExit(1)
	f = open(sys.argv[1]) # Filename on the command line
	lines = f.readlines() # Read all lines into a list
	f.close()
	# Convert all of the input values from strings to floats
	fvalues = [float(line) for line in lines]
	# fvalues = [float(line) for line in open(sys.argv[1])]
	# Print min and max values
	print "The minimum value is ", min(fvalues)
	print "The maximum value is ", max(fvalues)

7.Tuples - immutable

	创建一个简单的数据结构，可以通过tuple把不同的值打包到一个单一的
	对象/物体中。

	stock = ('GOOG', 100, 490.10)
	address = ('www.python.org', 80)
	person = (first_name, last_name, phone)

	注意如果tuples只有0或1个对象的定义：
	a = () # 0-tuple (empty tuple)
	b = (item,) # 1-tuple (note the trailing comma，注意最后的逗号)

	取出tuple里的值和list类似，通过下标索引，如
	print stock[0] # GOOG
	print stock[1] # 100

	不过更常见的是把tuples的值转化为set变量，如
	name, shares, price = stock
	host, port = address
	first_name, last_name, phone = person

	虽然tuples支持大部分和lists的操作，比如切片，索引，连接(indexing,
	slicing, concatenation)，但tuples创建后是不能再改变的，是immutable的，
	也就是不能进行诸如替换、删除、添加(replace, delete, append)等操作。

	tuples相对于list会使用更紧凑的空间，但list为了性能往往会使用更多的空间，
	因此虽然tuples可能没有lists更灵活，但会占更少的内存空间。

	tuples和lists通常会结合一起使用来展现数据，如：

	# File containing lines of the form "name,shares,price"
	filename = "portfolio.csv"
	portfolio = []
	for line in open(filename):
		fields = line.split(",") # Split each line into a list
		name = fields[0] # Extract and convert individual fields
		shares = int(fields[1])
		price = float(fields[2])
		stock = (name,shares,price) # Create a tuple (name, shares, price)
		portfolio.append(stock) # Append to list of records

8.Sets - mutable

	set用于包含无序的对象。
	s = set([3, 5, 9, 10])
	t = set("Hello")

	由于set是无序的，因此不能像lists，tuples那样通过索引来查找，此外，
	set里面的元素是不重复的。比如查看上面的t：
	>>> t
	set(['H', 'e', 'l', 'o'])

	注意这里只有一个'l'出现。

	>>> s
	set([9, 10, 3, 5])

	输出是无序。

	sets支持一系列标准的运算符操作，including union, intersection,
	difference, symmetric difference：

	a = t | s # Union of t and s
	b = t & s # Intersection of t and s
	c = t – s # Set difference (items in t, but not in s)
	d = t ^ s # Symmetric difference (items in t or s, but not both)

	set可以通过add()或update()来添加新元素
	t.add('x') # Add a single item
	s.update([10,37,42]) # Adds multiple items to s

	>>> t
	set(['H', 'e', 'l', 'o', 'x'])
	>>> s
	set([3, 5, 9, 42, 10, 37])

	set里的元素可通过remove()来删除
	t.remove('H')

9.Dictionaries - mutable

	dictionary是关联数组(associative array)或哈希表(hash table)组成的对象，
	并使用关键值来进行索引(indexed by keys)。

	stock = {
		"name" : "GOOG",
		"shares" : 100,
		"price" : 490.10
	}
	peoples = { "guangzhou" : 1000, "foshan" : 800 }

	通过key值来获取dictionary相应的值
	name = stock["name"]
	value = stock["shares"] * stock["price"]

	插入或修改相关的值：
	stock["shares"] = 75
	stock["date"] = "June 7, 2010"

	dictionary的关键字通常是使用string，但也可以使用其它Python对象，比如
	数字，tuples，但不能使用lists、dictionary等，因为他们的值时可变的。
	也就是说作为dictionary的关键字的对象是immutable的，是不可改变的。

	此外dictionary可用于快速查找无序的数据。
	prices = {
		"GOOG" : 490.10,
		"AAPL" : 123.50,
		"IBM" : 91.50,
		"MSFT" : 52.13
	}

	创建空的dictionary：
	name = {}
	name = dict()

	使用in操作符
	if "SCOX" in prices:
		p = prices["SCOX"]
	else:
		p = 0.0

	可以变成如下：
	p = prices.get("SCOX", 0.0)

	获取dictionary的关键字，转化为list：
	syms = list(prices) # syms = ["AAPL", "MSFT", "IBM", "GOOG"]

	使用del来移除dictionary的元素：
	del prices["MSFT"]

10.Iteration and Looping

	for n in [1,2,3,4,5,6,7,8,9]:
		print "2 to the %d power is %d" % (n, 2**n)

	for n in range(1,10):
		print "2 to the %d power is %d" % (n, 2**n)

	a = range(5) # a = 0,1,2,3,4
	b = range(1,8) # b = 1,2,3,4,5,6,7
	c = range(0,14,3) # c = 0,3,6,9,12
	d = range(8,1,-1) # d = 8,7,6,5,4,3,2

	当range的数值很大时，Python 2推荐使用xrange()，Python 3把xrange()重命名
	为range()，旧的range()已移除。

	for除了迭代(iterate)数字外，还支持其它对象比如string，lists，dictionaries，
	files等等。

11.Functions

	使用def来创建函数

	def remainder(a,b):
		q = a // b # // is truncating division.
		r = a - q*b
		return r

	可以通过tuple来返回多个值：
	def divide(a,b):
		q = a // b # If a and b are integers, q is integer
		r = a - q*b
		return (q,r)

	quotient, remainder = divide(1456,33)

	对于函数参数，我们可以添加使用默认值：
	def connect(hostname,port,timeout=300):
		# Function body

	当函数有默认值时，我们调用时可以省略该值：
	connect("www.python.org", 80)

	也可通过参数关键字来调用：
	connect(port=80, hostname="www.python.org")

	函数内的参数都是local的，如果要用全局变量，可使用global关键字：
	count = 0
	...
	def foo():
		global count
		count += 1 # Changes the global variable count

12.Generators

	使用yield可以使函数生成完整的序列结果，再返回。
	def countdown(n):
		print "Counting down!"
		while n > 0:
			yield n # Generate a value (n)
			n -= 1

	使用yield的函数称为generator。Calling a generator function creates
	an object that produces a sequence of results through successive calls
	to a next() method (or __next__() in Python 3).

	>>> c = countdown(5)
	>>> c.next()
	Counting down!
	5
	>>> c.next()
	4
	>>> c.next()
	3

	调用next()可以让generator函数运行，直到下一个yield语句。就是执行了yield
	语句后就停止，此时返回传到yield上的值给next()。

	通常不能手动调用next()，可以使用下面的方式：
	>>> for i in countdown(5):
	...	print i
	Counting down!
	5 4 3 2 1

13.Coroutines (协同程序)

	a function can also be written to operate as a task that processes a
	sequence of inputs sent to it. This type of function is known as a
	coroutine and is created by using the yield statement as an
	expression (yield)

	def print_matches(matchtext):
		print "Looking for", matchtext
		while True:
			line = (yield) # Get a line of text
			if matchtext in line:
				print line

	使用这个函数，首先调用，然后使用send()开始发送数据。
	>>> matcher = print_matches("python")
	>>> matcher.next() # Advance to the first (yield)
	Looking for python
	>>> matcher.send("Hello World")
	>>> matcher.send("python is cool")
	python is cool
	>>> matcher.send("yow!")
	>>> matcher.close() # Done with the matcher function call

	A coroutine is suspended until a value is sent to it using send().

	协同程序(coroutines)对于编写并行程序(concurrent programs)是非常有用的，
	比如生产者和消费者问题，一个程序用于生产数据，另一个程序用于消费数据。

	下面是同时使用generators和coroutines：

	# A set of matcher coroutines
	matchers = [
		print_matches("python"),
		print_matches("guido"),
		print_matches("jython")
	]

	# Prep all of the matchers by calling next()
	for m in matchers: m.next()

	# Feed an active log file into all matchers. Note for this to work,
	# a web server must be actively writing data to the log.
	wwwlog = tail(open("access-log"))
	for line in wwwlog:
		for m in matchers:
			m.send(line) # Send data into each matcher coroutine

14.Objects and Classes

	Python程序里所有的值都是对象(object)。对象包含内部数据和方法。
	All values used in a program are objects. An object consists of internal
	data and methods that perform various kinds of operations involving
	that data.

	dir()函数可以列出object的方法，内容。
	items = [37, 42]
	>>> dir(items)

	class语句用于定义新的对象和用于面向对象编程(object-oriented programming)。

	class Stack(object):
		def __init__(self): # Initialize the stack
			self.stack = [ ]
		def push(self,object):
			self.stack.append(object)
		def pop(self):
			return self.stack.pop()
		def length(self):
			return len(self.stack)

	class Stack(object)定义Stack为object。Stack继承于object，object是Python
	的根对象。(Stack inherits from object, which is the root of all Python types)
	每个方法的第一个函数都是object self。(The first argument in each method
	always refers to the object itself.)

	像__init__前后有两个下划线的方法是比较特殊的方法。比如__init__用于初始化
	刚创造的对象。

	s = Stack() # Create a stack
	s.push("Dave") # Push some things onto it
	s.push(42)
	s.push([3,4,5])
	x = s.pop() # x gets [3,4,5]
	y = s.pop() # y gets 42
	del s # Destroy s

	stack和list相似，因此可以直接继承list。
	class Stack(list):
		# Add push() method for stack interface
		# Note: lists already provide a pop() method.
		def push(self,object):
			self.append(object)

	通常类的方法一般只能通过该类的实例来调用，但也可以通过
	@ststicmethod 定义为static。

	后续章节会详解。

15.Exceptions

	如果程序出错抛出异常，会有回溯信息返回(traceback message)。
	异常通常会导致程序退出，但也可以用try和except来捕获处理异常。

	try:
		f = open("file.txt", "r")
	except IOError as e:
		print e

	raise语句可用于引发信号或异常：
	raise RuntimeError("Computer says no")

16.Modules

	创建一个module，文件名必须和module名一致。
	# file : div.py
	def divide(a,b):
		q = a/b # If a and b are integers, q is an integer
		r = a - q*b
		return (q,r)

	使用div模块，在文件里导入即可：
	import div
	a, b = div.divide(2305, 29)

	也可以使用别的名字：
	import div as foo
	a, b = foo.divide(2305, 29)

	如果只需导入模块里某个方法，可以使用from语句：
	from div import divide:
	a, b = divide(2305, 29)

	使用from也可以把模块所有内容导入进来：
	from div import *

	使用dir()，也可以列出模块的内容
	import div
	>>> dir(div)

17.Getting Help

	使用help()方法可查看函数的用法或模块的信息。
	>>> import sys
	>>> help(sys)
	>>> a = tuple()
	>>> help(a)

	大多数Python函数都有文档注释来说明用法，可以使用__doc__属性来输出
	这些文档注释。
	>>> print issubclass.__doc__

	大多数Python都装有pydoc命令，因此可以直接在linux终端命令行上使用pydoc
	来查看相应的内容，如
	$ pydoc sys

--------------------------------------------------------------------------------

第二章

1.Line Structure and Indentation

2.Identifiers and Reserved Words

	and         del        from      nonlocal    try
	as          elif       global    not         while
	assert      else       if        or          with
	break       except     import    pass        yield
	class       exec       in        print
	continue    finally    is        raise
	def         for        lambda    return

	变量名可以使用子母、数字、下划线(_)来组成，但不能以数字开头。变量名
	是case sensitive，也就是大小写区分的。以下划线开头的名字比较特殊，
	要注意。比如 _foo 是不会被导入语句from module import *导入的，
	__init__ 用于保留特殊方法，__bar 用于私有类成员。

3.Numeric Literals

	Python自由4种类型数值文字:
	booleans, integers, floating-point numbers, Complex bumbers(复数).

	八进制写法 0 - 0644
	十六进制写法 0x - 0x100fea8
	二进制写法 0b - 0b11110000

	对于很大的数字，可以直接写，比如 n = 12345678901234567890
	123.34 或 1.2334e+02 是浮点数
	带有j或J的整数或浮点数是虚数(imaginary number)。创建复数
	1.2 + 12.34J

4.String Literals

	\uxxxx          Unicode character (\u0000 to \uffff) - 2个字节的unicode
	\Uxxxxxxxx      Unicode character (\U00000000 to \Uffffffff) - 4个字节的unicode
	\N{charname}    Unicode character name
	\OOO            Octal value (\000 to \377)
	\xhh            Hexadecimal value (\x00 to \xff)

	Python 2不支持international character sets 和 Unicode。为了输入Unicode
	字符常量，可以在字符串前添加 u，如：
	s = u"jalape\u00f1o" # Jalapeño , u\00f1 is the character code for ñ

	但在Python 3，所有字符都是Unicode。

	后面没看明白。

5.Containers

6.Operators, Delimiters, and Special Symbols

	+     -     *     **     /     //     %     <<    >>    &      |
	^     ~     <     >      <=    >=     ==    !=    <>    +=
	-=    *=    /=    //=    %=    **=    &=    |=    ^=    >>=    <<=

7.Documentation Strings

	def fact(n):
	'''
		This function computes a factorial
	'''
		if (n <= 1): return 1
		else: return n * fact(n - 1)

8.Decorators(修饰符)

	@symbol 修饰符可用于函数、方法、类的定义：

	class Foo(object):
		@staticmethod #静态函数，可以被类以外的成员调用
		def bar():
			pass

	可以使用多个修饰符
	@foo
	@bar
	def spam():
		pass

	后续章节会详解。

9.Source Code Encoding

	#!/usr/bin/env python
	# -*- coding: UTF-8 -*-

	s = "Jalapeño" # String in quotes is directly encoded in UTF-8.

--------------------------------------------------------------------------------

第三章 Types and Objects

1.Terminology

2.Object Identity and Type

	函数in()返回对象的身份(identity)，操作符 is 判断两个对象的身份，type()
	返回对象的类型(type)

	# Compare two objects
	def compare(a,b):
		if a is b:
			# a and b are the same object
			statements
		if a == b:
			# a and b have the same value
			statements
		if type(a) is type(b):
			# a and b have the same type
			statements

	if type(s) is list:
		s.append(item)
	if type(d) is dict:
		d.update(t)

	检查对象的类型比较好使用isinstance(object, type)
	if isinstance(s,list):
		s.append(item)
	if isinstance(d,dict):
		d.update(t)

3.Reference Counting and Garbage Collection

	All objects are reference-counted.
	a = 37 # Creates an object with value 37
	b = a # Increases reference count on 37
	c = []
	c.append(b) # Increases reference count on 37

	对象的引用计数可以通过del语句来减少，或者对象重新指向新值。
	del a # Decrease reference count of 37
	b = 42 # Decrease reference count of 37
	c[0] = 2.0 # Decrease reference count of 37

	可以通过sys.getrefcount()来获取对象的引用数。
	>>> a = 37
	>>> import sys
	>>> sys.getrefcount(a)
	7

	对于数字和字符串，引用计数通常比较大，原因是系统为了节省内存，在不同的
	程序间共享对象。

	当对象的引用数为0，系统就会进行内存回收(garbage-collected)。

4.References and Copies

	对于类似数字和字符等immutable对象，复制一个对象非常有效，只需把变量
	指向该物体即可。
	>>> a = 37
	>>> b = a
	a ----> +----+
	        | 37 |
	b ----> +----+

	>>> b = 10
	>>> print b
	10
	>>> print a
	37

	>>> a = 10
	>>> b = a
	>>> a is b
	True
	>>> id(a)
	35208080
	>>> id(b)
	35208080
	>>> b = 100
	>>> id(a)
	35208080
	>>> id(b)
	35209904

	但如果是分别创建：
	>>> a = 9999.999
	>>> b = 9999.999
	>>> id(a)
	35225392
	>>> id(b)
	35225416
	

	但对于mutable对象，比如list和dictionaries，就不一样了。
	>>> a = [1, 2, 3, 4]
	>>> b = a
	>>> b[2] = -100
	>>> print a
	[1, 2, -100, 4]

	a和b指向同一个对象，改变其中一个变量的值会影响另外一个变量的值。因此，
	对于mutable对象，要复制一个对象，而不是进行引用(reference)。

	复制对象有两种形式：浅拷贝和深拷贝(shallow copy and deep cpoy)。

	A shallow copy creates a new object but populates it with references to
	the items contained in the original object.
	对一个对象进行浅拷贝其实是新创建了一个类型跟原对象一样，其内容是原来对象
	元素的引用，换句话说，这个拷贝的对象本身是新的，但是它的内容不是。
	即对象本身是新的，内容是旧的。

	>>> a = [ 1, 2, [3,4] ]
	>>> b = list(a) # Create a shallow copy of a.
	>>> b is a
	False
	>>> b.append(100) # Append element to b.
	>>> b
	[1, 2, [3, 4], 100]
	>>> a # Notice that a is unchanged
	[1, 2, [3, 4]]
	>>> b[2][0] = -100 # Modify an element inside b
	>>> b
	[1, 2, [-100, 4], 100]
	>>> a # Notice the change inside a
	[1, 2, [-100, 4]]

	对于上面列表里的嵌套[3, 4]，由于使用的是浅拷贝，因此a和b都是指向[3, 4]，
	当b发生改变时，也会影响到a。

	a ---> [1, 2, xx]
                       |
                       V
                      [3, 4]
                       ^
                       |
	b ---> [1, 2, xx]


	A deep copy creates a new object and recursively copies all the objects
	it contains.
	深度拷贝是创建新的对象，且递归地创建新的内容。
	>>> import copy
	>>> a = [1, 2, [3, 4]]
	>>> b = copy.deepcopy(a)
	>>> b[2][0] = -100
	>>> b
	[1, 2, [-100, 4]]
	>>> a # Notice that a is unchanged
	[1, 2, [3, 4]]

	a ---> [1, 2, xx]
                       |
                       V
                      [3, 4]

	b ---> [1, 2, XX]
                       |
                       V
                      [3, 4]

5.First-Class Objects

	All objects in Python are said to be "first class". It also means that
	all objects that can be named can be treated as data.

	items = {
		'number' : 42,
		'text' : "Hello World"
	}

	items["func"] = abs # Add the abs() function
	import math
	items["mod"] = math # Add a module
	items["error"] = ValueError # Add an exception type
	nums = [1,2,3,4]
	items["append"] = nums.append # Add a method of another object

	上面的例子items包含函数、模块、异常、对象的方法。

	>>> items["func"](-45) # Executes abs(-45)
	45
	>>> items["mod"].sqrt(4) # Executes math.sqrt(4)
	2.0
	>>> try:
	...     x = int("a lot")
	... except items["error"] as e: # Same as except ValueError as e
	...     print("Couldn't convert")
	...
	Couldn't convert
	>>> items["append"](100) # Executes nums.append(100)
	>>> nums
	[1, 2, 3, 4, 100]

6.Built-in Types for Representing Data

	Type Category    Type Name    Description
	-------------------------------------------
	None             type(None)   The null object None
	Numbers          int          Integer
	                 long         Arbitrary-precision integer (Python 2 only)
	                 float        Floating point
	                 complex      Complex number
	                 bool         Boolean (True or False)
	Sequences        str          Character string
	                 unicode      Unicode character string (Python 2 only)
	                 list         List
	                 tuple        Tuple
	                 xrange       A range of integers created by xrange() (In Python 3,
	                              it is called range.)
	Mapping          dict         Dictionary
	Sets             set          Mutable set
	                 frozenset    Immutable set

	详细的可以看书。
	The None Type
	Numeric Types
	Sequence Types
	Operations Common to All Sequences
	Lists
	Strings
	xrange() Objects
	Mapping Types
	Set Types
		s = set([1,5,10,15])
		f = frozenset(['a',37,'hello'])
	
7.Built-in Types for Representing Program Structure


	Callable Types

	Methods
		Methods are functions that are defined inside a class definition.
		There are three common types of methods —
		instance methods, class methods, and static methods:

		class Foo(object):
			def instance_method(self,arg):
				statements
			@classmethod
			def class_method(cls,arg):
				statements
			@staticmethod
			def static_method(arg):
				statements

		f = Foo() # Create an instance
		meth = f.instance_method # Lookup the method and notice the lack of ()
		meth(37) # Now call the method

		umeth = Foo.instance_method # Lookup instance_method on Foo
		umeth(f,37) # Call it, but explicitly supply self

		An instance method is a method that operates on an instance
			belonging to a given class.
		A class method operates on the class itself as an object.
		A static method is a just a function that happens to be packaged
			inside a class.
	
	Class, Types, and Instances

	Modules

	详细请看书

--------------------------------------------------------------------------------

第四章 Operators and Expressions

1.Operations on Numbers

	Operation    Description
	------------------------
	x + y        Addition
	x - y        Subtraction
	x * y        Multiplication
	x / y        Division
	x // y       Truncating division
	x ** y       Power (xy)
	x % y        Modulo (x mod y)
	–x           Unary minus
	+x           Unary plus
	x << y       Left shift
	x >> y       Right shift
	x & y        Bitwise and
	x | y        Bitwise or
	x ^ y        Bitwise xor (exclusive or)
	~x           Bitwise negation



--------------------------------------------------------------------------------

第五章 Program Structure and Control FlowBuilt-in Exceptions


--------------------------------------------------------------------------------

第六章 Functions and Functional Programming


Decorators 没明白

	@trace
	def square(x):
		return x*x

	等价于
	def square(x):
		return x*x
	square = trace(square)


	@dec2
	@dec1
	def func(arg1, arg2, ...):
		pass

	等价于

	def func(arg1, arg2, ...):
		pass
	func = dec2(dec1(func))
	
--------------------------------------------------------------------------------

第二十一章 Network Programming and Sockets

	# Time server program
	from socket import *
	import time

	s = socket(AF_INET, SOCK_STREAM) # Create a TCP socket
	s.bind(('',8888)) # Bind to port 8888
	s.listen(5) # Listen, but allow no more than
	            # 5 pending connections.
	while True:
		client,addr = s.accept() # Get a connection
		print("Got a connection from %s" % str(addr))
		timestr = time.ctime(time.time()) + "\r\n"
		client.send(timestr.encode('ascii'))
		client.close()


	# Time client program
	from socket import *

	s = socket(AF_INET,SOCK_STREAM) # Create a TCP socket
	s.connect(('localhost', 8888)) # Connect to the server
	tm = s.recv(1024) # Receive no more than 1024 bytes
	s.close()
	print("The time is %s" % tm.decode('ascii'))



--------------------------------------------------------------------------------

Python 还是现学现用吧

http://www.liaoxuefeng.com/wiki/001374738125095c955c1e6d8bb493182103fac9270762a000


1.字符编码

	Unicode把所有语言都统一到一套编码里，这样就不会再有乱码问题了。
	Unicode标准也在不断发展，但最常用的是用两个字节表示一个字符（如果要用到
	非常偏僻的字符，就需要4个字节）。现代操作系统和大多数编程语言都直接支持
	Unicode。

	现在，捋一捋ASCII编码和Unicode编码的区别：ASCII编码是1个字节，而Unicode
	编码通常是2个字节。

	字母'A'用ASCII编码是十进制的65，二进制的01000001；
	字符'0'用ASCII编码是十进制的48，二进制的00110000，注意字符'0'和整数0是不同的；
	汉字'中'已经超出了ASCII编码的范围，用Unicode编码是十进制的20013，
	二进制的01001110 00101101。
	你可以猜测，如果把ASCII编码的'A'用Unicode编码，只需要在前面补0就可以，
	因此，'A'的Unicode编码是00000000 01000001。

	新的问题又出现了：如果统一成Unicode编码，乱码问题从此消失了。但是，如果
	你写的文本基本上全部是英文的话，用Unicode编码比ASCII编码需要多一倍的存储
	空间，在存储和传输上就十分不划算。
	所以，本着节约的精神，又出现了把Unicode编码转化为“可变长编码”的UTF-8编码。
	UTF-8编码把一个Unicode字符根据不同的数字大小编码成1-6个字节，常用的英文
	字母被编码成1个字节，汉字通常是3个字节，只有很生僻的字符才会被编码成
	4-6个字节。如果你要传输的文本包含大量英文字符，用UTF-8编码就能节省空间：

	字符	ASCII		Unicode			UTF-8
	A	01000001	00000000 01000001	01000001
	中	x		01001110 00101101	11100100 10111000 10101101

	从上面的表格还可以发现，UTF-8编码有一个额外的好处，就是ASCII编码实际上
	可以被看成是UTF-8编码的一部分，所以，大量只支持ASCII编码的历史遗留软件
	可以在UTF-8编码下继续工作。

	搞清楚了ASCII、Unicode和UTF-8的关系，我们就可以总结一下现在计算机系统
	通用的字符编码工作方式：
	在计算机内存中，统一使用Unicode编码，当需要保存到硬盘或者需要传输的时候，
	就转换为UTF-8编码。

	用记事本编辑的时候，从文件读取的UTF-8字符被转换为Unicode字符到内存里，
	编辑完成后，保存的时候再把Unicode转换为UTF-8保存到文件：

	+-----------------------+
	| 记事本                |
	|      Unicode编码      |
	+-----------------------+
	   ^                |
	   |                |
	读取：转换为       保存：转换为
	Unicode            UTF-8
	   |                |
	   |                V
	+------------------------+
	| 文件：abc.txt          |
	|        UTF-8编码       |
	+------------------------+

	浏览网页的时候，服务器会把动态生成的Unicode内容转换为UTF-8再传输到浏览器：

	+--------------------+
	| 服务器             |
	|      Unicode编码   |
	+--------------------+
	        |
	        |
	    输出UTF-8网页
	        |
	        |
	        V
	+--------------------+
	|     浏览器         |
	+--------------------+

	所以你看到很多网页的源码上会有类似<meta charset="UTF-8" />的信息，
	表示该网页正是用的UTF-8编码。

2.Python的字符串

	搞清楚了令人头疼的字符编码问题后，我们再来研究Python对Unicode的支持。
	因为Python的诞生比Unicode标准发布的时间还要早，所以最早的Python只支持
	ASCII编码，普通的字符串'ABC'在Python内部都是ASCII编码的。Python提供了
	ord()和chr()函数，可以把字母和对应的数字相互转换：

	>>> ord('A')
	65
	>>> chr(65)
	'A'

	Python在后来添加了对Unicode的支持，以Unicode表示的字符串用u'...'表示，
	比如：

	>>> print u'中文'
	中文
	>>> u'中'
	u'\u4e2d'

	写u'中'和u'\u4e2d'是一样的，\u后面是十六进制的Unicode码。因此，
	u'A'和u'\u0041'也是一样的。

	两种字符串如何相互转换？字符串'xxx'虽然是ASCII编码，但也可以看成是
	UTF-8编码，而u'xxx'则只能是Unicode编码。

	把u'xxx'转换为UTF-8编码的'xxx'用encode('utf-8')方法：

	>>> u'ABC'.encode('utf-8')
	'ABC'
	>>> u'中文'.encode('utf-8')
	'\xe4\xb8\xad\xe6\x96\x87'

	英文字符转换后表示的UTF-8的值和Unicode值相等（但占用的存储空间不同），
	而中文字符转换后1个Unicode字符将变为3个UTF-8字符，你看到的\xe4就是其中
	一个字节，因为它的值是228，没有对应的字母可以显示，所以以十六进制显示
	字节的数值。len()函数可以返回字符串的长度：

	>>> len(u'ABC')
	3
	>>> len('ABC')
	3
	>>> len(u'中文')
	2
	>>> len('\xe4\xb8\xad\xe6\x96\x87')
	6

	反过来，把UTF-8编码表示的字符串'xxx'转换为Unicode字符串u'xxx'用
	decode('utf-8')方法：

	>>> 'abc'.decode('utf-8')
	u'abc'
	>>> '\xe4\xb8\xad\xe6\x96\x87'.decode('utf-8')
	u'\u4e2d\u6587'
	>>> print '\xe4\xb8\xad\xe6\x96\x87'.decode('utf-8')
	中文

	由于Python源代码也是一个文本文件，所以，当你的源代码中包含中文的时候，
	在保存源代码时，就需要务必指定保存为UTF-8编码。当Python解释器读取
	源代码时，为了让它按UTF-8编码读取，我们通常在文件开头写上这两行：
	#!/usr/bin/env python
	# -*- coding: utf-8 -*-

	第一行注释是为了告诉Linux/OS X系统，这是一个Python可执行程序，Windows
	系统会忽略这个注释；
	第二行注释是为了告诉Python解释器，按照UTF-8编码读取源代码，否则，你在
	源代码中写的中文输出可能会有乱码。

	还有 raw string:
	r"abc\n\ropq"

3.函数式编程

	函数是Python内建支持的一种封装，我们通过把大段代码拆成函数，通过一层一层
	的函数调用，就可以把复杂任务分解成简单的任务，这种分解可以称之为面向过程
	的程序设计。函数就是面向过程的程序设计的基本单元。

	而函数式编程（请注意多了一个“式”字）——Functional Programming，虽然也可以
	归结到面向过程的程序设计，但其思想更接近数学计算。

	我们首先要搞明白计算机（Computer）和计算（Compute）的概念。
	在计算机的层次上，CPU执行的是加减乘除的指令代码，以及各种条件判断和跳转
	指令，所以，汇编语言是最贴近计算机的语言。
	而计算则指数学意义上的计算，越是抽象的计算，离计算机硬件越远。
	对应到编程语言，就是越低级的语言，越贴近计算机，抽象程度低，执行效率高，
	比如C语言；越高级的语言，越贴近计算，抽象程度高，执行效率低，比如Lisp语言。

	函数式编程就是一种抽象程度很高的编程范式，纯粹的函数式编程语言编写的函数
	没有变量，因此，任意一个函数，只要输入是确定的，输出就是确定的，这种
	纯函数我们称之为没有副作用。而允许使用变量的程序设计语言，由于函数内部
	的变量状态不确定，同样的输入，可能得到不同的输出，因此，这种函数是有
	副作用的。

	函数式编程的一个特点就是，允许把函数本身作为参数传入另一个函数，还允许
	返回一个函数！

	Python对函数式编程提供部分支持。由于Python允许使用变量，因此，Python不是
	纯函数式编程语言。


	高阶函数
		map/reduce - Python内建了map()和reduce()函数。
		filter - Python内建的filter()函数用于过滤序列。
		sorted - Python内置的sorted()函数就可以对list进行排序

	返回函数
		高阶函数除了可以接受函数作为参数外，还可以把函数作为结果值返回。

	匿名函数
		关键字lambda表示匿名函数，冒号前面的x表示函数参数。

4.装饰器 (或者看看python实例手册.py 简单的例子)

	由于函数也是一个对象，而且函数对象可以被赋值给变量，所以，通过
	变量也能调用该函数。

	>>> def now():
	...     print '2013-12-25'
	...
	>>> f = now
	>>> f()
	2013-12-25

	函数对象有一个__name__属性，可以拿到函数的名字：
	>>> now.__name__
	'now'
	>>> f.__name__
	'now'

	现在，假设我们要增强now()函数的功能，比如，在函数调用前后自动打印日志，
	但又不希望修改now()函数的定义，这种在代码运行期间动态增加功能的方式，
	称之为“装饰器”（Decorator）。
	本质上，decorator就是一个返回函数的高阶函数。所以，我们要定义一个能
	打印日志的decorator，可以定义如下：

	def log(func):
		def wrapper(*args, **kw):
			print 'call %s():' % func.__name__
			return func(*args, **kw)
		return wrapper

	观察上面的log，因为它是一个decorator，所以接受一个函数作为参数，并返回
	一个函数。我们要借助Python的@语法，把decorator置于函数的定义处：

	@log
	def now():
		print '2013-12-25'

	调用now()函数，不仅会运行now()函数本身，还会在运行now()函数前打印一行
	日志：
	>>> now()
	call now():
	2013-12-25


	当函数的参数不确定时，可以使用*args 和**kwargs，*args 没有key值，
	**kwargs有key值。

5.模块

	以内建的sys模块为例，编写一个hello的模块：

	#!/usr/bin/env python
	# -*- coding: utf-8 -*-

	#字符串，表示模块的文档注释，任何模块代码的第一个字符串都被视为模块的文档注释；
	' a test module '

	__author__ = 'Michael Liao'

	import sys

	def test():
		args = sys.argv
		if len(args)==1:
			print 'Hello, world!'
		elif len(args)==2:
			print 'Hello, %s!' % args[1]
		else:
			print 'Too many arguments!'

	if __name__=='__main__':
		test()

	注意最后这两行代码，当我们在命令行运行hello模块文件时，Python解释器
	把一个特殊变量__name__置为__main__，而如果在其他地方导入该hello模块时，
	if判断将失败，因此，这种if测试可以让一个模块通过命令行运行时执行一些
	额外的代码，最常见的就是运行测试。

	使用该模块:
	>>> import hello
	>>> hello.test()

