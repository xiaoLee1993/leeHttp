// leeHttp.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include  <thread>
#include <Winsock2.h>
#include <cstdio>
#include <ws2tcpip.h>
#include <pthread.h>
#pragma comment(lib,"ws2_32.lib")


int startup(u_short *);

int _tmain(int argc, _TCHAR* argv[])
{
	int server_sock = -1;  //服务器返回
	u_short port = 4000;   //端口
	int client_sock = -1;  //客户端返回
	struct sockaddr_in client_name;
	socklen_t client_name_len = sizeof(client_name);
	pthread_t newthread;

	server_sock = startup(&port);
	printf("httpd running on port %d\n", port);

	while (1)
	{
		client_sock = accept(server_sock,
			(struct sockaddr *)&client_name,
			&client_name_len);
		if (client_sock == -1)
			error_die("accept");
		/* accept_request(&client_sock); */
		if (pthread_create(&newthread, NULL, (void *)accept_request, (void *)(intptr_t)client_sock) != 0)
			perror("pthread_create");
	}

	closesocket(server_sock);

	return(0);
}

void error_die(const char *sc)
{
	perror(sc);
	exit(1);
}

int startup(u_short *port)
{
	int httpd = 0;
	int on = 1;
	struct sockaddr_in name;

	httpd = socket(PF_INET, SOCK_STREAM, 0);
	if (httpd == -1)
		error_die("socket");
	memset(&name, 0, sizeof(name));
	name.sin_family = AF_INET;
	name.sin_port = htons(*port);
	name.sin_addr.s_addr = htonl(INADDR_ANY);
	if ((setsockopt(httpd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0)
	{
		error_die("setsockopt failed");
	}
	if (bind(httpd, (struct sockaddr *)&name, sizeof(name)) < 0)
		error_die("bind");
	if (*port == 0)  /* if dynamically allocating a port */
	{
		socklen_t namelen = sizeof(name);
		if (getsockname(httpd, (struct sockaddr *)&name, &namelen) == -1)
			error_die("getsockname");
		*port = ntohs(name.sin_port);
	}
	if (listen(httpd, 5) < 0)
		error_die("listen");
	return(httpd);
}
