/*
 * 协议 EAP 和 EAPOL
 *
 * Type:
 * EAPOL 是 logoff 和 start , 其他都是 EAP(即 EAP Packet)
 *
 * 
 * 1. client --> send start packet to server
 *  2. client <-- server send request identity to client
 * 3. client --> send response identity to server
 *  4. client <-- server send request MD5-Challenge to client
 * 5. client --> send response MD5-Challenge to server
 *  6. client <-- server send success packet to client
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h> 

#include <openssl/md5.h> // 需要加上 -lcrypto



#define USER_NAME_LEN_MAX	24

// define pkt_type
#define PKT_TYPE_EAP				0x00
#define PKT_TYPE_START				0x01
#define PKT_TYPE_LOGOFF				0x02

// packet_type tell what type of packet 
#define PACKET_UNKNOW	-1
#define PACKET_START	0
#define PACKET_LOGOFF	1
#define PACKET_SUCCESS	3
#define PACKET_FAIL		4
#define PACKET_REQUEST_ID	5
#define PACKET_REQUEST_CH	6
#define PACKET_RESPONSE_ID	7
#define PACKET_RESPONSE_CH	8

//define body length
#define BODY_LENGTH_START			0x0000
#define BODY_LENGTH_LOGOFF			0x0000
/*  BODY_LENGTH_IDENTIFY_BASE + strlen(username) */
#define BODY_LENGTH_IDENTIFY_BASE	0x0005
#define BODY_LENGTH_IDENTIFY_MAXSIZE	128		
/*  BODY_LENGTH_IDENTIFY_BASE + strlen(username) */
#define BODY_LENGTH_CHALLENGE_BASE	0x0016

// BODY CODE
#define BODY_CODE_REQUEST 		0x01
#define BODY_CODE_RESPONSE		0x02
#define BODY_CODE_SUCCESS		0x03
#define BODY_CODE_FAIL			0x04

//BODY TYPE
#define BODY_TYPE_IDENTIFY		0x01
#define BODY_TYPE_CHALLENGE		0x04

#define PACKET_CHALLENGE_VELUE_SIZE			16
#define USER_NAME_LEN_MAX	24

//practical packet length
#define PACKET_LENGTH(pkt)	(18 + ((int)(pkt)->bodylength[0] << 8) + (pkt)->bodylength[1])



// define packet struct
typedef struct _PACKET
{
	__uint8_t	dstmac[6];
	__uint8_t	srcmac[6];

	__uint8_t	eth_type[2];	// 0x888e: Type: 802.1x authentication
	__uint8_t	version;	// 0x01: Version: v1(protocal version)
	__uint8_t	pkt_type;	// start(0x01), logoff(0x02), EAP packet(0x00)
	__uint8_t	bodylength[2];	// packet body length
	union	// packet body (EPA packet body)
	{
		// test 
		struct
		{
			__uint8_t	code;
			__uint8_t	id;	 
			__uint8_t	bodylength[2]; // EAP packet body length
			__uint8_t	type;
		}__attribute__((__packed__)) test;

		// request identify
		struct
		{
			__uint8_t	code;	//code: request (0x01)
			__uint8_t	id;	//id:1
			__uint8_t	bodylength[2];
			__uint8_t	type;	//Type: identity[rfc3748] (1)
		}__attribute__((__packed__)) request_identify;
	
		// response identify
		struct
		{
			__uint8_t	code;	//code:response(0x02)
			__uint8_t	id;	//id:1
			__uint8_t	bodylength[2];
			__uint8_t	type;	//Type: identity[rfc3748] (1)
			__uint8_t	identity_value[BODY_LENGTH_IDENTIFY_MAXSIZE];//
			__uint8_t	username[USER_NAME_LEN_MAX];// not use
		}__attribute__((__packed__)) identify;

		// Request, MD5-Challenge [RFC3748]
		struct
		{
			__uint8_t	code;//code: request (0x01)
			__uint8_t	id;//id:2
			__uint8_t	bodylength[2];
			__uint8_t	type;//0x04, Type: MD5-Challenge [RFC3748] (4)
			__uint8_t	value_size;//0x10,Value-Size: 16
			__uint8_t	value[PACKET_CHALLENGE_VELUE_SIZE];//Value: F703914F6C0D6B6AB79F00A586585CB0
		}__attribute__((__packed__)) request_challenge;

		// Response, MD5-Challenge [RFC3748]
		struct
		{
			__uint8_t  	code;//0x02, Code: Response (0x02)
			__uint8_t	id;//id:2
			__uint8_t	bodylength[2];
			__uint8_t	type;//Type: MD5-Challenge [RFC3748] (4)
			__uint8_t	value_size;//0x10,Value-Size: 16
			__uint8_t	value[PACKET_CHALLENGE_VELUE_SIZE];//Value: F703914F6C0D6B6AB79F00A586585CB0
			__uint8_t	username[USER_NAME_LEN_MAX];//
		}__attribute__((__packed__)) challenge;

		// success
		struct
		{
			__uint8_t	code;	//Code: success (0x03)
			__uint8_t	id;	//id:0
			__uint8_t	bodylength[2];	//length 4
		}__attribute__((__packed__)) success;

		// fail
		struct
		{
			__uint8_t	code;	//Code: failed (0x04)
			__uint8_t	id;	//id:0
			__uint8_t	bodylength[2];	//length 4
		}__attribute__((__packed__)) fail;
	}__attribute__((__packed__)) body;
} PACKET;


