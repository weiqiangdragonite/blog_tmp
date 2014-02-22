参考自 [Markdown语法说明（简体中文版）][1] 和 [Markdown快速入门][2]

--------------------------------------------------------------------------------

# Markdown 安装：

Chrome 的 Markdown 插件：Made 挺好用

Linux 下的 Markdown 编辑器 ReText 安装（基于Fedora）：（需要 Python 3 和 PyQt4）

    <!-- lang: shell -->
    $ yum install python3-pip
    $ yum install python3-PyQt4
    $ python3-pip install Markdown
    $ python3-pip install Markups
    $ yum install python3-enchant

然后到 [ReText 主页][3] 处下载安装包安装：

    <!-- lang: shell -->
    # python3 setup.py install

--------------------------------------------------------------------------------

# 标题：

Markdown 支持两种标题的语法，类 Setext 和类 atx 形式。  
类 Setext 形式是用底线的形式，利用 = （最高阶标题）和 - （第二阶标题），例如：

    <!-- lang: shell -->
    This is an H1
    =============
    This is an H2
    -------------

任何数量的 = 和 - 都可以有效果。效果显示如下：

This is an H1
=============
This is an H2
-------------

类 Atx 形式则是在行首插入 1 到 6 个 # ，对应到标题 1 到 6 阶，例如：

    <!-- lang: shell -->
    # 这是H1
    ## 这是H2
    ### 这是H3

你可以选择性地「闭合」类 atx 样式的标题，这纯粹只是美观用的，若是觉得这样看起  
来比较舒适，你就可以在行尾加上 #，而行尾的 # 数量也不用和开头一样（行首的井  
字符数量决定标题的阶数）：

    <!-- lang: shell -->
    # 这是H1 #
    ## 这是H2 ##
    ### 这是H3 #

效果显示如下：

# 这是H1
## 这是H2
### 这是H3
###### 这是H6

--------------------------------------------------------------------------------

# 段落和换行：

使用一个或多个 空行 分隔内容段来生成段落 (&lt;p&gt;)，输入以下文本：

    <!-- lang: shell -->
    This is a paragraph.
    
    This is another paragraph.

就会生成下面的文本：

    <!-- lang: html -->
    <p>This is a paragraph.</p>
    <p>This is another paragraph.</p>

效果显示如下：

This is a paragraph.

This is another paragraph.

如果我们不插入一个空行的话，即输入下面的文本：

    <!-- lang: shell -->
    There is no paragraph.
    This is a paragraph.

就会变成这样子了：

    <!-- lang: html -->
    <p>There is no paragraph. This is a paragraph.</p>

效果是这样：

There is no paragraph.
This is a paragraph.

当我们要控制自己文本的换行，就要在每行的行尾处插入两个或以上的 空格 再回车来  
生成换行 （&lt;br&gt;）：

    <!-- lang: shell -->
    This is a new line and has two space after  
    I am in another line now.

就会变成这样：

    <!-- lang: html -->
    This is a new line have two space after<br>
    I am in another new line now.

效果如下：

This is a new line have two space after  
I am in another line now.

--------------------------------------------------------------------------------

# 分隔线：

在一行中用三个以上的星号（*）、减号（-）、底线（_）来建立一个分隔线，行内不能  
有其他东西。你也可以在星号或是减号中间插入空格。下面每种写法都可以建立分隔线：

    <!-- lang: shell -->
    *****
    -----
    _____

--------------------------------------------------------------------------------

# 转义字符：

--------------------------------------------------------------------------------

  [1]: http://gitcafe.com/riku/Markdown-Syntax-CN/blob/master/syntax.md
  [2]: http://www.oschina.net/question/100267_75314
  [3]: http://sourceforge.net/p/retext/home/ReText/
