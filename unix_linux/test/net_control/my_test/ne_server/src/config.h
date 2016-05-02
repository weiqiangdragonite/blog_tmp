///YS 配置文件
#ifndef CONFIG_H
#define CONFIG_H
#define MAXBYTES2CAPTURE 1600  ///YS 抓包的长度
#define dump_file "/home/workspace/networkbehavior/tmp" ///YS 抓包后的结果保存位置，现在没用

///YS 字符串长度
#define cache_sql_str_len 512
#define public_function_cache_str_len 512
#define http_process_str_len 256
#define public_function_http_str_len 128
#define tcp_process_strtmp_len 64

///YS 特征字相关的长度变量
#define OVECCOUNT 30
#define pcre_num 512 ///YS 协议特征字最多支持
#define patten_len 64 ///YS 特征字长度

///YS HTTP拦截BUFF的长度
#define http_content_len 2120
#define http_ban_title "ban!"///YS 拦截网页的标题



///YS 各种策略规则的定义
#define white_list 1
#define person_list 2
#define group_list 3
#define whole_list 4
#define ALL_w_list 5
#define ALL_b_list 6


///YS 定义数据库操作的宏
#define GET_URL_white_list "select Detail , User_IP from tactics_allow where Tactics_type = 'url' and Enable=1;"
#define GET_URL_person_list "select Detail , User_IP from tactics_user where Tactics_type = 'url' and Enable=1;"
#define GET_URL_group_list "select Detail, Gid from tactics_user_group where Tactics_type = 'url' and Enable=1;"
#define GET_URL_whole_list "select Detail from tactics_global where Tactics_type = 'url' and Enable=1;"
///YS HTTP ALL，HTTP大类的
#define SELECT_FROM_URL_GROUP "select Url from `%s` ;"
#define GET_URL_ALL_w_list "select User_IP from tactics_allow where Tactics_type = 'url' and Enable=1 and Detail='all';"
#define GET_URL_USER_ALL_b_list "select User_IP from tactics_user where Tactics_type = 'url' and Enable=1 and Detail='all';"
#define GET_URL_GROUP_ALL_b_list "select user.User_IP from user,tactics_user_group where tactics_user_group.Tactics_type = 'url' and tactics_user_group.Enable=1 and tactics_user_group.Detail='all' and tactics_user_group.Gid=user.Gid;"
///YS TCP 拦截相关的，从数据库读出对应的TCP拦截策略
#define GET_IP_white_list "select tactics_type ,detail,User_IP from tactics_allow where Enable=1"
#define GET_IP_person_list "select tactics_type ,detail,User_IP from tactics_user where Enable=1"
#define GET_IP_group_list "select tactics_type ,detail,gid from tactics_user_group where Enable=1"
#define GET_IP_whole_list "select tactics_type ,detail from tactics_global where Enable=1"

///YS 系统配置的
#define GET_CONFIG_WEB_TIPS  "select config_data from `system_configure` WHERE (`Config_name`='WEB_TIPS');"
#define GET_CONFIG_MY_DEV "select config_data from `system_configure` WHERE (`Config_name`='NET_interface');"
#define GET_CONFIG_DBUG "select config_data from `system_configure` WHERE (`Config_name`='debug');"
#define GET_CONFIG_cb_http_a_Size "select config_data from `system_configure` WHERE (`Config_name`='cb_http_a_Size');"
#define GET_CONFIG_cb_http_b_Size "select config_data from `system_configure` WHERE (`Config_name`='cb_http_b_Size');"
#define GET_CONFIG_cache_count_time "select config_data from `system_configure` WHERE (`Config_name`='cache_count_time');"
#define GET_CONFIG_cache_num_limit "select config_data from `system_configure` WHERE (`Config_name`='cache_num_limit');"
#define GET_CONFIG_sub_net "select config_data from `system_configure` WHERE (`Config_name`='SUB_NET');"
#define GET_CONFIG_db_ip "select config_data from `system_configure` WHERE (`Config_name`='DBSERV_IP');"
#define GET_CONFIG_ex_ip "select config_data from `system_configure` WHERE (`Config_name`='ex_ip');"
#define GET_CONFIG_ex_ip1 "select config_data from `system_configure` WHERE (`Config_name`='ex_ip1');"
#define GET_CONFIG_ex_ip2 "select config_data from `system_configure` WHERE (`Config_name`='ex_ip2');"
#define GET_CONFIG_net_version "select config_data from `system_configure` WHERE (`Config_name`='version');"
#define GET_CONFIG_arp_source_ip "select config_data from `system_configure` WHERE (`Config_name`='arp_source');"
#define GET_CONFIG_arp_mac "select config_data from `system_configure` WHERE (`Config_name`='arp_mac');"
#define GET_CONFIG_pro_thread_num "select config_data from `system_configure` WHERE (`Config_name`='pro_thread_num');"

///YS TCP应用里，有的应用直接用端口的
#define GET_port_list "select port_or_ip from app_table where port_or_ip !=''"

///YS HTTP包的话，会有这个值，这个值是在拦截时用的，TCP拦截只发RST包就可以了，但HTTP拦截，还要再发一个提示包
#define http_flag 9

///YS用于建立user 哈希表
#define GET_user_gid_list "select User_IP,Gid from user"

///YS 查询协议的特征字
#define GET_feature_list "select name,feature from feature where feature !=''"



























#endif