struct eapol_data {
	char ifname[12];	/* net card name, such as "eth0" */
	int sockfd;
	struct sockaddr_ll sll;
	char local_mac[6];
	char dest_mac[6];	/* standard 802.1x mac */
	char user[48];
	char passwd[48];
};

const char H3C_VERSION[16]="CH V7.10-0313"; // 华为客户端版本号  CH V7.10-0313
const char H3C_KEY[64]    ="HuaWei3COM1X";  // H3C的固定密钥
const char mac_8021x_standard[6] = {0x01, 0x80, 0xC2, 0x00, 0x00, 0x03};
static struct eapol_data eapdata;




// 函数: XOR(data[], datalen, key[], keylen)
//
// 使用密钥key[]对数据data[]进行异或加密
//（注：该函数也可反向用于解密）
void XOR(uint8_t data[], unsigned dlen, const char key[], unsigned klen)
{
	unsigned int	i,j;

	// 先按正序处理一遍
	for (i=0; i<dlen; i++)
		data[i] ^= key[i%klen];
	// 再按倒序处理第二遍
	for (i=dlen-1,j=0;  j<dlen;  i--,j++)
		data[i] ^= key[j%klen];
}


void FillClientVersionArea(uint8_t area[20])
{
	uint32_t random;
	char	 RandomKey[8+1];

	random = (uint32_t) time(NULL);    // 注：可以选任意32位整数
	sprintf(RandomKey, "%08x", random);// 生成RandomKey[]字符串

	// 第一轮异或运算，以RandomKey为密钥加密16字节
	memcpy(area, H3C_VERSION, sizeof(H3C_VERSION));
	XOR(area, 16, RandomKey, strlen(RandomKey));

	// 此16字节加上4字节的random，组成总计20字节
	random = htonl(random); // 需调整为网络字节序
	memcpy(area+16, &random, 4);

	// 第二轮异或运算，以H3C_KEY为密钥加密前面生成的20字节
	XOR(area, 20, H3C_KEY, strlen(H3C_KEY));
}


