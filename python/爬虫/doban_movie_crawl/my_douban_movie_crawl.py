#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# https://pythonhosted.org/pyquery/
# http://www.python-requests.org/en/master/
# csv: http://blog.csdn.net/lixiang0522/article/details/7755059

import requests
from pyquery import PyQuery as pq
import csv


attrs = ["超链接", "名称", "评分", "导演", "编剧", "主演", "类型", \
    "制片国家/地区", "语言", "上映日期", "片长", "又名", "IMDb链接"]



def parse_movie_info(text, info):
    '''
        解析电影信息
    '''
    key = None
    start = True

    for e in text.split(':'):
        e = e.strip()
        pos = e.rfind(' ')
        if pos == -1 and start is True:
            key = e
            start = False
        elif pos == -1 and start is False:
            info[key] = e
        else:
            info[key] = e[:pos]
            key = e[pos+1:]



def crawl_info(url):
    '''
        解析电影页面
    '''
    info = {}
    response = requests.get(url)
    page = pq(response.content)
    content = page("div#content").eq(0)

    info["超链接"] = url
    info["名称"] = content("h1 span").eq(0).text()
    info["评分"] = content("div.rating_wrap strong.rating_num").text()

    info_text = content("div#info").text()
    parse_movie_info(info_text, info)

    print(info)
    return info



def crawl(query_text, counter):
    '''
        获取电影列表
    '''
    start = 0
    ret_list = []
    is_stop = False
    url = "https://movie.douban.com/subject_search?start={start}&search_text={query_text}&cat=1002"

    while True:
        response = requests.get( \
            url.format(start = start, query_text = query_text))
        page = pq(response.content)
        # find the <a> tag in <div> id = content, <table> without the .nbg class
        links = page("div#content table a").not_(".nbg")
        if len(links) == 0:
            is_stop = True

        for link in links:
            href = pq(link).attr["href"]
            ret_list.append(crawl_info(href))
            start += 1
            if len(ret_list) >= counter:
                is_stop = True
                break

        if is_stop:
            break

    return ret_list



def write_to_file(info, path):
    '''
        写入文件
    '''
    with open(path, 'w') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(attrs)
        for line in info:
            row = []
            for key in attrs:
                row.append(line.get(key, ''))
            writer.writerow(row)




if __name__ == "__main__":
    query_text = input("请输入关键字: ")
    counter = input("请输入爬取的数据量: ")

    query_text = query_text.strip() if query_text.strip() else "长城"
    counter = int(counter) if counter.isdigit() else 10

    print("关键字: %s, 数量: %d" % (query_text, counter))

    ret_list = crawl(query_text, counter)
    write_to_file(ret_list, "result.csv")
