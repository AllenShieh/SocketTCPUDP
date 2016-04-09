#include <stdio.h>
#include <Windows.h>
#include <WinSock.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_PORT      8888 
#define MAX_MSG_SIZE     1024 

void udps_respon(int sockfd)
{
	struct sockaddr_in addr;
	int      addrlen, n;
	char     msg[MAX_MSG_SIZE];

	addrlen = sizeof(sockaddr_in);

	while (1)
	{
		/* �������϶�,д����������ȥ  */
		n = recvfrom(sockfd, msg, MAX_MSG_SIZE, 0, (struct sockaddr*)&addr, &addrlen);
		msg[n] = 0;

		/* ��ʾ������Ѿ��յ�����Ϣ */
		fprintf(stdout, "Received %s", msg);
		sendto(sockfd, msg, n, 0, (struct sockaddr*)&addr, addrlen);
	}
}

int main()
{
	printf("UDP server\n");
	WSADATA wsa;
	SOCKET sockfd;
	struct sockaddr_in       addr;

	//��ʼ���׽���
	WSAStartup(MAKEWORD(2, 2), &wsa);

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (SOCKET_ERROR == sockfd)
	{
		printf("Socket Error:%s\n");
		exit(1);
	}
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(SERVER_PORT);
	if (bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in))<0)
	{
		printf("Bind Error:%s\n");
		exit(1);
	}

	udps_respon(sockfd);

	WSACleanup();
}