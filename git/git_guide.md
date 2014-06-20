
# Git guide

http://git-scm.com/  
http://try.github.com/  
http://www.bootcss.com/p/git-guide/  
http://blog.jobbole.com/71091/  

--------------------------------------------------------------------------------

# Basic Setting:

Configure user name: (use when you commit)

    $ git config --global user.name "weiqiangdragonite"

Configure user email: (use when you commit)

    $ git config --global user.email "weiqiangdragonite@gmail.com"

Tell git that you don't want to type your username and password every time you
talk to a remote server:

    $ git config --global credential.helper cache

By default git will cache your password for 15 minutes.
You can change this if you like:

    $ git config --global credential.helper 'cache --timeout=3600'

This will set the cache to timeout after 1 hour (setting is in seconds)

--------------------------------------------------------------------------------

# Create new Repository

    $ mkdir new_repo
    $ cd new_repo
    $ git init
    $ touch README.md
    $ git add --all
    $ git commit -m "..."
    $ git remote add origin git@github.com:weiqiangdragonite/hello.git
    $ git push -u origin master


--------------------------------------------------------------------------------

# 常用命令：

1.从远程主机克隆到本地

    git clone https://...

2.将改动添加至缓存

    git add --all

3.提交改动到本地仓库

    git commit -m "..."

4.将本地仓库推到远程主机

    git push -u origin master

5.远程主机的版本有了更新，将这些更新取回本地

    git pull origin master

6.创建新的分支并切换过去

    git checkout -b feature_x

7.将新的分支推到远程主机上 (更新完后还需要将改动提交到本地仓库再推到主机上)

    git push origin feature_x

8.切换回主分支

    git checkout master

9.合并分支

    git merge feature_x


--------------------------------------------------------------------------------

1.创建新仓库

    $ mkdir new_directory
    $ cd new_directory
    $ git init

2.执行如下命令以创建一个本地仓库的克隆版本：

    $ git clone /path/to/repository

如果是远端服务器上的仓库，你的命令会是这个样子：

    git clone username@host:/path/to/repository

你的本地仓库由 git 维护的三棵“树”组成。第一个是你的 工作目录，它持有实际文件；
第二个是 缓存区（Index），它像个缓存区域，临时保存你的改动；
最后是 HEAD，指向你最近一次提交后的结果。

                 add          commit
    working dir -----> Index --------> HEAD

检查当前状态：

    $ git status

有4种状态，分别是  
staged: Files are ready to be committed.  
unstaged: Files with changes that have not been prepared to be commited.  
untracked: Files aren't tracked by Git yet. This usually indicates a newly
created file.  
deleted: File has been deleted and is waiting to be removed from Git.

3.可以计划改动（把它们添加到缓存区），使用如下命令：

    $ git add <filename>
    $ git add *
    $ git add .

4.使用如下命令以实际提交改动： (现在，你的改动已经提交到了 HEAD，但是还没到你的
远端仓库)

    $ git commit -m "代码提交信息"

5.你的改动现在已经在本地仓库的 HEAD 中了。执行如下命令以将这些改动提交到远端
仓库：

    $ git push origin master
    $ git push -u origin master

The -u tells Git to remember the parameters, so that next time we can simply
run git push and Git will know what to do.

(可以把 master 换成你想要推送的任何分支)

如果你还没有克隆现有仓库，并欲将你的仓库连接到某个远程服务器，你可以使用如下
命令添加：

    $ git remote add origin <server>

如此你就能够将你的改动推送到所添加的服务器上去了。

分支是用来将特性开发绝缘开来的。在你创建仓库的时候，master 是“默认的”。在其他
分支上进行开发，完成后再将它们合并到主分支上。

6.创建一个叫做“feature_x”的分支，并切换过去：

    $ git checkout -b feature_x

切换回主分支：

    $ git checkout master

把新建的分支删掉：

    $ git branch -d feature_x

除非你将分支推送到远端仓库，不然该分支就是不为他人所见的：

    $ git push origin <branch>

7.要更新你的本地仓库至最新改动，执行：
git pull命令的作用是，取回远程主机某个分支的更新，再与本地的指定分支合并。

    $ git pull origin master
    $ git pull

要合并其他分支到你的当前分支（例如 master），执行：

    $ git merge <branch>

在合并改动之前，也可以使用如下命令查看：

    $ git diff <source_branch> <target_branch>

8.标签

9.替换本地改动

    $ git checkout -- <filename>
    
此命令会使用 HEAD 中的最新内容替换掉你的工作目录中的文件。已添加到缓存区的改动，
以及新文件，都不受影响。

假如你想要丢弃你所有的本地改动与提交，可以到服务器上获取最新的版本并将你本地
主分支指向到它：

    $ git fetch origin
    $ git reset --hard origin/master

10.查看记录

    $ git log
    $ git log --summary




11.一旦远程主机的版本库有了更新（Git术语叫做commit），需要将这些更新取回本地，这时就要用到
git fetch命令。

    $ git fetch <远程主机名>

上面命令将某个远程主机的更新，全部取回本地。
默认情况下，git fetch取回所有分支（branch）的更新。如果只想取回特定分支的更新，可以指定
分支名。

    $ git fetch <远程主机名> <分支名>

比如，取回origin主机的master分支

    $ git fetch origin master

所取回的更新，在本地主机上要用”远程主机名/分支名”的形式读取。比如origin主机的master，
就要用origin/master读取。

git branch命令的-r选项，可以用来查看远程分支，-a选项查看所有分支。

    $ git branch -r
      origin/master
 
    $ git branch -a
      * master
      remotes/origin/master

上面命令表示，本地主机的当前分支是master，远程分支是origin/master。
