#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import requests
import json

URL = "https://api.github.com"

def build_uri(endpoint):
    return '/'.join([URL, endpoint])

def better_json(json_str):
    return json.dumps(json.loads(json_str), indent = 4)

def request_method():
    #response = requests.get(build_uri("users/weiqiangdragonite"))
    response = requests.get(build_uri("user/emails"))
    #print(response.status_code)
    #print(response.headers)
    print(better_json(response.text))

if __name__ == "__main__":
    request_method()
