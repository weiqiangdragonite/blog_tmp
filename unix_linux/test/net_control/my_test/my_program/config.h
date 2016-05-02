/*
 *
 */

#ifndef CONFIG_H
#define CONFIG_H


///YS 各种策略规则的定义
#define white_list 1
#define person_list 2
#define group_list 3
#define whole_list 4
#define ALL_w_list 5
#define ALL_b_list 6



///YS 特征字相关的长度变量
#define OVECCOUNT 30
#define pcre_num 512 ///YS 协议特征字最多支持
#define patten_len 64 ///YS 特征字长度


#define Tcp 1
#define Udp 2
/*d_in   XXXXX>192.168*/
#define d_in 1  ///YS 这个数据包的出入方向
/*d_out  192.168>XXXXX*/
#define d_out 0

///YS TCPFLAG的值
#define	TH_FIN	0x01
#define	TH_SYN	0x02
#define	TH_RST	0x04
#define	TH_PUSH	0x08
#define	TH_ACK	0x10
#define	TH_URG	0x20
#define	TH_ECE	0x40
#define	TH_CWR	0x80


///YS HTTP包的话，会有这个值，这个值是在拦截时用的，TCP拦截只发RST包就可以了，但HTTP拦截，还要再发一个提示包
#define http_flag 9




///YS用于建立user 哈希表
#define GET_user_gid_list "select User_IP, Gid from user"

///YS 查询协议的特征字
#define GET_feature_list "select name,feature from feature where feature !=''"

///YS TCP应用里，有的应用直接用端口的
#define GET_port_list "select port_or_ip from app_table where port_or_ip !=''"

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

///YS 审计插入数据到数据库的SQL
#define INSERT_STREAM  "INSERT INTO `tcp_stream` (`stream_index`) VALUES ('%s');"
#define INSERT_AUDIT_SEARCH  "INSERT INTO `audit_search` (`Key_only`,`Name`,`Uid`,`User_IP`,`Http_website`,`Time`,`Detail`,`Gid`)VALUES ('%s','%s','%s','%s','%s','%s','%s','%s');"
#define INSERT_AUDIT_EMAIL  "INSERT INTO `audit_email` (`Key_only`,`Name`,`Uid`,`User_IP`,`Email_type`,`Time`,`Detail`,`receiver`,`sender`,`title`,`Gid`)VALUES ('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s');"
#define INSERT_AUDIT_HTTP_HOST "INSERT INTO `audit_http` (`Name`,`User_IP`,`Uid`,`Time`,`Detail`,`Key_only`,`Referer`,`Gid`)VALUES ('%s','%s','%s','%s','%s','%s','%s','%s');"
#define INSERT_AUDIT_BBS "INSERT INTO `audit_bbs` (`Name`,`User_IP`,`Uid`,`Time`,`Detail`,`Key_only`,`Url`,`Poster`,`Title`,`Host`,`Gid`)VALUES ('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s');"
#define INSERT_AUDIT_U_OR_D "INSERT INTO `attachment` (`Name`,`IP`,`Uid`,`Datetime`,`U_or_D`,`file_name`,`Gid`)VALUES ('%s','%s','%s','%s','%d','%s','%s');"
#define INSERT_AUDIT_IM "INSERT INTO `audit_im` (`Name`,`User_IP`,`Uid`,`Im_type`,`Time`,`Sender`,`Receiver`,`Detail`,`Comment`,`Gid`)VALUES('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s');"
#define INSERT_AUDIT_EMAIL_READ "INSERT INTO `audit_email_read` (`Name`,`Uid`,`User_IP`,`Datetime`,`file_name`,`Gid`)VALUES('%s','%s','%s','%s','%s','%s');"
#define INSERT_AUDIT_EMAIL_POP3 "INSERT INTO `audit_email_read` (`Name`,`Uid`,`User_IP`,`Datetime`,`Gid`,`Detail`,`Receiver`,`Sender`,`Title`,`Is_Pop3`)VALUES('%s','%s','%s','%s','%s','%s','%s','%s','%s','%d');"
#define INSERT_FLOW "INSERT INTO `flow_statistics` (`Name`,`User_IP`,`Uid`,`Gid`,`Flow`,`Block`,`Flow_limit`)VALUES('%s','%s','%s','%s','%ld','%d','%s');"
#define UPDATE_FLOW "UPDATE `flow_statistics` set `Flow`='%ld',`Block`='%d'where `User_IP`='%s';"
#define PUT_FILE_NAME_2_DB "INSERT INTO `file_name_tmp` (`name`, `ACK`, `create_time`,`stream`) VALUES ('%s', '%x', '%d','%s'); "


