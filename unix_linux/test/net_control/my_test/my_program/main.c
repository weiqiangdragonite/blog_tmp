/*
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <mysql.h>
#include <pthread.h>
#include <glib.h>

#include "circular_buffer.h"
#include "config.h"




/* local config */
char *local_ip = "localhost";
char *db_ip = "localhost";
char *db_name = "mydb";
char *db_user = "dragonite";
char *db_pwd = "dragonite";

MYSQL mysql;///ys定义数据库连接的句柄，它被用于几乎所有的MySQL函数
MYSQL my_mysql;///ys这个MYSQL句柄是用于线程池的线程向数据库插入服务器IP（YY，QQ的服务器IP）



extern GHashTable *http_hash_white_list; ///ys http 白名单 ip+hosturl
extern GHashTable *http_hash_person_list;///ys http 个人黑名单 ip+hosturl
extern GHashTable *http_hash_group_list;///ys http 组黑名单 usergroup+hosturl
extern GHashTable *http_hash_whole_list;///ys http 全局黑名单 hosturl
extern GHashTable *http_hash_ALL_w_list;///ys htpp all white，选择ALL，其中一个URL分类整个全选，使用ALL ，白名单
extern GHashTable *http_hash_ALL_b_list;///ys htpp all black ，黑名单
extern GHashTable *user_hash_list;
GHashTable *port_feature_hash_list;///ys 暂时没用了

/****for tcp port or pcre****/
extern GHashTable *tcp_hash_white_list;///ys TCP白名单 IP＋DST—IP
extern GHashTable *tcp_hash_person_list;///ys 个人白名单 IP＋DST—IP
extern GHashTable *tcp_hash_group_list;///ys gid + Dst_ip
extern GHashTable *tcp_hash_whole_list;///ys Dst_ip
extern GHashTable *tcp_app_tactics_hash_list;///ys 这个哈希表是用于查APP对应的过滤条件，KEY：YY－－－》VALUE：BLACKLIST
extern GHashTable *tcp_port_list;///ys 查APP对应的端口 KEY：FTP－》23


extern GHashTable *tcp_stream_list;




/* system config */
unsigned int cb_catch_size = 300;
unsigned int cb_audit_size = 300;


int threads_num = 10;
GThreadPool *http_process_pool = NULL;
GThreadPool *tcp_process_pool = NULL;
GError *gerror = NULL;


/* main */
void http_pcap(void);
void tcp_pcap(void);
void http_process(void);
void tcp_process(void);

extern void get_syn_and_build_stream(void);


/* public */
void user_hase_table_create(void);
void feature_hase_table_create(void);
void http_hase_table_create(void);
void tcp_port_hase_table_create(void);
void tcp_app_tactics_hase_table_create(void);
void tcp_hase_table_create(void);

void free_data(gpointer hash_data);


/* mysql */
int mysql_connect(MYSQL *mysql);
int sql_get_user_to_hash(char *SELECT_QUERY, MYSQL *sock, GHashTable *hash_table);
int sql_get_feature_to_pcre(char *SELECT_QUERY, MYSQL *sock, GHashTable *hash_table);
int sql_get_website_to_hash(char *SELECT_QUERY,MYSQL *sock,int http_type,GHashTable * hash_table);
int sql_get_tcp_port_to_hash(char *SELECT_QUERY, MYSQL *sock, GHashTable * hash_table);
int sql_get_server_ip_to_hash(char *SELECT_QUERY,MYSQL *sock,GHashTable * hash_table,int type);