void FillBase64Area(char area[])
{
	uint8_t version[20];
	const char Tbl[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz"
			"0123456789+/"; // 标准的Base64字符映射表
	uint8_t	c1,c2,c3;
	int i, j;

	// 首先生成20字节加密过的H3C版本号信息
	FillClientVersionArea(version);

	// 然后按照Base64编码法将前面生成的20字节数据转换为28字节ASCII字符
	i = 0;
	j = 0;
	while (j < 24)
	{
		c1 = version[i++];
		c2 = version[i++];
		c3 = version[i++];
		area[j++] = Tbl[ (c1&0xfc)>>2                               ];
		area[j++] = Tbl[((c1&0x03)<<4)|((c2&0xf0)>>4)               ];
		area[j++] = Tbl[               ((c2&0x0f)<<2)|((c3&0xc0)>>6)];
		area[j++] = Tbl[                                c3&0x3f     ];
	}
	c1 = version[i++];
	c2 = version[i++];
	area[24] = Tbl[ (c1&0xfc)>>2 ];
	area[25] = Tbl[((c1&0x03)<<4)|((c2&0xf0)>>4)];
	area[26] = Tbl[               ((c2&0x0f)<<2)];
	area[27] = '=';
}


////////////////////////////////////////////////////////////////////////////////

//printhex("packet hexstream: ", pkt, PACKET_LENGTH(pkt), "\n");
static void printhex(char *start, void *dst, int len, char *end)
{
	unsigned char *p = NULL;
	int i;

	if (start != NULL)
		printf("%s", start);

	for (p = (unsigned char *)dst, i = 0; i < len; i++, p++)
	{
		if(*p >= 0x10)
			printf("0x%2x ", *p);
		else
			printf("0x0%1x ", *p);
	}

	if(end != NULL)
		printf("%s", end);
}

static void printhex2(char *start, void *dst, int len, char *end)
{
	unsigned char *p = NULL;
	int i, j;
	unsigned char hex[16];

	printf("len = [%d]\n", len);

	memset(hex, 0, sizeof(hex));
	if (start != NULL)
		printf("%s", start);

	
	printf("%08X:  ", 0);
	for (p = (unsigned char *)dst, i = 0, j = 0; i < len; i++, p++)
	{
		hex[j++] = *p;
		if (j == 16)
		{
			for (j = 0; j <= 15; ++j)
				printf("%02x%s", hex[j], j == 7 ? "  " : " ");
			printf("    ");
			for (j = 0; j <= 15; ++j)
				printf("%c%s", isprint(hex[j]) ? hex[j] : '.',
					j == 7 ? " " : "");
			printf("\n%08X:  ", i+1);
			j = 0;
		}
	}
	if (j != 0)
	{
		for (i = 0; i < j; ++i)
			printf("%02x%s", hex[i], i == 7 ? "  " : " ");
		for (; i <= 15; ++i)
			printf("%02c%s", ' ', i == 7 ? "  " : " ");
		printf("    ");
		for (i = 0; i < j; ++i)
			printf("%c%s", isprint(hex[i]) ? hex[i] : '.', 
				i == 7 ? " " : "");
		for (; i <= 15; ++i)
			printf("%c%s", ' ', i == 7 ? " " : "");
	}
	printf("\n");
	if(end != NULL)
		printf("%s", end);
}

int packet_length(PACKET *pkt)
{
	if(pkt == NULL)
		return 0;

	return pkt->bodylength[1] + (pkt->bodylength[0] << 8) + 18; // 大端小端的问题，所以是 第1位 + 第0位左移8位
}

/**
 * packet_type - tell what type of packet 
 * @pkt:	PACKET pointer
 * @return:	return packet type macro, see packet.h
 */
int packet_type(PACKET	*pkt)
{
	switch(pkt->pkt_type)
	{
		case PKT_TYPE_EAP:
			printf("###this is packet EAP\n");
			break;
		case PKT_TYPE_START:
			printf("###this is packet_start\n");
			return PACKET_START;
			break;
		case PKT_TYPE_LOGOFF:
			printf("###this is packet logoff\n");
			return PACKET_LOGOFF;
			break;
		default:
			return PACKET_UNKNOW;
	}

	//EAP packet
	switch(pkt->body.test.code)
	{
		case BODY_CODE_REQUEST:
			if (pkt->body.test.type == BODY_TYPE_IDENTIFY) {	
				printf("####### request IDENTIFY\n");
				return PACKET_REQUEST_ID;
			} else if (pkt->body.test.type == BODY_TYPE_CHALLENGE) {	
				printf("####### request CHALLENGE\n");
				return PACKET_REQUEST_CH;
			}
			break;

		case BODY_CODE_RESPONSE:
			if (pkt->body.test.type == BODY_TYPE_IDENTIFY) {	
				printf("####### response IDENTIFY\n");
				return PACKET_RESPONSE_ID;
			} else if (pkt->body.test.type == BODY_TYPE_CHALLENGE) {	
				printf("####### response CHALLENGE\n");
				return PACKET_RESPONSE_CH;
			}
			break;

		case BODY_CODE_SUCCESS:
			printf("####this is a SUCCESS packet\n");	
			return PACKET_SUCCESS;
			break;

		case BODY_CODE_FAIL:
			printf("####this is a FAIL packet\n");
			return PACKET_FAIL;
			break;
		default:
			return PACKET_UNKNOW;
	}
}

/**
 * packet_get_start - make a start packet
 * @pkt:	where to store packet dataoff
 * @srcmac: local mac
 * @return: return pkt
 */
PACKET *packet_get_start(PACKET *pkt, char *srcmac)
{
	if (pkt == NULL || srcmac == NULL)
	{
		fprintf(stderr, "[%s][%d] param is null\n", __FILE__, __LINE__);
		return NULL;
	}

	memset(pkt, 0, sizeof(PACKET));
	memcpy(pkt->srcmac, srcmac, 6);
	memcpy(pkt->dstmac, mac_8021x_standard, 6);
	pkt->eth_type[0] = 0x88;// 0x888e，这里要用大端，所以第0位是0x88
	pkt->eth_type[1] = 0x8e;
	pkt->version = 0x01;

	pkt->pkt_type = PKT_TYPE_START;//Type: Start (1)
	pkt->bodylength[0] = 0x00;
	pkt->bodylength[1] = 0x00;

	return pkt;
}


int GetIpFromDevice(uint32_t ip, const char DeviceName[])
{
	int fd;
	struct ifreq ifr;

	if (strlen(DeviceName) > IFNAMSIZ)
	{
		fprintf(stderr, "[%s][%d] error: strlen(DeviceName) > IFNAMSIZ\n",
			__FILE__, __LINE__);
		return -1;
	}

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0)
	{
		fprintf(stderr, "[%s][%d] socker() error: %s\n",
			__FILE__, __LINE__, strerror(errno));
		return -1;
	}

	strncpy(ifr.ifr_name, DeviceName, IFNAMSIZ);
	ifr.ifr_addr.sa_family = AF_INET;

	if (ioctl(fd, SIOCGIFADDR, &ifr) == 0)
	{
		struct sockaddr_in *p = (void*) &(ifr.ifr_addr);
		memcpy(&ip, &(p->sin_addr), 4);
	}
	else
	{
		// 查询不到IP时默认填零处理
		//memset(ip, 0x00, 4);

		// 查询ip，如果有，填当前ip，如果没有，填0，(这里不能填0，要填1个ip)
		char *ip_str = "128.128.166.7"; // 随便填一个
		struct sockaddr_in addr4;
		if (inet_pton(AF_INET, ip_str, &addr4.sin_addr) != 1)
		{
			fprintf(stderr, "[%s][%d] inet_pton() error: \n", 
				__FILE__, __LINE__, strerror(errno));
			return -1;
		}
		//ip = (uint32_t) addr4.sin_addr.s_addr;
		memcpy(&ip, &(addr4.sin_addr), 4);
	}

	close(fd);
	return 0;
}


