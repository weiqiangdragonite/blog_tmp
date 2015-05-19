#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
这个脚本看起来很复杂，其实做的东西真不多。SQLAlchemy-migrate通过对比数据库的
结构（从app.db文件读取）和models结构（从app/models.py文件读取）的方式来创建
迁移任务，两者之间的差异将作为一个迁移脚本记录在迁移库中，迁移脚本知道如何
应用或者撤销一次迁移，所以它可以方便的升级或者降级一个数据库的格式。

虽然我使用上面的脚本自动生成迁移时没遇到什么问题，但有时候真的很难决定数据库
旧格式和新格式究竟有啥改变。为了让SQLAlchemy-migrate更容易确定数据库的改变，
我从来不给现有字段重命名，限制了添加删除models、字段，或者对现有字段的类型修改。
我总是检查下生成的迁移脚本是否正确。

不用多讲，在你试图迁移数据库前必须做好备份，以防出现问题。不要在生产用的数据库
上运行第一次使用的脚本，先在开发用的数据库上运行下。
"""

import imp
from app import db
from migrate.versioning import api
from config import SQLALCHEMY_DATABASE_URI
from config import SQLALCHEMY_MIGRATE_REPO

ver = api.db_version(SQLALCHEMY_DATABASE_URI, SQLALCHEMY_MIGRATE_REPO) 
migration = SQLALCHEMY_MIGRATE_REPO + '/versions/%03d_migration.py' % (ver + 1)

tmp_module = imp.new_module('old_model')
old_model = api.create_model(SQLALCHEMY_DATABASE_URI, SQLALCHEMY_MIGRATE_REPO)

exec old_model in tmp_module.__dict__

script = api.make_update_script_for_model(SQLALCHEMY_DATABASE_URI, \
    SQLALCHEMY_MIGRATE_REPO, tmp_module.meta, db.metadata)

open(migration, "wt").write(script)
api.upgrade(SQLALCHEMY_DATABASE_URI, SQLALCHEMY_MIGRATE_REPO)

ver = api.db_version(SQLALCHEMY_DATABASE_URI, SQLALCHEMY_MIGRATE_REPO)
print 'New migration saved as ' + migration
print 'Current database version: ' + str(ver)