///从数据库里读取配置数据
#define GET_CONFIG_DEBUG  "select config_data from `system_configure` WHERE (`Config_name`='debug');"
#define GET_CONFIG_SUB_NET  "select config_data from `system_configure` WHERE (`Config_name`='SUB_NET');"
#define GET_CONFIG_NET_INTERFACE  "select config_data from `system_configure` WHERE (`Config_name`='NET_interface_2_re');"
#define GET_CONFIG_http_enable  "select config_data from `system_configure` WHERE (`Config_name`='http_enable');"
#define GET_CONFIG_search_enable  "select config_data from `system_configure` WHERE (`Config_name`='search_enable');"
#define GET_CONFIG_bbs_enable  "select config_data from `system_configure` WHERE (`Config_name`='bbs_enable');"
#define GET_CONFIG_email_enable  "select config_data from `system_configure` WHERE (`Config_name`='email_enable');"
#define GET_CONFIG_arp_mac "select config_data from `system_configure` WHERE (`Config_name`='arp_mac');"
#define GET_CONFIG_arp_source_ip "select config_data from `system_configure` WHERE (`Config_name`='arp_source');"

#define GET_CONFIG_db_ip "select config_data from `system_configure` WHERE (`Config_name`='DBSERV_IP');"
#define GET_CONFIG_ex_ip "select config_data from `system_configure` WHERE (`Config_name`='ex_ip');"
#define GET_CONFIG_ex_ip1 "select config_data from `system_configure` WHERE (`Config_name`='ex_ip1');"
#define GET_CONFIG_ex_ip2 "select config_data from `system_configure` WHERE (`Config_name`='ex_ip2');"


#define GET_feature_list "select Filetype from file_header where Enable =1"
#define GET_flow_list "select User_IP,Flow,Flow_limit from flow_statistics"
#define GET_file_name_header_list "select Filetype,Fileheader from file_header where enable =1"
#define GET_app_im_feature_list "select * from im_feature"
#define SQL_GET_PORT "select sport,name,offset_type,offset from port_table_statistics where enable =1"
#define GET_NAME_FEATURE "select name,start,lenth,lenth_type from file_name_feature"
#define GET_FILE_NAME_USE_ACK_OR_STREAM "select name from file_name_tmp where ACK='%x' or stream='%s';"
#define GET_FILE_NAME_USE_STREAM "select name from file_name_tmp where stream='%s';"
#define GET_USER_NAME_UID "select Name,Uid,Gid,Flow from user where User_IP ='%s';"
#define DEL_FILE_NAME  "DELETE FROM `file_name_tmp` WHERE (`name`='%s');"

#define GET_CONFIG_SERVERIP  "select config_data from `system_configure` WHERE (`Config_name`='serverIP');"
#define GET_CONFIG_net_version "select config_data from `system_configure` WHERE (`Config_name`='version_re');"
#define GET_CONFIG_LOGUPDATESETTING  "select config_data from `system_configure` WHERE (`Config_name`='logUpdateSetting');"
#define GET_CONFIG_LOG  "select config_data from `system_configure` WHERE (`Config_name`='log');"
#define GET_CONFIG_DBSERV_IP  "select config_data from `system_configure` WHERE (`Config_name`='DBSERV_IP');"
#define GET_CONFIG_LOGEFFECTIVEDAYS  "select config_data from `system_configure` WHERE (`Config_name`='logEffectiveDays');"

#endif	/* CONFIG_H */