/**
 * packet_get_identify - make a identify packet
 * @pkt:	where to store packet dataoff
 * @return: return pkt, or null
 */
PACKET *packet_get_identify(PACKET *pkt, PACKET *src_pkt, struct eapol_data *eapdatap)
{
	int i, len, usernamelen;
	uint32_t ip;
	uint8_t	response[128];
	uint16_t eaplen;
	char *srcmac = eapdatap->local_mac;
	char *username = eapdatap->user;

	if(pkt == NULL || src_pkt == NULL || srcmac == NULL || username == NULL)
	{
		fprintf(stderr, "[%s][%d] param is null\n", __FILE__, __LINE__);
		return NULL;
	}

	memset(response, 0, sizeof(response));
	len = strlen(username);
	if(len > USER_NAME_LEN_MAX)
		len = USER_NAME_LEN_MAX;

	memset(pkt, 0, sizeof(PACKET));
	memcpy(pkt->srcmac, srcmac, 6);
	memcpy(pkt->dstmac, mac_8021x_standard, 6);

	pkt->eth_type[0] = 0x88;
	pkt->eth_type[1] = 0x8e;
	pkt->version = 0x01;

	pkt->pkt_type = PKT_TYPE_EAP;//Type: eap package(0)

	pkt->body.identify.code = BODY_CODE_RESPONSE;
	pkt->body.identify.id = 0x01;//0x01 for stage 1
	pkt->body.identify.type = BODY_TYPE_IDENTIFY;


	/* body type data */
	i = 0;
	response[i++] = 0x15;//
	response[i++] = 0x04;

	// 上传IP地址
	if (GetIpFromDevice(ip, eapdatap->ifname) < 0)
	{
		fprintf(stderr, "[%s][%d] GetIpFromDevice error\n", __FILE__, __LINE__);
		return NULL;
	}
	htonl(ip);
	memcpy(response+i, &ip, 4);//

	i += 4;//
	response[i++] = 0x06;// 携带版本号
	response[i++] = 0x07;//
	FillBase64Area((char*)response+i);//28个字节
	i += 28;
	response[i++] = ' ';// 两个空格符
	response[i++] = ' ';//
	usernamelen = strlen(username);//末尾添加用户名
	memcpy(response+i, username, usernamelen);
	i += usernamelen;

	memset(pkt->body.identify.identity_value, 0, BODY_LENGTH_IDENTIFY_MAXSIZE);
	memcpy(pkt->body.identify.identity_value, (char *) response, i);

	// body length = code + id + type + bodylength + identity_value
	eaplen = htons(i+5);
	memcpy(pkt->bodylength, &eaplen, sizeof(eaplen));
	memcpy(pkt->body.identify.bodylength, &eaplen, sizeof(eaplen));

	// not use
	memcpy(pkt->body.identify.username, username, usernamelen);

	return pkt;
}


PACKET *packet_get_logoff(PACKET *pkt, char *srcmac)
{
	if (pkt == NULL || srcmac == NULL)
	{
		fprintf(stderr, "[%s][%d] param is null\n", __FILE__, __LINE__);
		return NULL;
	}

	memset(pkt, 0, sizeof(PACKET));

	memcpy(pkt->dstmac, mac_8021x_standard, 6);
	memcpy(pkt->srcmac, srcmac, 6);
	pkt->eth_type[0] = 0x88;
	pkt->eth_type[1] = 0x8e;
	pkt->version = 0x01;
	pkt->pkt_type = PKT_TYPE_LOGOFF;//Type: Logoff (2)
	pkt->bodylength[0] = 0x00;
	pkt->bodylength[1] = 0x00;

	return pkt;
}


void FillMD5Area(uint8_t digest[], uint8_t id, const char passwd[], const uint8_t srcMD5[])
{
	uint8_t	msgbuf[128]; // msgbuf = ‘id‘ + ‘passwd’ + ‘srcMD5’
	size_t	msglen;
	size_t	passlen;

	passlen = strlen(passwd);
	msglen = 1 + passlen + 16;
	//assert(sizeof(msgbuf) >= msglen);

	msgbuf[0] = id;
	memcpy(msgbuf+1,	 passwd, passlen);
	memcpy(msgbuf+1+passlen, srcMD5, 16);

	(void) MD5(msgbuf, msglen, digest);
}



/**
 * packet_get_challenge - make a challenge packet
 * @pkt:	where to store packet dataoff
 * @src_pkt:request Challenge packet from server
 * @return: return pkt, or null
 */
