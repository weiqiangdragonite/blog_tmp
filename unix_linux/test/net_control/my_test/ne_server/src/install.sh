#!/bin/sh
gcc -o libys0.so -O2 -fPIC -shared udp_pcap.c
gcc -o libys1.so -O2 -fPIC -shared public_function.c
gcc -o libys2.so -O2 -fPIC -shared ringbuff.c
gcc -o libys3.so -O2 -fPIC -shared sock_client.c
gcc -o libys4.so -O2 -fPIC -shared sock_server.c
gcc -o libys5.so -O2 -fPIC -shared sock_server_cmd.c
gcc -o libys6.so -O2 -fPIC -shared sql_function.c
gcc -o libys7.so -O2 -fPIC -shared tcp_block.c
gcc -o libys8.so -O2 -fPIC -shared tcp_hash_function.c
gcc -o libys9.so -O2 -fPIC -shared tcp_pcap.c
gcc -o libys10.so -O2 -fPIC -shared tcp_process.c
gcc -o libys11.so -O2 -fPIC -shared user_function.c
gcc -o libys12.so -O2 -fPIC -shared IM_process.c
gcc -o libys13.so -O2 -fPIC -shared arp.c
gcc -o libys14.so -O2 -fPIC -shared cache.c
gcc -o libys15.so -O2 -fPIC -shared feature.c
gcc -o libys16.so -O2 -fPIC -shared http_hash_function.c
gcc -o libys17.so -O2 -fPIC -shared http_pcap.c
gcc -o libys18.so -O2 -fPIC -shared http_process.c
gcc -o libys19.so -O2 -fPIC -shared user_function.c
gcc -o libys20.so -O2 -fPIC -shared replace_name.c

gcc -o  yspro -O2  -L. -L/usr/lib -lpcap -lpthread -lmysqlclient -lglib-2.0 -lgthread-2.0 -lcrypto -lys0 -lys1 -lys2 -lys3 -lys4 -lys5 -lys6 -lys7 -lys8 -lys9 -lys20 -lys11 -lys12 -lys13 -lys14 -lys15 -lys16 -lys17 -lys18 -lys19  -lys10  -lys11 main.c -lnet 

#gcc main.c -o yspro -L.  -lpthread -lnet  -lcrypto -lmysqlclient   -lpcap  -lglib-2.0 -lgthread-2.0  -lys0 -lys1 -lys2 -lys3 -lys4 -lys5 -lys6 -lys7 -lys8 -lys9 -lys10 -lys11 -lys12 -lys13 -lys14 -lys15 -lys16 -lys17 -lys18 -lys19  -lys20   



cp *.so /usr/lib/
cp yspro /mg/
