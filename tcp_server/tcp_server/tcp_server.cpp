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
char IP[DEFAULT_BUFLEN] = "";
char filename[DEFAULT_BUFLEN] = "";
char temp[DEFAULT_BUFLEN] = "";

int main()
{
	printf("TCP server\n");

	int iResult = 0;
	WSADATA wsaData;
	struct addrinfo *result = NULL, *ptr = NULL, hints;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		printf("server WSAStartup failed: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		printf("server getaddrinfo faild: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	SOCKET ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET)
	{
		printf("server failed at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		printf("server bind faild: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	freeaddrinfo(result);

	if (listen(ListenSocket, SOMAXCONN))
	{
		printf("server listen socket failed %ld\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	ConnectSocket = accept(ListenSocket, NULL, NULL);
	if (ConnectSocket == INVALID_SOCKET)
	{
		printf("server accept failed: %ld\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	closesocket(ListenSocket);

	printf("waiting for request\n");

	while (1)
	{
		int num = 0;
		num = recv(ConnectSocket, filename, DEFAULT_BUFLEN, 0);
		if (num == 0) break;
		//printf("filename: %s length:%d\n", filename, num);
		//temp[num] = '\0';

		// file transfer   
		FILE * fp = fopen(filename, "rb"); // binary mode for read  
		if (fp == NULL)
		{
			printf("open file %s failed\n", filename);
			return -1;
		}

		while (!feof(fp))
		{
			num = fread(temp, 1, DEFAULT_BUFLEN, fp);
			//printf("%s\n", temp);
			send(ConnectSocket, temp, num, 0);
		}
		send(ConnectSocket, "EOF", 3, 0);
		printf("file sent\n");
		fclose(fp);
	}

	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		printf("server shutdown failed %ld\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}
	closesocket(ConnectSocket);
	WSACleanup();

	return 0;
}
