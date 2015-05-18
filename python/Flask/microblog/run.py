#!/usr/bin/env python
# -*- coding: utf-8 -*-

# http://blog.miguelgrinberg.com/post/the-flask-mega-tutorial-part-i-hello-world
# http://www.oschina.net/translate/the-flask-mega-tutorial-part-i-hello-world

from app import app

if __name__ == "__main__":
    app.run(host="0.0.0.0", debug=True)
