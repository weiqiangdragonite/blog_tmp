#include <stdio.h>
#include <signal.h>  
#include "phruncall.h"
#include "phupdate.h"
#include "log.h"
#include "phkey.h"

PHGlobal global;
PH_parameter parameter;
static void my_handleSIG (int sig)
{
	if (sig == SIGINT)
	{
		printf ("signal = SIGINT\n");
		phddns_stop(&global);
		exit(0);
	}
	if (sig == SIGTERM)
	{
		printf ("signal = SIGTERM\n");
		phddns_stop(&global);
	}
	signal (sig, my_handleSIG);
}
//状态更新回调
static void myOnStatusChanged(PHGlobal* global, int status, long data)
{
	printf("myOnStatusChanged %s", convert_status_code(status));
	if (status == okKeepAliveRecved)
	{
		printf(", IP: %d", data);
	}
	if (status == okDomainsRegistered)
	{
		printf(", UserType: %d", data);
	}
	printf("\n");
}
//域名注册回调
static void myOnDomainRegistered(PHGlobal* global, char *domain)
{
	printf("myOnDomainRegistered %s\n", domain);
}
//用户信息XML数据回调
static void myOnUserInfo(PHGlobal* global, char *userInfo, int len)
{
	printf("myOnUserInfo %s\n", userInfo);
}
//域名信息XML数据回调
static void myOnAccountDomainInfo(PHGlobal* global, char *domainInfo, int len)
{
	printf("myOnAccountDomainInfo %s\n", domainInfo);
}

int main(int argc, char *argv[])
{
	void (*ohandler) (int);
	WORD VersionRequested;	
	WSADATA  WsaData;
	int error;

	VersionRequested = MAKEWORD(2, 0);
	WSAStartup(VersionRequested, &WsaData);

	ohandler = signal (SIGINT, my_handleSIG);
	if (ohandler != SIG_DFL) {
		printf ("previous signal handler for SIGINT is not a default handler\n");
		signal (SIGINT, ohandler);
	}

	init_global(&global);
	
	global.cbOnStatusChanged = myOnStatusChanged;
	global.cbOnDomainRegistered = myOnDomainRegistered;
	global.cbOnUserInfo = myOnUserInfo;
	global.cbOnAccountDomainInfo = myOnAccountDomainInfo;
	set_default_callback(&global);
	strcpy(global.szHost,"phddns60.oray.net");
	strcpy(global.szUserID,"youname");
	strcpy(global.szUserPWD,"youpassword");
	global.clientinfo = PH_EMBED_CLIENT_INFO;
	global.challengekey = PH_EMBED_CLIENT_KEY;
	
	for (;;)
	{
		int next = phddns_step(&global);
		sleep(next);
	}
	phddns_stop(&global);
	return 0;
}

