#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Windows.h>
#include <WinSock.h>

#pragma comment(lib, "ws2_32.lib")

#define MAX_BUF_SIZE     1024 
char temp[MAX_BUF_SIZE] = "";
char filename[MAX_BUF_SIZE] = "";

char IP[20] = "59.78.22.231";
char PORT[20] = "8888";

int main()
{
	printf("UDP client with RDT\nInput file name:\n");
	int port = 8888;
	struct sockaddr_in addr;
	WSADATA wsa;
	SOCKET sockfd;

	WSAStartup(MAKEWORD(2, 2), &wsa);

	if ((port = atoi(PORT))<0)
	{
		printf("Wrong server_port\n");
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (sockfd<0)
	{
		printf("Socket Error:%s\n");
		exit(1);
	}

	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	if (inet_addr(IP)<0)
	{
		printf("Ip error:%s\n");
		exit(1);
	}

	addr.sin_addr.S_un.S_addr = inet_addr(IP);


	int n;
	while (1)
	{
		scanf("%s", filename);
		sendto(sockfd, filename, strlen(filename), 0, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));

		FILE * fp = fopen(filename, "wb");
		
		int status = 1;
		int total = 0;
		int wrong = 0;
		while (1){
			//n = recvfrom(sockfd, temp, MAX_BUF_SIZE, 0, NULL, NULL);
			//sendto(sockfd, "ACK", 3, 0, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
			//if (n <= 0){
			//	printf("nothing received\n");
			//	system("pause");
			//	exit(0);
			//}
			//if (n >= 3 && temp[n - 1] == 'F' && temp[n - 2] == 'O' && temp[n - 3] == 'E'){
			//	n = n - 3;
			//	temp[n] = '\0';
			//	fwrite(temp, 1, n, fp);
			//	break;
			//}
			//fwrite(temp, 1, n, fp);
			if (status == 1){
				n = recvfrom(sockfd, temp, MAX_BUF_SIZE, 0, NULL, NULL);
				if (temp[n - 1] == '1'){
					char c = 0;
					for (int k = 0; k < n - 2; k++){
						c += temp[k];
					}
					if (c == temp[n - 2]){
						//printf("right\n");
					}
					else{
						//printf("wrong\n");
						wrong++;
					}
					total++;
					sendto(sockfd, "ACK1", 4, 0, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
					fwrite(temp, 1, n-2, fp);
					status = 2;
				}
				else if (temp[n - 1] == '2'){
					continue;
				}
				else if (n >= 3 && temp[n - 1] == 'F' && temp[n - 2] == 'O' && temp[n - 3] == 'E'){
					break;
				}
			}
			else if (status == 2){
				n = recvfrom(sockfd, temp, MAX_BUF_SIZE, 0, NULL, NULL);
				if (temp[n - 1] == '2'){
					char c = 0;
					for (int k = 0; k < n - 2; k++){
						c += temp[k];
					}
					if (c == temp[n - 2]){
						//printf("right\n");
					}
					else{
						//printf("wrong\n");
						wrong++;
					}
					total++;
					sendto(sockfd, "ACK2", 4, 0, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
					fwrite(temp, 1, n - 2, fp);
					status = 1;
				}
				else if (temp[n - 1] == '1'){
					continue;
				}
				else if (n >= 3 && temp[n - 1] == 'F' && temp[n - 2] == 'O' && temp[n - 3] == 'E'){
					break;
				}
			}
		}
		printf("file received\ntotal packets: %d\nwrong packets: %d\n", total, wrong);
		fclose(fp);
	}

	//close(sockfd); 
	WSACleanup();

	return 0;
}