int
main(int argc, char *argv[])
{
	int result;


	/* 1. get local configure file */
	/// 获取本地配置文件内容，即数据库的地址，用户名，密码等
	//get_local_config_file();

	/* 2. init mysql */
	mysql_connect(&mysql);///ys 初始化SQL，几乎用于全局的SQL语句
	mysql_connect(&my_mysql);///ys 初始化SQL，只用于插入服务器IP到数据库表里

	/* 3. get system configure */
	//system_configure();///ys 初始化系统配置

	/* 4. define the pthread */
	pthread_t http_pcap_thread;
	pthread_t tcp_pcap_thread;
	pthread_t udp_pcap_thread;
	pthread_t rebuild_packet_thread;



	/* 5. clear the circular buffer */
	memset(&cb_http_catch, 0, sizeof(CircularBuffer));
	memset(&cb_tcp_catch, 0, sizeof(CircularBuffer));

	/* 6. init the circular buffer */
	cb_init(&cb_http_catch, cb_catch_size);
	cb_init(&cb_tcp_catch, cb_catch_size);
	cb_init(&cb_http_intercept, cb_catch_size);
	cb_init(&cb_tcp_intercept, cb_catch_size);

	cb_init(&cb_tcp_audit, cb_audit_size);


	/* 7. init the glib thread pool */
	/*
	g_thread_supported() is deprecated and should not be used in
	newly-written code.
	g_thread_init() has been deprecated since version 2.32 and should not
	be used in newly-written code. This function is no longer necessary. The
	GLib threading system is automatically initialized at the start of your
	program.
	*/

	if (!g_thread_supported())
		g_thread_init(NULL);


	/* create user info hash table */
	//ys  用户哈希表初始化   ys//
	user_hase_table_create();
	///ys 从数据库读取用户信息放入哈希表中
	// 应该主要是IP+GID
	/* get user info into hash table from mysql */
	sql_get_user_to_hash(GET_user_gid_list, &mysql, user_hash_list);



	/* create feature hash table */
	/* get feature into hash table from mysql */
	//ys feature哈希表初始化 ys//
	feature_hase_table_create();
	///ys 从数据库读取协议特征字放入哈希表中
	sql_get_feature_to_pcre(GET_feature_list, &mysql, port_feature_hash_list);



	/* init http hash table */
	///ys  http哈希表初始化  ys///
	http_hase_table_create();
	///ys 从数据库读HTTP白名单放入哈希表中
	sql_get_website_to_hash(GET_URL_white_list, &mysql, white_list, http_hash_white_list);
	///ys 从数据库读HTTP黑名单放入哈希表中
	sql_get_website_to_hash(GET_URL_person_list, &mysql, person_list, http_hash_person_list);
	///ys 从数据库读HTTP组黑名单放入哈希表中
	sql_get_website_to_hash(GET_URL_group_list, &mysql, group_list, http_hash_group_list);
	///ys 从数据库读HTTP全局黑名单放入哈希表中
	sql_get_website_to_hash(GET_URL_whole_list, &mysql, whole_list, http_hash_whole_list);

	///ys 从数据库读HTTP ALL 白名单放入哈希表中
	sql_get_website_to_hash(GET_URL_ALL_w_list, &mysql, ALL_w_list, http_hash_ALL_w_list);
	///ys 从数据库读HTTP 个人大类（某人不能上搜索那一类网站），黑名单放入哈希表中
	sql_get_website_to_hash(GET_URL_USER_ALL_b_list, &mysql, ALL_b_list, http_hash_ALL_b_list);
	///ys 从数据库读HTTP 组大类（某组不能上搜索那一类网站），黑名单放入哈希表中
	sql_get_website_to_hash(GET_URL_GROUP_ALL_b_list, &mysql, ALL_b_list, http_hash_ALL_b_list);




	/* mutex and cond for http */
	mutex_cb_http_catch = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex_cb_http_catch, NULL);
	mutex_cb_http_intercept = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex_cb_http_intercept, NULL);

	not_full_http_catch = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
	pthread_cond_init(not_full_http_catch, NULL);
	not_empty_http_catch = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
	pthread_cond_init(not_empty_http_catch, NULL);
	not_full_http_intercept = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
	pthread_cond_init(not_full_http_intercept, NULL);
	not_empty_http_intercept = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
	pthread_cond_init(not_empty_http_intercept, NULL);
	
	/* create thread to catch http */
	result = pthread_create(&http_pcap_thread, NULL, (void *) http_pcap, NULL);
	if (result != 0) {
		fprintf(stderr, "create http thread failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* create http process thread pool */
	http_process_pool = g_thread_pool_new((void *) http_process, NULL, threads_num, FALSE, &gerror);
	if (http_process_pool == NULL) {
		fprintf(stderr, "create http process pool failed\n");
		exit(EXIT_FAILURE);
	}






	/* tcp ... */

	//hash for feature
	tcp_app_tactics_hase_table_create();  //feature哈希表初始化
	tcp_port_hase_table_create();  //port哈希表初始化
	sql_get_tcp_port_to_hash(GET_port_list, &mysql, tcp_port_list);

	//ys hash for tcp ys//
	tcp_hase_table_create();  //tcp哈希表初始化
	//各种类型的tcp哈希表插入数据赋值
	sql_get_server_ip_to_hash(GET_IP_person_list, &mysql, tcp_hash_person_list, person_list);
	sql_get_server_ip_to_hash(GET_IP_white_list, &mysql, tcp_hash_white_list, white_list);
	sql_get_server_ip_to_hash(GET_IP_group_list, &mysql, tcp_hash_group_list, group_list);
	sql_get_server_ip_to_hash(GET_IP_whole_list, &mysql, tcp_hash_whole_list, whole_list);


	/* mutex */
	mutex_cb_tcp_catch = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex_cb_tcp_catch, NULL);
	mutex_cb_tcp_intercept = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex_cb_tcp_intercept, NULL);

	not_full_tcp_catch = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
	pthread_cond_init(not_full_tcp_catch, NULL);
	not_empty_tcp_catch = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
	pthread_cond_init(not_empty_tcp_catch, NULL);
	not_full_tcp_intercept = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
	pthread_cond_init(not_full_tcp_intercept, NULL);
	not_empty_tcp_intercept = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
	pthread_cond_init(not_empty_tcp_intercept, NULL);

	/* create tcp thread */
	result = pthread_create(&tcp_pcap_thread, NULL, (void *) tcp_pcap, NULL);
	if (result != 0) {
		fprintf(stderr, "create tcp thread failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* create tcp process thread pool */
	tcp_process_pool = g_thread_pool_new((void *) tcp_process, NULL, threads_num, FALSE, &gerror);
	if (tcp_process_pool == NULL) {
		fprintf(stderr, "create tcp process pool failed\n");
		exit(EXIT_FAILURE);
	}

	/* udp ... */




	/* tcp audit */
	mutex_cb_tcp_audit = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex_cb_tcp_audit, NULL);

	not_full_tcp_audit = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
	pthread_cond_init(not_full_tcp_audit, NULL);
	not_empty_tcp_audit = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
	pthread_cond_init(not_empty_tcp_audit, NULL);


	tcp_stream_list = g_hash_table_new_full(g_str_hash, g_direct_hash, free_data, free_data);


	result = pthread_create(&rebuild_packet_thread, NULL, (void *) get_syn_and_build_stream, NULL);
	if (result != 0) {
		fprintf(stderr, "create rebuild packet thread failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}


	/* socket ... */

	/* arp ... */

	/* tcp rst ... */


	/* wait for thread */
	pthread_join(http_pcap_thread, NULL);
	pthread_join(tcp_pcap_thread, NULL);
	pthread_join(rebuild_packet_thread, NULL);

	cb_free(&cb_http_catch);
	cb_free(&cb_tcp_catch);
	cb_free(&cb_http_intercept);
	cb_free(&cb_tcp_intercept);
	cb_free(&cb_tcp_audit);

	pthread_mutex_destroy(mutex_cb_http_catch);
	pthread_mutex_destroy(mutex_cb_http_intercept);
	pthread_mutex_destroy(mutex_cb_tcp_catch);
	pthread_mutex_destroy(mutex_cb_tcp_intercept);
	pthread_mutex_destroy(mutex_cb_tcp_audit);

	pthread_cond_destroy(not_full_http_catch);
	pthread_cond_destroy(not_empty_http_catch);
	pthread_cond_destroy(not_full_tcp_catch);
	pthread_cond_destroy(not_empty_tcp_catch);
	pthread_cond_destroy(not_full_http_intercept);
	pthread_cond_destroy(not_empty_http_intercept);
	pthread_cond_destroy(not_full_tcp_intercept);
	pthread_cond_destroy(not_empty_tcp_intercept);
	pthread_cond_destroy(not_full_tcp_audit);
	pthread_cond_destroy(not_empty_tcp_audit);


	return 0;
}

void
get_local_config_file(void)
{
	/*  */
	return;
}














