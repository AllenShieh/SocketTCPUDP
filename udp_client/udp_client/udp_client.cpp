#include <stdio.h>
#include <Windows.h>
#include <WinSock.h>

#pragma comment(lib, "ws2_32.lib")

/*           客户端程序            */

#define MAX_BUF_SIZE     1024 

char IP[20] = "59.78.22.231";
char PORT[20] = "8888";

void udpc_requ(int sockfd, const struct sockaddr_in *addr, int len)
{
	char buffer[MAX_BUF_SIZE];
	int n;
	while (1)
	{
		/*    从键盘读入,写到服务端  */
		fgets(buffer, MAX_BUF_SIZE, stdin);
		sendto(sockfd, buffer, strlen(buffer), 0, (sockaddr *)addr, len);
		memset(buffer, 0, MAX_BUF_SIZE);

		/*    从网络上读,写到屏幕上   */
		n = recvfrom(sockfd, buffer, MAX_BUF_SIZE, 0, NULL, NULL);
		buffer[n] = 0;
		fputs(buffer, stdout);
	}
}

int main()
{
	printf("UDP client\n");
	int port = 8888;
	struct sockaddr_in       addr;
	WSADATA wsa;
	SOCKET sockfd;

	//初始化套接字
	WSAStartup(MAKEWORD(2, 2), &wsa);

	if ((port = atoi(PORT))<0)
	{
		printf("Wrong server_port\n");
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (sockfd<0)
	{
		printf("Socket   Error:%s\n");
		exit(1);
	}

	/*       填充服务端的资料     */

	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	if (inet_addr(IP)<0)
	{
		printf("Ip error:%s\n");
		exit(1);
	}

	addr.sin_addr.S_un.S_addr = inet_addr(IP);
	udpc_requ(sockfd, &addr, sizeof(struct sockaddr_in));
	//close(sockfd); 
	WSACleanup();

	return 0;
}