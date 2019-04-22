
# Git guide

Git主页 http://git-scm.com/  
交互式教程 http://try.github.com/  
简易指南 http://www.bootcss.com/p/git-guide/  
Git远程操作详解 http://blog.jobbole.com/71091/  
(伯乐在线有比较多git的教程)  
Git课程 http://www.imooc.com/learn/208?from=jobboleblog  

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


或者直接设置SSH

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

    git add <file>
    git add --all

3.提交改动到本地仓库

    git commit -m "..."

4.将本地仓库推到远程主机

    git push -u origin master
	git push  origin <branch>

5.远程主机的版本有了更新，将这些更新取回本地

    git pull origin master
	
	不建议使用pull，推荐使用 git fetch 和 git merge
	(
	git fetch origin master
	git log -p master.. origin/master
	git merge origin/master
	)

6.创建新的分支并切换过去

    git checkout -b feature_x
	(git branch feature_x && git checkout feature_x)

7.将新的分支推到远程主机上 (更新完后还需要将改动提交到本地仓库再推到主机上)

    git push origin feature_x

8.切换回主分支

    git checkout master

9.合并分支

    git merge feature_x


如果开发者是在创建develop分支前就clone下来的话，这里checkout会报错，应该先执行
git fetch origin develop
然后再执行 git checkout -b develop origin/develop



查看仓库状态
git status

查看修改内容
git diff

查看提交日志
git log
git log --pretty=oneline


查看命令历史
git reflog

回退版本
git reset --hard <ID>

把工作区的修改撤销
git checkout -- <file>
git reset HEAD <file>

查看当前分支
git branch
git branch -av

切换分支
git checkout <branch>

合并分支(合并指定分支到当前分支)
git merge <branch>
git merge --no-ff -m "merge with no-ff" <branch>

删除分支
eaa34a0 <branch>
git branch -D <branch> (丢弃一个没有被合并过的分支,强行删除)

删除远程分支
git push origin --delete <分支名称>

删除本地不存在的远程分支
git remote show origin
git remote prune origin


查看分支合并图
git log --graph
git log --graph --pretty=oneline --abbrev-commit



(Git鼓励你使用分支完成某个任务，合并后再删掉分支，这和直接在master分支上工作效果是一样的，但过程更安全)

分支策略
在实际开发中，我们应该按照几个基本原则进行分支管理：

首先，master分支应该是非常稳定的，也就是仅用来发布新版本，平时不能在上面干活；

那在哪干活呢？干活都在dev分支上，也就是说，dev分支是不稳定的，到某个时候，比如1.0版本发布时，再把dev分支合并到master上，在master分支发布1.0版本；

你和你的小伙伴们每个人都在dev分支上干活，每个人都有自己的分支，时不时地往dev分支上合并就可以了。






GitLab Flow
https://docs.gitlab.com/ce/workflow/gitlab_flow.html


master：主分支，负责记录上线版本的迭代，该分支代码与线上代码是完全一致的。
develop：开发分支，该分支记录相对稳定的版本，所有的 feature 分支和 bugfix 分支都从该分支创建。
(这2个分支是受保护的)

其他为短期分支，开发之后需要删除
feature-*: 特性/功能分支
bugfix-*: bug修复分支(不紧急)
release-*: 发布分支
hotfix-*: 紧急bug修复分支


人员分工
开发人员： a、b  负责代码功能开发，bug修复
开发 leader：c   负责review代码，合并代码到develop和master分支
测试人员： d     测试功能或bug修复是否正常
部署人员：e      部署代码到线上



1. leader 创建代码仓库，主分支和开发分支设为受保护
   然后开发者clone这个库下来
git clone ssh://user@host/path/to/repo.git

因为git clone默认会把远程仓库整个clone下来，但只会在本地默认创建一个master分支，
因此我们还需要把develop分支从远程分支取到本地
git checkout -b develop origin/develop


2、开发者基于develop分支，创建各自的功能分支
git checkout -b feature-1 develop
然后开发、编辑、提交
git status
git add <some-file>
git commit -m "Add feature-1"
git push origin feature-1


git checkout -b feature-2 develop
然后开发、编辑、提交
git status
git add <some-file>
git commit -m "Add feature-2"
git push origin feature-2

