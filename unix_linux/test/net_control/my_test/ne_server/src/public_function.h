#ifndef PUBLIC_FUNCTION_H
#define PUBLIC_FUNCTION_H
#include <pcap.h>
#include <glib.h>
extern int public_fun_datalink_off(pcap_t *pd);
extern int public_fun_bpf_config(pcap_t *handle,char *filter_exp,struct bpf_program fp,bpf_u_int32 net);
extern void rst_tcp();
extern void rst_tcp_for_tcp();
extern char * http_get_info(char *packet,char * str_cmp,int len);
extern int public_fun_user_iterator(gpointer key, gpointer value ,gpointer user_data);
#endif