//PACKET *packet_get_challenge(PACKET *pkt, PACKET *src_pkt, char *srcmac, char *dstmac, char *username, char *password)
PACKET *packet_get_challenge(PACKET *pkt, PACKET *src_pkt, struct eapol_data *eapdatap)
{
	char *srcmac = eapdatap->local_mac;
	char *username = eapdatap->user;
	char *password = eapdatap->passwd;
	int userlen;

	if (pkt == NULL || src_pkt == NULL || username == NULL || password == NULL)
	{
		fprintf(stderr, "[%s][%d] param is null\n", __FILE__, __LINE__);
		return NULL;
	}

	userlen = strlen(username);
	memset(pkt, 0, sizeof(PACKET));
	memcpy(pkt->dstmac, mac_8021x_standard, 6);
	memcpy(pkt->srcmac, srcmac, 6);
	pkt->eth_type[0] = 0x88;
	pkt->eth_type[1] = 0x8e;
	pkt->version = 0x01;

	pkt->pkt_type = PKT_TYPE_EAP;//Type: eap package(0)

	//body length
	pkt->bodylength[0] = (userlen + BODY_LENGTH_CHALLENGE_BASE) >> 8;
	pkt->bodylength[1] = userlen + BODY_LENGTH_CHALLENGE_BASE;

	pkt->body.challenge.bodylength[0] = pkt->bodylength[0];
	pkt->body.challenge.bodylength[1] = pkt->bodylength[1];


	pkt->body.challenge.code = BODY_CODE_RESPONSE;
	pkt->body.challenge.id = src_pkt->body.request_challenge.id;//hold same id
	pkt->body.challenge.type = BODY_TYPE_CHALLENGE;
	pkt->body.challenge.value_size = PACKET_CHALLENGE_VELUE_SIZE;
	



	// compute hush value[16]--------------------------------------
	/* XXX:
	 *	Hei, challenge value is a hush value: 
	 * 	data[0]: src_pkt->body.request_challenge.id; a id from server
	 *  data[1]~data[passwordlength + 1]:	your account password	
	 *  data[passwordlength + 2] ~ data[passwordlength + 16]: value[16]
	 *  from server
	 *
	 * Every one loves PIC:
	 *  data:
	 * 		 --------------------------
	 * 		|id | password | value[16] |
	 * 		 --------------------------
	 * compute this data hush value, that is challenge value!
	 */
	unsigned char data[64] = "";
	unsigned char *hushvalue = NULL;
	int datalen = 0;
	int passwdlen = 0;

	data[0] = src_pkt->body.request_challenge.id;
	datalen = 1;

	passwdlen = strlen(password);
	memcpy(data + datalen, password, passwdlen);
	datalen += passwdlen;


	memcpy(data + datalen, src_pkt->body.request_challenge.value, src_pkt->body.request_challenge.value_size);
	datalen += src_pkt->body.request_challenge.value_size;

//	hushvalue = ComputeHash(data, datalen);
//	//compute hush value[16]done!!---------------------------------  这个函数不行
//	memcpy(pkt->body.challenge.value, hushvalue, src_pkt->body.request_challenge.value_size);
//	strcpy(pkt->body.challenge.username, username);


	uint8_t	response[128];
	FillMD5Area(response, src_pkt->body.request_challenge.id,
		password, src_pkt->body.request_challenge.value);
	memcpy(pkt->body.challenge.value, response, src_pkt->body.request_challenge.value_size);
	strcpy(pkt->body.challenge.username, username);


	return pkt;
}

/**
 * packet_print - print packet info
 */
void packet_print(PACKET *pkt)
{
	unsigned char *p = NULL;
	int i;
	int is_request_not_response = -1;

	printf("\n\n");
	if(pkt == NULL){
		printf("empty packet\n");
		return;
	}

	printhex("dst mac:\t", pkt->dstmac, 6, "\n");
	printhex("src mac:\t", pkt->srcmac, 6, "\n");
	printhex("eth_type:\t  ", pkt->eth_type, 2, "\n");
	printhex("version:\t  ", &pkt->version, 1, "\n");
	printhex("pkt_type:\t  ", &pkt->pkt_type, 1, "\n");
	printhex("bodylength:\t  ", pkt->bodylength, 2, " ");
	printf("(%d)\n", PACKET_LENGTH(pkt) - 18);

	
	switch(packet_type(pkt)){
		case PACKET_START:
			printf("###this is packet PACKET_START\n");
			break;

		case PACKET_LOGOFF:
			printf("###this is packet PACKET_LOGOFF\n");
			break;

		case PACKET_SUCCESS:
			printf("###this is packet PACKET_SUCCESS\n");
			break;

		case PACKET_FAIL:
			printf("###this is packet PACKET_FAIL\n");
			break;

		case PACKET_REQUEST_ID:
			printf("###this is packet PACKET_REQUEST_ID\n");
			break;

		case PACKET_REQUEST_CH:
			printf("###this is packet PACKET_REQUEST_CH\n");
			printhex("###value_size:\t  ", &pkt->body.request_challenge.value_size, 1, "\n");
			printhex("###value:\t  ", pkt->body.request_challenge.value, pkt->body.request_challenge.value_size, "\n");
			break;

		case PACKET_RESPONSE_ID:
			printf("###this is packet PACKET_RESPONSE_ID\n");
			printf("###username %24s\n", pkt->body.identify.username);
			break;

		case PACKET_RESPONSE_CH:
			printf("###this is packet PACKET_RESPONSE_CH\n");
			printf("####### response challenge\n");
			printhex("###value_size:\t  ", &pkt->body.challenge.value_size, 1, "\n");
			printhex("###value:\t  ", pkt->body.challenge.value, pkt->body.challenge.value_size, "\n");
			printhex("###username:\t  ", pkt->body.challenge.username, USER_NAME_LEN_MAX, "\n");
			printf("###username: %24s\n", pkt->body.challenge.username);
			break;

		default:
			printf("-------unknow packet!!-------\n");
			break;
	}

	printf("############################\n");
	printhex2("packet hexstream: \n", pkt, PACKET_LENGTH(pkt), "\n");
	printf("############################\n");

	printf("\n\n");
}