(
假如开发者2需要进入分支1进行开发
git fetch                        // 取回远端所有分支的更新
git checkout feature-1           // 切换到要进行开发的分支
git pull origin feature-1        // 拉取远端分支

这里少用git pull，多用git fetch和merge
	git fetch origin feature-1
	git log -p feature-1.. origin/feature-1
	git merge origin/feature-1
	
然后在进行开发和提交。

如果merge时有冲突，解决完冲突在提交
)



3、功能开发完后，在页面发起merge request，请求合并到develop分支。
功能分支合并到develop分支后需要删除本地分支
git branch -d feature-1 
由leader审核并合并代码


4、准备发布，创建release分支 (由开发leader创建)
git checkout -b release-0.2 develop
git push origin release-0.2            // 推送到远端后，测试人员和开发人员可以使用
后续所有相关的测试和修改都需要在这个分支进行

测试通过后，开发leader将release分支合并到develop分支和master分支。
并对master分支打上tag







3、功能开发完后，合并到develop分支
git pull --rebase origin develop
git checkout develop
git merge --no--ff some-feature
git push -u origin develop
git branch -d some-feature

4、准备发布 (如果是gitlab，第4、5步不需要做，直接在页面发起pull request)
git checkout -b release-0.1 develop

5、完成发布
git checkout master
git merge --no-ff release-0.1
git push -u origin  master
git checkout develop
git merge --no-ff release-0.1
git push
git branch -d release-0.1

发布分支是作为功能开发（develop分支）和对外发布（master分支）间的缓冲。只要有合并到master分支，就应该打好Tag以方便跟踪。
git tag -a 0.1 -m "Initial public release" master
git push --tags
Git有提供各种勾子（hook），即仓库有事件发生时触发执行的脚本。 可以配置一个勾子，在你push中央仓库的master分支时，自动构建好对外发布。








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




11.一旦远程主机的版本库有了更新（Git术语叫做commit），需要将这些更新取回本地，
这时就要用到git fetch命令。

    $ git fetch <远程主机名>

上面命令将某个远程主机的更新，全部取回本地。
默认情况下，git fetch取回所有分支（branch）的更新。如果只想取回特定分支的更新，
可以指定分支名。

    $ git fetch <远程主机名> <分支名>

比如，取回origin主机的master分支

    $ git fetch origin master

所取回的更新，在本地主机上要用”远程主机名/分支名”的形式读取。比如origin主机的
master，就要用origin/master读取。

git branch命令的-r选项，可以用来查看远程分支，-a选项查看所有分支。

    $ git branch -r
      origin/master
 
    $ git branch -a
      * master
      remotes/origin/master

上面命令表示，本地主机的当前分支是master，远程分支是origin/master。

--------------------------------------------------------------------------------

团队开发的模式有：
Centralized Workflow（在主分支上进行开发）
Feature Branch Workflow（在其它分支上进行开发）

http://nvie.com/posts/a-successful-git-branching-model/

https://www.atlassian.com/git/tutorials/comparing-workflows/#!workflow-overview

http://blog.jobbole.com/76843/

http://blog.csdn.net/self001/article/details/7333096

http://my.oschina.net/u/150705/blog/191534

http://blog.csdn.net/kasagawa/article/details/6797812 (V)








第一次管理员创建项目，设置好后（主分支）
各个开发人员（如，A、B）从项目仓库clone到本地
这样，每位参与的组员也有了与代码库版本一致的第一份代码。

A、B开始进行各自开发

假设A首先完成任务，需要将修改提交到服务器
然后将新修改提交到本地
git add 
git commit
然后将服务器的最新版本取回本地与主分支（或其它）合并
git pull origin master
（由于可以确保服务器一开始和本地是一样的，所以这一步其实可以跳过）
最后提交到服务器。
git push


B开发完后，需要将服务器的最新版本取回本地与主分支（或其它）合并
git pull origin master
（此时，由于A已经更新过服务器，所以与B的版本不同，因此将会有冲突，
所以B需要解决冲突）
解决好冲突后，B再提交到服务器。


接着到A的第二个任务完成了，
A需要将服务器的最新版本取回本地与主分支（或其它）合并
...
以此类推


