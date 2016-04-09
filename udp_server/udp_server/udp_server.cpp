#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Windows.h>
#include <WinSock.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_PORT 8888 
#define MAX_MSG_SIZE 1024 
char filename[MAX_MSG_SIZE] = "";
char temp[MAX_MSG_SIZE] = "";

int main()
{
	printf("UDP server\n");
	WSADATA wsa;
	SOCKET sockfd;
	struct sockaddr_in addr;

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

	int addrlen, n;

	addrlen = sizeof(sockaddr_in);

	while (1)
	{
		n = recvfrom(sockfd, filename, MAX_MSG_SIZE, 0, (struct sockaddr*)&addr, &addrlen);
		filename[n] = 0;
		//printf("filename: %s\n", filename);

		FILE * fp = fopen(filename, "rb");

		while (!feof(fp)){
			n = fread(temp, 1, MAX_MSG_SIZE, fp);
			sendto(sockfd, temp, n, 0, (struct sockaddr*)&addr, addrlen);
		}
		sendto(sockfd, "EOF", 3, 0, (struct sockaddr*)&addr, addrlen);
		printf("file sent\n");
		fclose(fp);
	}

	WSACleanup();
}