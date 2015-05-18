#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sqlite3
from contextlib import closing
from flask import Flask, request, session, g, redirect, url_for, abort, \
                  render_template, flash


# create our little application :)
app = Flask(__name__)

# configuration
# os.path.abspath('.')
DATABASE = os.path.join(app.root_path, 'flaskr.db')
DEBUG = True
SECRET_KEY = 'development key'
USERNAME = 'admin'
PASSWORD = 'default'

app.config.from_object(__name__)
app.config.from_envvar("FLASKR_SETTINGS", slient = True)


def connect_db():
    """ """
    return sqlite3.connect(app.config['DATABASE'])

def init_db():
    """
    >>> from flaskr import init_db
    >>> init_db()
    """
    with closing(connect_db()) as db:
        with app.open_resource('schema.sql', mode='r') as f:
            db.cursor().executescript(f.read())
        db.commit()

@app.before_request
def before_request():
    g.db = connect_db()

@app.teardown_request
def teardown_request(exception):
    db = getattr(g, 'db', None)
    if db is not None:
        db.close()
    g.db.close()


# Æô¶¯·þÎñÆ÷
if __name__ == "__main__":
    app.run(host = "0.0.0.0", debug = app.config["DEBUG"])