rebase这东西，能不用尽量不用，可能会导致一些混乱。

推荐使用 git pull --rebase origin master
解决完冲突后，使用
git add
git rebase --continue

如果你碰到了冲突，但发现搞不定，不要惊慌。只要执行下面这条命令，
就可以回到你执行git pull --rebase命令前的样子：
git rebase --abort

1.新建一个分支，并且代码和服务器中代码同步
   git checkout origin/v2.0 -b temp  
2.为了保证新建的temp分支代码是最新的，可以多执行下面一步
  git pull
3.当你新建分支后，系统会自动checkout到temp分支上，此时
  git checkout  new
4.合并代码，并整理
  git rebase  temp  //会将temp分支的代码合并过来，并按照提交的顺序排序
5.  因为顺序是重新整理的，所以肯定会出现冲突
6.解决冲突，最后 git add * ，但不许要git commit
7.解决后，执行 git rebase --continue
8.重新提交代码： git push for-*





从git中删除文件：
git rm file1
git rm -r dir1


想要恢复到某个版本的代码或者撤销某个操作
回滚代码：

git revert HEAD
你也可以revert更早的commit

例如：git revert HEAD^

想要恢复到某个版本的代码就用这个git reset命令：

git reset





Feature Branch Workflow
步骤                      Git 操作
克隆代码                  git clone 远程代码
创建分支                  git checkout -b branch_name
在分支中开发              无
review代码                无
第一轮测试                无
添加代码到分支的暂存区    git add somefile
提交代码到分支            git commit -m "本次提交的注释"
切换到主版本              git checkout master
获取远程最新代码          git pull origin master
合并某分支到master分支    git merge branch_name
解决合并时产生的冲突      请参考分支合并时冲突的解决
第二轮测试                无
准备上线文档              无
获取远程最新代码          git pull origin master
推送master分支            git push origin master
通知上线                  无
没有问题了删除本地分支    git branch -d branch_name


--------------------------------------------------------------------------------

git 解决冲突


树冲突

文件名修改造成的冲突，称为树冲突。
比如，a用户把文件改名为a.c，b用户把同一个文件改名为b.c，
那么b将这两个commit合并时，会产生冲突。
$ git status
    added by us:    b.c
    both deleted:   origin-name.c
    added by them:  a.c
如果最终确定用b.c，那么解决办法如下：
git rm a.c
git rm origin-name.c
git add b.c
git commit
执行前面两个git rm时，会告警“file-name : needs merge”，可以不必理会。
 
树冲突也可以用git mergetool来解决，但整个解决过程是在交互式问答中完成的，
用d 删除不要的文件，用c保留需要的文件。最后执行git commit提交即可。




内容冲突的解决办法

直接编辑冲突文件

冲突产生后，文件系统中冲突了的文件（这里是test.txt）里面的内容会显示为类似下面这样：
a123
<<<<<<< HEAD
b789
=======
b45678910
>>>>>>> 6853e5ff961e684d3a6c02d4d06183b5ff330dcc
c
其中：冲突标记<<<<<<< （7个<）与=======之间的内容是我的修改，=======与>>>>>>>之间的内容是别人的修改。
此时，还没有任何其它垃圾文件产生。




每人克隆原项目后，开一条分支，在分支下进行开发，并提交到本地分支上，
(git status, git add, git commit
git push -u origin feature-branch --> 也就是把分支提交到服务器上
)
（
当要提交到原项目时，需要先使用git pull把原项目的更新取回本地，
然后使用git merge把自己的分支合并到主分支上，解决好冲突后，再提交
到原项目上。
）
当要提交到原项目时
git checkout master
git pull
git pull origin feature-branch
git push
（其实可以用merge）

git add --all
git commit 

git checkour master
git merge hwq


--------------------------------------------------------------------------------