////////////////////////////////////////////////////////////////////////////////

static void sig_intr_handler(int signo)
{
	printf("SIGINT, exit...\n");
	exit(0);
}


void gwnet_logoff(void)
{
	PACKET pkt_logoff;
	packet_get_logoff(&pkt_logoff, eapdata.local_mac);

	printf("logoff...\n");
	if (eapdata.sockfd < 0 || &pkt_logoff == NULL)
		return;


	if (sendto(eapdata.sockfd, &pkt_logoff, packet_length(&pkt_logoff), 0,
		(struct sockaddr *) &eapdata.sll, sizeof eapdata.sll) < 0)
	{
		fprintf(stderr, "[%s][%d] sendto() error: %s\n", 
				__FILE__, __LINE__, strerror(errno));
		//return;
	}

	close(eapdata.sockfd);
	eapdata.sockfd = -1;
}

/**
 * socket_eapol_init - init socket for eapol protocol
 * @ifname: net card name, such as "eth0"
 * @psll:   where to store struct sockaddr_ll
 * @return: return socket, -1 on error
 */
int socket_eapol_init(char *ifname, struct sockaddr_ll *psll)
{
	struct ifreq ifr;
	int sockfd;
	int sockopts, sockerr, retval;

	// check para
	if (ifname == NULL || psll == NULL)
	{
		fprintf(stderr, "[%s][%d] param is null\n", __FILE__, __LINE__);
		return -1;
	}

	memset(psll, 0, sizeof(struct sockaddr_ll));
	memset(&ifr, 0, sizeof(ifr));

	sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_PAE));
	if (sockfd < 0)
	{
		fprintf(stderr, "[%s][%d] socket() error: %s\n", 
			__FILE__, __LINE__, strerror(errno));
		return -1;
	}

	// indicate net card
	strcpy((char *) &ifr.ifr_name, ifname);
	retval = ioctl(sockfd, SIOCGIFINDEX, &ifr);
	if (retval < 0)
	{
		fprintf(stderr, "[%s][%d] ioctl() error: %s\n", 
			__FILE__, __LINE__, strerror(errno));
		close(sockfd);
		return -1;
	}

	psll->sll_family = PF_PACKET;
	psll->sll_ifindex = ifr.ifr_ifindex;
	psll->sll_protocol = htons(ETH_P_PAE);

	retval = bind(sockfd, (const struct sockaddr *) psll, sizeof(struct sockaddr_ll));
	if (retval < 0)
	{
		fprintf(stderr, "[%s][%d] bind() error: %s\n", 
			__FILE__, __LINE__, strerror(errno));
		close(sockfd);
		return -1;
	}

	// set nonblock socket
	sockopts = fcntl(sockfd, F_GETFL, 0);
	if (sockopts < 0)
	{
		fprintf(stderr, "[%s][%d] fcntl() error: %s\n", 
			__FILE__, __LINE__, strerror(errno));
		close(sockfd);
		return -1;
	}
	sockerr = fcntl(sockfd, F_SETFL, sockopts | O_NONBLOCK);
	if (sockerr < 0)
	{
		fprintf(stderr, "[%s][%d] fcntl() error: %s\n", 
			__FILE__, __LINE__, strerror(errno));
		close(sockfd);
		return -1;
	}

	return sockfd;
}


/**
 * get_hwaddr - get netdevice mac addr
 * @name: device name, e.g: eth0
 * @hwaddr: where to save mac, 6 byte hwaddr[6]
 * @return: 0 on success, -1 on failure
 */
