#ifndef WIN32_LEAN_AND_MEAN  
#define WIN32_LEAN_AND_MEAN  
#endif  
#define _CRT_SECURE_NO_DEPRECATE
#include <Windows.h>  
#include <winsock2.h>  
#include <ws2tcpip.h>  
#include <iphlpapi.h>
#include <cstdio>  
#include <iostream>
#include <conio.h>
#include <stdlib.h>
using namespace std;
#pragma comment (lib, "Ws2_32.lib")  
#pragma comment (lib, "Mswsock.lib")  
#pragma comment (lib, "AdvApi32.lib")  
#define DEFAULT_PORT "27015"  
#define DEFAULT_PORT_REC "27013"
#define DEFAULT_BUFLEN 512  
SOCKET ConnectSocket = INVALID_SOCKET;
//SOCKET ClientSocket = INVALID_SOCKET;

//enter the ip you are going to chat with
char IP[DEFAULT_BUFLEN] = "59.78.22.231";
char filename[DEFAULT_BUFLEN] = "";
char temp[DEFAULT_BUFLEN] = "";

//sender thread
DWORD WINAPI sender(LPVOID lpParameter){

	while (1){
		char temp[DEFAULT_BUFLEN];
		scanf("%s", temp);
		send(ConnectSocket, temp, 256, 0);

		// receive file status
		FILE *fp = fopen(temp, "wb");
		if (fp == NULL)
		{
			printf("create file %s failed\n", temp);
			return -1;
		}
		int num = 0;
		char tt[DEFAULT_BUFLEN];
		while (1)
		{
			num = recv(ConnectSocket, tt, DEFAULT_BUFLEN, 0);
			cout << "length:" << num << endl;
			if (num == 0) break;
			//if (!strcmp(tt, "EOF")) break;
			fwrite(tt, 1, num, fp);
		}
		printf("fils received\n");
		fclose(fp);
	}

	return 0;
}


int main()
{
	printf("TCP client\n");

	//whatever, i barely understand these
	int iResult = 0;
	WSADATA wsaData;
	struct addrinfo *result = NULL, *ptr = NULL, hints;
	int recvbuflen = DEFAULT_BUFLEN;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		printf("client WSAStartup failed: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	//this is important
	iResult = getaddrinfo(IP, DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		printf("client get addrinfor fail: %d\n", iResult);
		WSACleanup(); // terminate use of WS2_32.dll  
		return 1;
	}

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET)
		{
			printf("error %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;// if fail try next address returned by getaddrinfo  
			continue;
		}
		break;
	}

	freeaddrinfo(result);
	if (ConnectSocket == INVALID_SOCKET)
	{
		printf("client unable to connect to server\n");
		WSACleanup();
		return 1;
	}

	printf("input file name:\n");

	//create threads
	//HANDLE handle1, handle2;
	//handle1 = CreateThread(NULL, 0, sender, NULL, 0, NULL);
	//handle2 = CreateThread(NULL, 0, receiver, NULL, 0, NULL);


	while (1){
		scanf("%s", filename);
		send(ConnectSocket, filename, 256, 0);

		// receive file status
		FILE *fp = fopen(filename, "wb");
		if (fp == NULL)
		{
			printf("create file %s failed\n", filename);
			return -1;
		}
		int num = 0;
		while (1)
		{
			num = recv(ConnectSocket, temp, DEFAULT_BUFLEN, 0);
			//cout << "length:" << num;
			//cout << "content:" << temp << endl;
			if (num <= 0){
				cout << "nothing received\n";
				exit(0);
			}
			if (num >= 3 && temp[num - 1] == 'F' && temp[num - 2] == 'O' && temp[num - 3] == 'E'){
				num = num - 3;
				temp[num] = '\0';
				fwrite(temp, 1, num, fp);
				break;
			}
			fwrite(temp, 1, num, fp);
		}
		printf("fils received\n");
		fclose(fp);
	}

	closesocket(ConnectSocket);
	WSACleanup();

	return 0;
}