参与到某个项目
中
在 GitHub 上 fork 到自己的仓库，如 docker_user/docker_practice，然后 clone 到本地，并设置用户信息。
$ git clone git@github.com:docker_user/docker_practice.git
$ cd docker_practice
$ git config user.name "yourname"
$ git config user.email "your email"
修改代码后提交，并推送到自己的仓库。
$ #do some change on the content
$ git commit -am "Fix issue #1: change helo to hello"
$ git push
在 GitHub 网站上提交 pull request。
定期使用项目仓库内容更新自己仓库内容。
$ git remote add upstream https://github.com/yeasy/docker_practice
$ git fetch upstream
$ git checkout master
$ git rebase upstream/master
$ git push -f origin master

--------------------------------------------------------------------------------

Git安装与设置

1.windows下载安装 http://git-scm.com/downloads
  fedora下：yum install git

2.windows下设置 http://blog.csdn.net/self001/article/details/7337182
  Linux下不需要

2.1、
C:\Program Files\Git\etc\git-completion.bash：
alias ls='ls --show-control-chars --color=auto'

说明：使得在 Git Bash 中输入 ls 命令，可以正常显示中文文件名。

2.2、不推荐使用中文commit
C:\Program Files\Git\etc\inputrc：
set output-meta on
set convert-meta off

说明：使得在 Git Bash 中可以正常输入中文，比如中文的 commit log。

2.3、
C:\Program Files\Git\etc\profile：
export LESSCHARSET=utf-8

说明：$ git log 命令不像其它 vcs 一样，n 条 log 从头滚到底，它会恰当地停在
第一页，按 space 键再往后翻页。这是通过将 log 送给 less 处理实现的。以上即是
设置 less 的字符编码，使得 $ git log 可以正常显示中文。

2.4、
C:\Program Files\Git\etc\gitconfig：
[gui]
encoding = utf-8

说明：我们的代码库是统一用的 utf-8，这样设置可以在 git gui 中正常显示代码中的
中文。

[i18n]
commitencoding = GB2312

说明：如果没有这一条，虽然我们在本地用 $ git log 看自己的中文修订没问题，但，
一、我们的 log 推到服务器后会变成乱码；二、别人在 Linux 下推的中文 log 我们 
pull 过来之后看起来也是乱码。这是因为，我们的 commit log 会被先存放在项目
的 .git/COMMIT_EDITMSG 文件中；在中文 Windows 里，新建文件用的是 GB2312 的
编码；但是 Git 不知道，当成默认的 utf-8 的送出去了，所以就乱码了。有了这条
之后，Git 会先将其转换成 utf-8，再发出去，于是就没问题了。

2.5、
可以设置git默认为其它编辑器，在 git bash 命令行下
$ git config --global core.editor "notepad++"

其中 notepad 可以替换为更好用的 wordpad、notepad++ 等（不过它们在命令行里
无法直接访问，得先设置 PATH 变量）。

2.6、

git bash 进行粘贴
在Bash命令行界面上右键点击左上角，在出现的菜单中点击properties，
然后在quickedit上打上对钩，这样就ok了。

在 git bash 命令行下
git config --global gui.encoding utf-8


3. 用户名（Linux是在普通用户下）
git config --global user.name "你的名字"

4. Email（Linux是在普通用户下）
git config --global user.email "你的Email"


5.缓存账号

Tell git that you don't want to type your username and password every time you
talk to a remote server:

$ git config --global credential.helper cache

By default git will cache your password for 15 minutes.
You can change this if you like:

$ git config --global credential.helper 'cache --timeout=3600'

This will set the cache to timeout after 1 hour (setting is in seconds)


6.git osc 添加SSH Keys

你可以按如下命令来生成sshkey（在git bash下）
ssh-keygen -t rsa -C "xxxxx@xxxxx.com"
# Creates a new ssh key using the provided email

查看你的public key
cat ~/.ssh/id_rsa.pub

进入http://git.oschina.net/keys/new，添加公钥（粘贴你刚才复制的）,保存

添加后，在终端（Terminal）中输入
ssh -T git@git.oschina.net
若返回
Welcome to Git@OSC, yourname! 
则证明添加成功。

6.github 添加SSH Keys

你可以按如下命令来生成sshkey
ssh-keygen -t rsa -C "xxxxx@xxxxx.com"

Then add your new key to the ssh-agent:
# start the ssh-agent in the background
$ eval "$(ssh-agent -s)"
Agent pid 59566
$ ssh-add ~/.ssh/id_rsa