int get_hwaddr(char *name, unsigned char *hwaddr)
{
	struct ifreq ifr;
	unsigned char memzero[6];
	int sock;

	if (name == NULL || hwaddr == NULL)
	{
		fprintf(stderr, "[%s][%d] param is null\n", __FILE__, __LINE__);
		return -1;
	}

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		fprintf(stderr, "[%s][%d] socket() error: %s\n", 
			__FILE__, __LINE__, strerror(errno));
		return -1;
	}

	// get mac addr
	memset(hwaddr, 0, 6);
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, name, 6);
	if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0)
	{
		fprintf(stderr, "[%s][%d] ioctl() error: %s\n", 
			__FILE__, __LINE__, strerror(errno));
		close(sock);
		return -1;
	}
	else
	{
		memcpy(hwaddr, ifr.ifr_hwaddr.sa_data, 6);
		printf("hwaddr: %02x:%02x:%02x:%02x:%02x:%02x\n",
			hwaddr[0], hwaddr[1], hwaddr[2], hwaddr[3], hwaddr[4], hwaddr[5]);
	}

	memset(memzero, 0, 6);
	if (memcmp(memzero, hwaddr, 6) == 0)
	{
		fprintf(stderr, "[%s][%d] no mac address\n", __FILE__, __LINE__);
		close(sock);
		return -1;
	}

	close(sock);
	return 0;
}

/**
 * eapdata_init - init eapdata(build socket, indicate net device, save account)
 * @return: 0 on success, -1 on error
 */
int eapdata_init(struct eapol_data *eapdatap, char *netdev, char *user, char *passwd)
{
	if (eapdatap == NULL || netdev == NULL || user == NULL || passwd == NULL)
	{
		fprintf(stderr, "[%s][%d] param is null\n", __FILE__, __LINE__);
		//printf("%s: null para\n", __FUNCTION__);
		return -1;
	}

	memset(eapdatap, 0, sizeof(struct eapol_data));
	strncpy(eapdatap->ifname, netdev, sizeof(eapdatap->ifname));
	strncpy(eapdatap->user, user, sizeof(eapdatap->user));
	strncpy(eapdatap->passwd, passwd, sizeof(eapdatap->passwd));

	eapdatap->sockfd = socket_eapol_init(eapdatap->ifname, &eapdatap->sll);
	if (eapdatap->sockfd < 0)
	{
		fprintf(stderr, "[%s][%d] socket_eapol_init() error\n", __FILE__, __LINE__);
		return -1;
	}

	// get mac
	if (get_hwaddr(eapdatap->ifname, eapdatap->local_mac) < 0)
	{
		fprintf(stderr, "[%s][%d] get %s mac address error\n",
			__FILE__, __LINE__, eapdatap->ifname);
		close(eapdatap->sockfd);
		return -1;
	}

	return 0;
}




/**
 * pselect_recvfrom - recvfrom with pselect
 * @fd:	sockfd
 * @buf: where to store rec data
 * @len: buf length
 * @tv_sec: timeout seconds
 * @return: returns the number of bytes recvfrom, -1 on timeout or error.
 */
int pselect_recvfrom(int fd, void *buf, int len, int tv_sec)
{
	fd_set read_set;
	int ret;
	struct timespec timeout;
	sigset_t sigset_full;

	if(buf == NULL)
	{
		fprintf(stderr, "[%s][%d] param is null\n", __FILE__, __LINE__);
		return -1;
	}
	memset(buf, 0, len);

	signal(SIGINT, sig_intr_handler);
	signal(SIGTERM, sig_intr_handler);
	sigfillset(&sigset_full); // init sigset to full, including all signals
	sigprocmask(SIG_BLOCK, &sigset_full, NULL); // set signal to block

	sigfillset(&sigset_full);
	sigdelset(&sigset_full, SIGINT); // delete from set
	sigdelset(&sigset_full, SIGTERM);
	FD_ZERO(&read_set);
	FD_SET(fd, &read_set);
	timeout.tv_sec = tv_sec;
	timeout.tv_nsec = 0;

	//wait with all signals(except SIGINT) blocked.
	ret = pselect(fd + 1, &read_set, NULL, NULL, &timeout, &sigset_full);
	if (ret <= 0) {
		if (ret == 0)
			fprintf(stderr, "[%s][%d] pselect() timeout\n", __FILE__, __LINE__);
		else
			fprintf(stderr, "[%s][%d] pselect() error: %s\n", 
				__FILE__, __LINE__, strerror(errno));
		return -1;
	}

	//rec data
	if (FD_ISSET(fd, &read_set))
	{
		ret = recvfrom(fd, buf, len, 0, NULL, NULL);
		if (ret < 0)
		{
			fprintf(stderr, "[%s][%d] recvfrom() error: %s\n", 
				__FILE__, __LINE__, strerror(errno));
			return -1;
		}
		else
		{
			return ret;
		}
	}

	return -1;
}

