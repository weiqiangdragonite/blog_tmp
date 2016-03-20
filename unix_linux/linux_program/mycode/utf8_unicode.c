/*
 * UTF-8无BOM格式  <--> Unicode
 */

/*

   |  Unicode符号范围      |  UTF-8编码方式  
 n |  (十六进制)           | (二进制)  
---+-----------------------+------------------------------------------------------  
 1 | 0000 0000 - 0000 007F |                                              0xxxxxxx  
 2 | 0000 0080 - 0000 07FF |                                     110xxxxx 10xxxxxx  
 3 | 0000 0800 - 0000 FFFF |                            1110xxxx 10xxxxxx 10xxxxxx  
 4 | 0001 0000 - 0010 FFFF |                   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx  
 5 | 0020 0000 - 03FF FFFF |          111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx  
 6 | 0400 0000 - 7FFF FFFF | 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx  

前面几个1就代表后面几个字节是属于一起的。


Unicode也是一种字符编码方法, 不过它是由国际组织设计, 可以容纳全世界所有语言文
字的编码方案. Unicode的学名是"Universal Multiple-Octet Coded Character Set",
简称为UCS. UCS可以看作是"Unicode Character Set"的缩写.

Unicode（统一码、万国码、单一码）是一种在计算机上使用的字符编码。
也就是每个字符占用2个字节


UTF-8最大的一个特点, 就是它是一种变长的编码方式. 它可以使用1~6个字节表示一个符
号, 根据不同的符号而变化字节长度.

UTF-8的编码规则很简单, 只有两条:

1) 对于单字节的符号, 字节的第一位设为0, 后面7位为这个符号的unicode码. 因此对于
   英语字母, UTF-8编码和ASCII码是相同的.

2) 对于n字节的符号(n>1), 第一个字节的前n位都设为1, 第n+1位设为0, 后面字节的前
   两位一律设为10. 剩下的没有提及的二进制位, 全部为这个符号的unicode码.

*/




/*
 * 用两个char存一个unicode，采用小端模式，
 * 低位存unicode的低位，高位存unicode的高位
 * unicode -> [low_1][high_1][low_2][high_2]
 */
int
utf8_to_unicode(const char *utf8, char *unicode)
{
	int i, num;
	const char *str = utf8;
	unsigned char ch;

	i = 0;
	while (str) {
		ch = (unsigned char) *str;
		if (ch == '\0')
			break;

		/* calculate utf8 has num bytes */
		num = cal_utf8_num_bytes(ch);
		//printf("ch = %02X, num = %d\n", ch, num);


		switch (num) {
		case 0:
			unicode[i] = utf8[0];
			break;
		case 2:
			break;
		case 3:
			unicode[i + 1] = ((utf8[0] & 0x0F) << 4) | ((utf8[1] >> 2) & 0x0F);
			unicode[i] = ((utf8[1] & 0x03) << 6) + (utf8[2] & 0x3F);
			break;
		default:
			break;
		}

		if (num)
			utf8 += num;
		else
			++utf8;
		i += 2;
	}

	return i / 2;
}



unicode_to_utf8()
{

}







/*
 * convert one byte to binary: 0xFF -> 1111 1111
 */
void
convert_to_binary(unsigned char byte, char binary[])
{
	char i;

	i = 7;
	while (1) {
		binary[7-i] = ((byte & (1 << i)) >> i) + '0';
		--i;
		if (i < 0)
			break;
	}
}


/*
 * 计算有多少个字节
 */
int
cal_utf8_num_bytes(unsigned char byte)
{
	int num = 0;
	int val = 0x80;
	while (byte & val) {
		++num;
		val >>= 1;
	}

	return num;
}



/*
 * 我用于gsm模块的函数
 */
void
get_unicode_str(char *s, char *msg, ssize_t size)
{
	// +8615361256066
	//char *s = "hello！东方正昕。";

	char buf[1024];
	//char msg[1024];
	int len;
	int i, j;

	memset(buf, 0, sizeof(buf));

	// 转字符串转换为Unicode代码
	len = utf8_to_unicode(s, buf);
	len *= 2;
	//printf("len = %d\n", (int) len);
	//for (i = 0; i < len; ++i)
	//	printf("%02X\n", buf[i]);

	memset(msg, 0, size);
	// 将msg 长度除2，保留两位16 进制数
	snprintf(msg, size, "%02X", len);

	// 将unicode码转为字符串
	for (i = 0, j = 0; i < len && j < size; i += 2, j += 4) {
		unsigned char high = buf[i + 1];
		unsigned char low = buf[i];
		unsigned char tmp;

		//printf("%02X %02X\n", high, low);

		tmp = (high & 0xF0) >> 4;
		msg[j + 2] = tmp > 9 ? tmp - 10 + 'A' : tmp + '0';

		tmp = high & 0x0F;
		msg[j + 3] = tmp > 9 ? tmp - 10 + 'A' : tmp + '0';

		tmp = (low & 0xF0) >> 4;
		msg[j + 4] = tmp > 9 ? tmp - 10 + 'A' : tmp + '0';

		tmp = low & 0x0F;
		msg[j + 5] = tmp > 9 ? tmp - 10 + 'A' : tmp + '0';

		//printf("%c %c %c %c\n", msg[j+2], msg[j+3], msg[j+4], msg[j+5]);
	}
}



