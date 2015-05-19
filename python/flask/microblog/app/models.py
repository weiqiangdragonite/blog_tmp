#!/usr/bin/env python
# -*- coding: utf-8 -*-

from app import db
from hashlib import md5
from urllib import urlencode

ROLE_USER = 0
ROLE_ADMIN = 1

# we use the lower level APIs in flask-sqlalchemy to create the table without
# an associated model
followers = db.Table("followers",
    db.Column("follower_id", db.Integer, db.ForeignKey("user.id")),
    db.Column("followed_id", db.Integer, db.ForeignKey("user.id"))
)

class User(db.Model):
    __tablename__ = "users"
    id = db.Column(db.Integer, primary_key = True)
    nickname = db.Column(db.String(64), index = True, unique = True)
    email = db.Column(db.String(120), index = True, unique = True)
    role = db.Column(db.SmallInteger, default = ROLE_USER)
    posts = db.relationship("Post", backref = "author", lazy = "dynamic")
    about_me = db.Column(db.String(140))
    last_seen = db.Column(db.DateTime)
    followed = db.relationship("User",
        secondary = followers,
        primaryjoin = (followers.c.follower_id == id),
        secondaryjoin = (followers.c.follower_id == id),
        backref = db.backref("followers", lazy = "dynamic"),
        lazy = "dynamic")


    # __repr__方法告诉Python如何打印class对象，方便我们调试使用。
    def __repr__(self):
        return "<User %r>" % (self.nickname)

    def is_authenticated(self):
        return True

    # is_active方法应该为用户返回True除非用户不是激活的，例如，他们已经被禁了
    def is_active(self):
        return True

    # is_anonymous方法应该为那些不被获准登录的用户返回True
    def is_anonymous(self):
        return False

    # get_id方法为用户返回唯一的unicode标识符。我们用数据库层生成唯一的id
    def get_id(self):
        return unicode(self.id)

    # 这里就使用Gravatar给咋们提供的用户头像即可
    def avatar(self, size):
        return 'http://www.gravatar.com/avatar/%s?d=mm&s=%d' % \
               (md5(self.email.encode('utf-8')).hexdigest(), size)

    # this operation does not apply to any particular instance of the class.
    @staticmethod
    def make_unique_nickname(nickname):
        if User.query.filter_by(nickname = nickname).first == None:
            return nickname
        version = 2
        while True:
            new_nickname = nickname + str(version)
            if User.query.filter_by(nickname = new_nickname).first() == None:
                break;
            version += 1
        return new_nickname

    #
    def follow(self, user):
        if not self.is_following(user):
            self.followed.append(user)
            return self

    #
    def unfollow(self, user):
        if self.is_following(user):
            self.followed.remove(user)
            return self

    #
    def is_following(self, user):
        return self.followed.filter(followers.c.followed_id == user.id).count > 0

    #
    def followed_posts(self):
        # There are three parts: the join, the filter and the order_by.
        return Post.query.join(followers, (followers.c.followed_id == Post.user_id)).filter(followers.c.follower_id == self.id).order_by(Post.timestamp.desc())


class Post(db.Model):
    __tablename__ = "posts"
    id = db.Column(db.Integer, primary_key = True)
    body = db.Column(db.String(140))
    timestamp = db.Column(db.DateTime)
    user_id = db.Column(db.Integer, db.ForeignKey("users.id"))

    def __repr__(self):
        return "<Post %r>" % (self.body)