int authentication(struct eapol_data *eapdatap)
{
	int i, reclen;
	int cnt = 0;
	PACKET pkt_recv;
	PACKET pkt_start;
	PACKET pkt_id;
	PACKET pkt_passwd;

	// packet init
	packet_get_start(&pkt_start, eapdatap->local_mac);

	// find server
	for (i = 0; i < 10; ++i)
	{
		printf("find server...\n");
		if (sendto(eapdatap->sockfd, &pkt_start, packet_length(&pkt_start), 0,
			(struct sockaddr *) &eapdatap->sll, sizeof(eapdatap->sll)) < 0)
		{
			fprintf(stderr, "[%s][%d] sendto() error: %s\n", 
				__FILE__, __LINE__, strerror(errno));
			return -1;
		}

		memset(&pkt_recv, 0, sizeof(pkt_recv));
		reclen = pselect_recvfrom(eapdatap->sockfd, &pkt_recv, sizeof(pkt_recv), 5);
		if (reclen <= 0)
		{
			fprintf(stderr, "[%s][%d] pselect_recvfrom() error, reclen = [%d]\n", 
				__FILE__, __LINE__, reclen);
			continue;
		}

		// server found
		if (packet_type(&pkt_recv) == PACKET_REQUEST_ID)
			break;
	}
	if (i == 10)
	{
		// no server
		fprintf(stderr, "[%s][%d] can't find server\n", __FILE__, __LINE__);
		return -1;
	}

	printf("server found\n\n\n");


	printf("send start packet...\n");

	printf("-----> SEND PACKET DATA:\n");
	packet_print(&pkt_start);
	if (sendto(eapdatap->sockfd, &pkt_start, packet_length(&pkt_start), 0,
			(struct sockaddr *) &eapdatap->sll, sizeof(eapdatap->sll)) < 0)
	{
		fprintf(stderr, "[%s][%d] sendto() error: %s\n", 
			__FILE__, __LINE__, strerror(errno));
		return -1;
	}

	atexit(gwnet_logoff); // try: on_exit()


	while (1)
	{
		memset(&pkt_recv, 0, sizeof(pkt_recv));
		reclen = pselect_recvfrom(eapdatap->sockfd, &pkt_recv, sizeof(pkt_recv), 30);
		if (reclen <= 0)
		{
			fprintf(stderr, "[%s][%d] pselect_recvfrom() error, reclen = [%d]\n", 
				__FILE__, __LINE__, reclen);
			return -1;
		}

		printf("\n\n<----- RECV PACKET DATA:\n");
		packet_print(&pkt_recv);

		switch (packet_type(&pkt_recv))
		{
		case PACKET_REQUEST_ID:
			printf("send identity packet... (%d)\r\n", cnt++);
			//init identify packet
			packet_get_identify(&pkt_id, &pkt_recv, eapdatap);

			printf("-----> SEND PACKET DATA:\n");
			packet_print(&pkt_id);
			if (sendto(eapdatap->sockfd, &pkt_id, packet_length(&pkt_id), 0,
				(struct sockaddr *) &eapdatap->sll, sizeof(eapdatap->sll)) < 0)
			{
				fprintf(stderr, "[%s][%d] sendto() error: %s\n", 
					__FILE__, __LINE__, strerror(errno));
				return -1;
			}
			break;

		case PACKET_REQUEST_CH:
			//memcpy(pkt_recv.body.request_challenge.value, testval, 16);
			//packet_print(&pkt_recv);

			printf("send challenge packet...\n");
			packet_get_challenge(&pkt_passwd, &pkt_recv, eapdatap);

			printf("-----> SEND PACKET DATA:\n");
			packet_print(&pkt_passwd);
			if (sendto(eapdata.sockfd, &pkt_passwd, packet_length(&pkt_passwd), 0,
				(struct sockaddr *) &eapdatap->sll, sizeof(eapdatap->sll)) < 0)
			{
				fprintf(stderr, "[%s][%d] sendto() error: %s\n", 
					__FILE__, __LINE__, strerror(errno));
				return -1;
			}
			break;

		case PACKET_SUCCESS:
			printf("login success!!\n");
			return 0;
			break;

		case PACKET_FAIL:
			fprintf(stderr, "[%s][%d] login failed, may be used by other or wrong password\n",
				__FILE__, __LINE__);
			return -1;
			break;

		default:
			fprintf(stderr, "[%s][%d] unknow packet\n", __FILE__, __LINE__);
			break;
		}
	}

	return 0;
}



/*
 * 有以下情况
 * 获取ip后，退出客户端，但不logoff
 * 获取ip后，一直运行，直到手动退出，并logoff
 */

int main(int argc, char *argv[])
{
	char *user = "heweiqiang@gkoa";
	char *passwd = "password123";
	char *netdev = "enp0s8";
	//char *netdev = "enp0s3";
	//struct eapol_data eapdata;

	// init socket
	if (eapdata_init(&eapdata, netdev, user, passwd) < 0)
	{
		fprintf(stderr, "[%s][%d] eapdata_init() error\n", __FILE__, __LINE__);
		return -1;
	}

	//
	authentication(&eapdata);

	// 在 logoff 里
	close(eapdata.sockfd);
	eapdata.sockfd = -1;

	return 0;
}

