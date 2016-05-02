/*
 * File: mysql.c
 * -------------
 * Test for mysql function
 *
 * Compile:
 * gcc mysql.c -I/usr/include/mysql -L/usr/lib64/mysql -lmysqlclient -o mysql
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mysql.h>


/* MySQL database connect info */
#define MYSQL_HOST	"192.168.1.170"
#define MYSQL_USER	"dragonite"
#define	MYSQL_PASSWD	"dragonite"
#define MYSQL_DB	"PEMS"


/* Function prototypes */
void mysql_error_exit(const char *msg);
int mysql_connect();
void mysql_version();


/* Global MYSQL structure */
MYSQL mysql;


int
main(int argc, char *argv[])
{
	mysql_connect();
	mysql_version();



	/* query data start*/
	
	char *query = "SELECT * FROM `server_table`";
	int result;
	MYSQL_RES *res_ptr;
	MYSQL_ROW row;
	int num_row, num_col;
	int i, j;

	result = mysql_query(&mysql, query);
	if (result)
		mysql_error_exit("mysql_query() failed");

	res_ptr = mysql_store_result(&mysql);
	if (res_ptr == NULL)
		mysql_error_exit("mysql_store_result() failed");


	num_row = mysql_num_rows(res_ptr);
	num_col = mysql_num_fields(res_ptr);

	printf("num_row = %d, num_column = %d\n", num_row, num_col);

	for(i = 0; i < num_row; ++i) {
		row = mysql_fetch_row(res_ptr);
		for(j = 0; j < num_col; ++j)
			printf("%s  ", row[j]);
		printf("\n");
	}

	mysql_free_result(res_ptr);

	/* Closes a previously opened connection */
	mysql_close(&mysql);

	return 0;
}

/*
 *
 */
void
mysql_error_exit(const char *msg)
{
	fprintf(stderr, "%s: %s\n", msg, mysql_error(&mysql));
	exit(EXIT_FAILURE);
}

/*
 *
 */
int
mysql_connect(void)
{
	MYSQL *connect_ptr;

	/* Prepares and initializes a MYSQL structure to be used
	   withmysql_real_connect() */
	connect_ptr = mysql_init(&mysql);
	if (connect_ptr == NULL)
		mysql_error_exit("mysql_init() failed");

	/* We can set extra connect options and affect behavior for a
	   connection using mysql_options() */

	/* Establish a connection to database server */
	connect_ptr = mysql_real_connect(&mysql, MYSQL_HOST, MYSQL_USER,
					 MYSQL_PASSWD, MYSQL_DB, 0, NULL, 0);
	if (connect_ptr == NULL)
		mysql_error_exit("mysql_real_connect() failed");

	printf("Connect to mysql success!\n");

	/* Sets the default character set for the current connection */
	mysql_set_character_set(&mysql, "utf8");

	return 0;
}

/*
 *
 */
void
mysql_version(void)
{
	/* Get client library info */
	printf("MySQL client info: %s!\n", mysql_get_client_info());
	/* Get server info */
	printf("MySQL server info: %s!\n", mysql_get_server_info(&mysql));
}

void
mysql_insert()
{
	/* insert data start */
	/*
	query = "INSERT INTO access_info (Card_Num, Card_Custom_Num, "
		"Card_Name, Card_Group) VALUES ('110', '110', '何伟强', '技术部')";
	result = mysql_query(&mysql, query);
	if (result)
		mysql_error_exit("mysql_query() failed");
	*/
	/* insert data end */
}