进入 https://github.com/settings/ssh 添加公钥

添加后，在终端（Terminal）中输入
ssh -T git@github.com

返回
Hi username! You've successfully authenticated, but GitHub does not
provide shell access.
则表示成功


如果之前使用https的，要修改为SSH，只需要把项目中.git/config文件中的url改成
ssh链接即可。


--------------------------------------------------------------------------------

GitHub Pages https://pages.github.com/

1.个人或组织

新建一个repository，命名为 username.github.io ，username为你在github上的名字
或组织名。
接着clone项目，编辑，提交。
浏览器访问 http://username.github.io

2.项目主页

项目主页可以通过GitHub做好的模板来创建，或者自己从头创建。


-----

使用Jekyll来创建页面

安装Jekyll （这个是官网的 http://jekyllrb.com/docs/installation/ ）
1.Ruby - yum install ruby ruby-devel
2.安装RubyGems - https://rubygems.org/pages/download
tar -xzvf rubygems-xxx.tgz
cd rubygems
ruby setup.rb
3.Node.js - http://nodejs.org/download/
（fedora可以直接 yum install nodejs）
tar -zxvf node-xxx.tar.gz
cd node-xxx
./conigure
make
make install
4.gem install jekyll
检查版本 - jekyll --version


这个是GitHub的 - https://help.github.com/articles/using-jekyll-with-pages/
http://www.pchou.info/web-build/2014/07/04/build-github-blog-page-08.html

1.Ruby
2.Bundler - gem install bundler
3.Jekyll - 
$ yum install -y rubygem-nokogiri
( $ yum install libxslt-devel libxml2-devel
  $ gem install nokogiri
  如果用 yum install 了，这两条就不用了，fedora 21下出错，原因不知)
在repository根目录下创建名为 Gemfile 的文件，并编辑:
source 'https://rubygems.org'
gem 'github-pages'
然后在命令行下运行
$ bundle install

命令会根据当前目录下的Gemfile，安装所需要的所有软件。这一步所安装的东西，可以
说跟github本身的环境是完全一致的，所以可以确保本地如果没有错误，上传后也不会
有错误。而且可以在将来使用下面命令，随时更新环境，十分方便。
$ bundle update

使用下面命令，启动转化和本地服务：
$ bundle exec jekyll serve


使用 bundle install 命令初始化后，在Project根目录下有两个文件，一个是自己创建的
Gemfile，另外一个是生成的 Gemfile.lock
我们首先到其它目录生成一个模板先，用于自己学习和改造：
$ jekyll new blog
$ cd blog
$ jekyll serve
然后就可以在 localhost:4000 看到生成的网站了

看一下生成的 blog 目录
.
├── about.md
├── _config.yml
├── css
│   └── main.scss
├── feed.xml
├── _includes
│   ├── footer.html
│   ├── header.html
│   └── head.html
├── index.html
├── _layouts
│   ├── default.html
│   ├── page.html
│   └── post.html
├── _posts
│   └── 2015-01-14-welcome-to-jekyll.markdown
└── _sass
    ├── _base.scss
    ├── _layout.scss
    └── _syntax-highlighting.scss

首先 _config.yml 是必须的，
_layouts 是存放模板文件的，
_post 是存放blog文章的，
index.html 是首页
其实也就跟着 http://www.ruanyifeng.com/blog/2012/08/blogging_with_jekyll.html 来学



输入以下命令生成网站，就可以到localhost:4000查看了
$ bundle exec jekyll serve


-----

Jekyll入门 - http://www.ruanyifeng.com/blog/2012/08/blogging_with_jekyll.html
http://jekyllbootstrap.com/

A basic Jekyll site usually looks something like this:
.
├── _config.yml
├── _drafts
|   ├── begin-with-the-crazy-ideas.textile
|   └── on-simplicity-in-technology.markdown
├── _includes
|   ├── footer.html
|   └── header.html
├── _layouts
|   ├── default.html
|   └── post.html
├── _posts
|   ├── 2007-10-29-why-every-programmer-should-play-nethack.textile
|   └── 2009-04-26-barcamp-boston-4-roundup.textile
├── _data
|   └── members.yml
├── _site
└── index.html

