
从罗锦洪的抓包代码出发：

main函数

1.首先获取本地文件的内容（即数据库的地址，用户名，密码等）
get_config_local_file()

2.初始化mysql

3.初始化系统配置



=======

问题：
0.
抓http的(tcp[13] != 0x19)究竟是什么意思


1.
packe_info.h定义的raw_packet只有64字节，且
memcpy(my_packet.raw_packet, packet, header->caplen);
将packet的长度复制到raw_packet里，但是通过抓http包可得header->caplen超过64字节
导致Segmentation fault

-->
我觉得要将raw_packet设成65535
现在设置的值为1518


2.
关于VLAN，没有VLAN处理，如果http的包不含VLAN信息，那就没问题，如果有VLAN，那么就需要处理


3.
http抓包时，虽然说典型的以太网帧长度是1518字节，但是如果抓包时设置MAXBYTES2CAPTURE为65535，
抓到的tcp 80的包会存在部分包的长度超过1518字节


4.
packet_tmp.packet_size=ntohs(ip_header->ip_len)-40;///YS 数据包的有效负载
为什么是减去40，而不是减去20（ip首部长为20）

--> 解决：因为 ip_len不包含ethernet层的14字节，IP+TCP首部的长度为40，减去40后，就是TCP的数据部分了


5.
打开一个网站，记录的url地址太多了，比如打开sina，出现一堆其它次要的网址，怎样才能精准的记录用户
打开的某个网站网址，而不会记录相应无关必要的网址呢


6.
http_pcap只用来捉http包，进行拦截，不做审计
tcp_pcap才会做审计，这样是不是有点重复，不是应该在http_pcap里抓包审计拦截吗

真的重复了，http里拦截的包放到cb_http_b缓冲区，叫pool_tcp_rst去做拦截
tcp里，一开始抓的包放到cb_tcp_a里，但到了黑名单那里后，却检查cb_http_b，用pool_tcp_rst去拦截


7.
抓包的规则像 "tcp and (tcp[13] != 0x4) and (tcp[13] != 0x19) and (tcp[13] != 0x14)";
究竟是什么意思






