#!/bin/sh
gcc -c udp_pcap.c
gcc -c public_function.c
gcc -c ringbuff.c
gcc -c sock_client.c
gcc -c sock_server.c
gcc -c sock_server_cmd.c
gcc -c sql_function.c
gcc -c tcp_block.c
gcc -c tcp_hash_function.c
gcc -c tcp_pcap.c
gcc -c tcp_process.c
gcc -c user_function.c
gcc -c IM_process.c
gcc -c arp.c
gcc -c cache.c
gcc -c feature.c
gcc -c http_hash_function.c
gcc -c http_pcap.c
gcc -c http_process.c
gcc -c user_function.c
gcc -c replace_name.c

ar -r libys0.a udp_pcap.o
ar -r libys1.a public_function.o
ar -r libys2.a ringbuff.o
ar -r libys3.a sock_client.o
ar -r libys4.a sock_server.o
ar -r libys5.a sock_server_cmd.o
ar -r libys6.a sql_function.o
ar -r libys7.a tcp_block.o
ar -r libys8.a tcp_hash_function.o
ar -r libys9.a tcp_pcap.o
ar -r libys10.a tcp_process.o
ar -r libys11.a user_function.o
ar -r libys12.a IM_process.o
ar -r libys13.a arp.o
ar -r libys14.a cache.o
ar -r libys15.a feature.o
ar -r libys16.a http_hash_function.o
ar -r libys17.a http_pcap.o
ar -r libys18.a http_process.o
ar -r libys19.a user_function.o
ar -r libys20.a replace_name.o


gcc  main.c  -O2  -lys0 -lys1 -lys2 -lys4 -lys5 -lys6 -lys7 -lys8 -lys9 -lys20 -lys11 -lys12 -lys13 -lys14 -lys15 -lys16 -lys17 -lys18 -lys19  -lys10  -lys11 -lpcap -lpthread -lnet -lmysqlclient -lglib-2.0 -lgthread-2.0 -lcrypto -L. -static -o yspro

cp *.a /usr/lib/
