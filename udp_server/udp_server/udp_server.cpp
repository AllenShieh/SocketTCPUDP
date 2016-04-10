#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Windows.h>
#include <WinSock.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_PORT 8888 
#define MAX_MSG_SIZE 1024 
char filename[MAX_MSG_SIZE] = "";
char temp[MAX_MSG_SIZE] = "";
char ack[20] = "";

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
			//n = fread(temp, 1, MAX_MSG_SIZE, fp);
			//bool ack = false;
			//while (!ack){
			//	sendto(sockfd, temp, n, 0, (struct sockaddr*)&addr, addrlen);
			//	recvfrom(sockfd, temp, MAX_MSG_SIZE, 0, (struct sockaddr*)&addr, &addrlen);
			//	temp[3] = '\0';
			//	if (!strcmp(temp, "ACK")){
			//		printf("ACK\n");
			//		ack = true;
			//	}
			//}
			int status = 1;
			n = fread(temp, 1, MAX_MSG_SIZE - 1, fp);
			temp[n] = '1';
			n = n + 1;
			sendto(sockfd, temp, n, 0, (struct sockaddr*)&addr, addrlen);
			while (!feof(fp)){
				struct timeval tv;
				fd_set readfds;
				FD_ZERO(&readfds);
				FD_SET(sockfd, &readfds);
				tv.tv_sec = 3;
				tv.tv_usec = 10;
				select(sockfd + 1, &readfds, NULL, NULL, &tv);
				if (FD_ISSET(sockfd, &readfds)){
					recvfrom(sockfd, ack, MAX_MSG_SIZE, 0, (struct sockaddr*)&addr, &addrlen);
					temp[4] = '\0';
					if (!strcmp(ack, "ACK1")){
						//printf("ACK1\n");
						if (status == 1){
							n = fread(temp, 1, MAX_MSG_SIZE - 1, fp);
							temp[n] = '2';
							n = n + 1;
							sendto(sockfd, temp, n, 0, (struct sockaddr*)&addr, addrlen);
							status = 2;
						}
						else if (status == 2){
							continue;
						}
					}
					else if (!strcmp(ack, "ACK2")){
						//printf("ACK2\n");
						if (status == 2){
							n = fread(temp, 1, MAX_MSG_SIZE - 1, fp);
							temp[n] = '1';
							n = n + 1;
							sendto(sockfd, temp, n, 0, (struct sockaddr*)&addr, addrlen);
							status = 1;
						}
						else if (status == 1){
							continue;
						}
					}
				}
				else{
					printf("time out\n");
					sendto(sockfd, temp, n, 0, (struct sockaddr*)&addr, addrlen);
				}
			}
			
		}
		recvfrom(sockfd, ack, MAX_MSG_SIZE, 0, (struct sockaddr*)&addr, &addrlen);
		sendto(sockfd, "EOF", 3, 0, (struct sockaddr*)&addr, addrlen);

		printf("file sent\n");
		fclose(fp);
	}

	WSACleanup();
}