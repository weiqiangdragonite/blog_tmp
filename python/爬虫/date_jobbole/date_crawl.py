#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# 爬取 伯乐在线 面向对象
# http://date.jobbole.com/  http://date.jobbole.com/page/2/
# http://date.jobbole.com/tag/shanghai/  http://date.jobbole.com/tag/shanghai/page/2/      
# 30个对象一页

# 单进程版本
# Connection reset by peer
# 要防止被封

# https://pythonhosted.org/pyquery/
# http://www.python-requests.org/en/master/
# csv: http://blog.csdn.net/lixiang0522/article/details/7755059



import requests
from pyquery import PyQuery as pq
import csv
import time


attrs = ["超链接", "标题", "喜欢数", "出生年月", "身高", "所在城市", "籍贯", \
    "职业", "父母情况", "是否是独生子女", "收入描述", "兴趣爱好", \
    "是否接受异地恋", "打算几年内结婚", "要几个小孩（0至n）", \
    "对另一半的最低要求是", "对另一半的特殊要求是", "一句话让自己脱颖而出", \
    "照片链接"]


headers = { \
'User-Agent': 'Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/45.0.2924.87 Safari/537.36', \
'Accept-Encoding': 'gzip, deflate, sdch', \
'Accept-Language': 'zh-CN,zh;q=0.8', \
}



# 发现会有 Connection reset by peer 的情况，所以失败的话重新尝试
# 每次请求前先sleep，降低频率
# 后期可以考虑使用tor代理
def get_url_content(url):
    '''
    '''
    fail_times = 5
    counter = 0
    while True:
        try:
            time.sleep(1)
            print("Getting %s ..." % url)
            res = requests.get(url, timeout = 60, headers = headers)
            if res.status_code == requests.codes.ok:
                pass
            return res
        except Exception as e:
            print("Error:", e)
            counter += 1
            time.sleep(counter)
            if counter >= fail_times:
                return None
                break
            print("  Trying %s again ..." % url)


def parse_girl_info(text, info):
    '''
        解析信息
    '''
    key = None
    start = True

    # 注意这里用了中文的 ：
    for e in text.split('：'):
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
        解析页面
    '''
    info = {}
    response = get_url_content(url)
    if response == None:
        return info
    page = pq(response.content)
    content = page("div.p-single")

    info["超链接"] = url
    title = content("h1.p-tit-single").text()
    info["标题"] = title

    # 如果是已经撤销的
    if '【' in title and '】' in title:
        return info

    info_text = content("div.p-entry").text()
    parse_girl_info(info_text, info)

    img_links = content("div.p-entry img")
    href = []
    for link in img_links:
        href.append(pq(link).attr["src"])
    info["照片链接"] = href

    get_image(info)
    return info




def crawl(counter):
    '''
        获取面向对象列表
        counter = -1 就爬取所有
    '''
    count = 0
    start = 0
    ret_list = []
    is_stop = False
    url = "http://date.jobbole.com/page/{start}/"

    while True:
        start += 1
        response = get_url_content(url.format(start = start))
        if response == None:
            continue
        # 要不要判断是否返回200
        page = pq(response.content)
        # find the <div class="media-body"><h3 class="p-tit"><a>
        links = page("div.media-body h3.p-tit a")

        if len(links) == 0:
            is_stop = True

        for link in links:
            href = pq(link).attr["href"]
            if not href.startswith("http://date.jobbole.com/"):
                continue
            #print(href)
            ret_list.append(crawl_info(href))
            count += 1
            if counter != -1 and count >= counter:
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

        
def get_image(line):
    '''

    '''
    file = '/tmp/date_jobbole/' + line["超链接"].split('/')[-2]
    # 要注意路径要存在
    for index, link in enumerate(line["照片链接"]):
        save_image(link, file + '-' + str(index) + link[link.rfind('.'):])


def save_image(url, path):
    img = get_url_content(url)
    # 要不要判断返回是否200
    if img != None:
        with open(path, 'wb') as f:
            f.write(img.content)


if __name__ == "__main__":
    counter = input("请输入爬取的数据量: ")
    counter = int(counter) if counter.isdigit() else 10

    #ret_list = crawl(counter)
    ret_list = crawl(-1)
    write_to_file(ret_list, "result.csv")
