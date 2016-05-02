#!/bin/sh
gcc -o libys0.so -O2 -fPIC -shared app_im.c -I/usr/local/include  -I/usr/local/include/glib-2.0/  -I/usr/local/include/mysql -L/usr/local/lib  -L/usr/local/lib/mysql
gcc -o libys1.so -O2 -fPIC -shared audit_insert_db.c -I/usr/local/include  -I/usr/local/include/glib-2.0/  -I/usr/local/include/mysql -L/usr/local/lib  -L/usr/local/lib/mysql
gcc -o libys2.so -O2 -fPIC -shared email.c -I/usr/local/include  -I/usr/local/include/glib-2.0/  -I/usr/local/include/mysql -L/usr/local/lib  -L/usr/local/lib/mysql
gcc -o libys3.so -O2 -fPIC -shared get_file_name.c -I/usr/local/include  -I/usr/local/include/glib-2.0/  -I/usr/local/include/mysql -L/usr/local/lib  -L/usr/local/lib/mysql
gcc -o libys4.so -O2 -fPIC -shared hash_foreach.c -I/usr/local/include  -I/usr/local/include/glib-2.0/  -I/usr/local/include/mysql -L/usr/local/lib  -L/usr/local/lib/mysql
gcc -o libys5.so -O2 -fPIC -shared http.c -I/usr/local/include  -I/usr/local/include/glib-2.0/  -I/usr/local/include/mysql -L/usr/local/lib  -L/usr/local/lib/mysql
gcc -o libys6.so -O2 -fPIC -shared http_bss.c -I/usr/local/include  -I/usr/local/include/glib-2.0/  -I/usr/local/include/mysql -L/usr/local/lib  -L/usr/local/lib/mysql
gcc -o libys7.so -O2 -fPIC -shared http_instream.c -I/usr/local/include  -I/usr/local/include/glib-2.0/  -I/usr/local/include/mysql -L/usr/local/lib  -L/usr/local/lib/mysql
gcc -o libys8.so -O2 -fPIC -shared http_search.c -I/usr/local/include  -I/usr/local/include/glib-2.0/  -I/usr/local/include/mysql -L/usr/local/lib  -L/usr/local/lib/mysql
gcc -o libys9.so -O2 -fPIC -shared iconv.c -I/usr/local/include  -I/usr/local/include/glib-2.0/  -I/usr/local/include/mysql -L/usr/local/lib  -L/usr/local/lib/mysql
gcc -o libys10.so -O2 -fPIC -shared public_function.c -I/usr/local/include -I/usr/local/include/libnet11/ -L/usr/local/lib/libnet11/ -I/usr/local/include/glib-2.0/  -I/usr/local/include/mysql -L/usr/local/lib  -L/usr/local/lib/mysql
gcc -o libys11.so -O2 -fPIC -shared read_file.c -I/usr/local/include  -I/usr/local/include/glib-2.0/  -I/usr/local/include/mysql -L/usr/local/lib  -L/usr/local/lib/mysql
gcc -o libys12.so -O2 -fPIC -shared rebuild_udp.c -I/usr/local/include  -I/usr/local/include/glib-2.0/  -I/usr/local/include/mysql -L/usr/local/lib  -L/usr/local/lib/mysql
gcc -o libys13.so -O2 -fPIC -shared replace_name.c -I/usr/local/include  -I/usr/local/include/glib-2.0/  -I/usr/local/include/mysql -L/usr/local/lib  -L/usr/local/lib/mysql
gcc -o libys14.so -O2 -fPIC -shared ringbuff.c -I/usr/local/include  -I/usr/local/include/glib-2.0/  -I/usr/local/include/mysql -L/usr/local/lib  -L/usr/local/lib/mysql
gcc -o libys15.so -O2 -fPIC -shared slist_hash.c -I/usr/local/include  -I/usr/local/include/glib-2.0/  -I/usr/local/include/mysql -L/usr/local/lib  -L/usr/local/lib/mysql
gcc -o libys16.so -O2 -fPIC -shared sock_client.c -I/usr/local/include  -I/usr/local/include/glib-2.0/  -I/usr/local/include/mysql -L/usr/local/lib  -L/usr/local/lib/mysql
gcc -o libys17.so -O2 -fPIC -shared sql_function.c -I/usr/local/include  -I/usr/local/include/glib-2.0/  -I/usr/local/include/mysql -L/usr/local/lib  -L/usr/local/lib/mysql
gcc -o libys18.so -O2 -fPIC -shared tcp_buff_Mag.c -I/usr/local/include  -I/usr/local/include/glib-2.0/  -I/usr/local/include/mysql -L/usr/local/lib  -L/usr/local/lib/mysql
gcc -o libys19.so -O2 -fPIC -shared uri.c -I/usr/local/include  -I/usr/local/include/glib-2.0/  -I/usr/local/include/mysql -L/usr/local/lib  -L/usr/local/lib/mysql
gcc -o libys20.so -O2 -fPIC -shared arp.c -I. -I/usr/local/include  -I/usr/local/include/libnet11/  -I/usr/local/include/mysql -L/usr/local/lib/libnet11  -L/usr/local/lib/mysql -lnet

gcc -o  yspro -O2  -L. -I. -I/usr/local/include  -I/usr/local/include/glib-2.0/  -I/usr/local/include/mysql -L/usr/local/lib  -L/usr/local/lib/mysql -lpcap -lpthread -lz -lcrypto -lmysqlclient  -lglib-2.0 -lgthread-2.0  -lys0 -lys20 -lys1 -lys2 -lys3 -lys4 -lys5 -lys6 -lys7 -lys8 -lys9  -lys11 -lys12 -lys13 -lys14 -lys15 -lys16 -lys17 -lys18 -lys19  -lys10  main.c  



#gcc main.c -o yspro -L.  -lpthread -lnet  -lcrypto -lmysqlclient   -lpcap  -lglib-2.0 -lgthread-2.0  -lys0 -lys1 -lys2 -lys3 -lys4 -lys5 -lys6 -lys7 -lys8 -lys9 -lys10 -lys11 -lys12 -lys13 -lys14 -lys15 -lys16 -lys17 -lys18 -lys19  -lys20   



mv *.so /usr/lib/
mv yspro /mg/
