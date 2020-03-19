#include "stdafx.h"
#include <stdio.h>
#include <Winsock2.h>
#include <time.h>  
#include<conio.h>


#define IS_CLIENT

#define DEST_IP "47.94.20.34"

#define DEST_PORT 10111

#define FROM_WHERE_PC_PREFIX "PC_DEV_"
#define FROM_WHERE_PC "PC_DEV_OFFICE" // for client

#define FROM_WHERE_APP_PREFIX "APP_DEV_"
#define FROM_WHERE_APP "APP_DEV_OFFICE"	// for client

#pragma comment(lib, "ws2_32.lib")
#pragma comment( linker, "/subsystem:windows /entry:mainCRTStartup" )

extern int sendQueryParm(void);
extern int Bird_soc_tbox_rev_command_deal(U8 *p_receive_data, U32 receive_length);

char sendBuf[1000];
char recvBuf[1000];
char sendAppBuf[1000];
int sendAppBufLen = 0;
extern bool ackerror;
SOCKET sockConn;

char InputCommand;
bool stopsendmessage = false;
bool queryparm = false;
bool setcontrol;
bool haveConentSendtoApp = false;
//int reclen = SOCKET_ERROR;
SOCKET  ClientSocketFromPC = (SOCKET)0;

#define DEV_ID_LENGTH	20
#define SOCKET_LIST_LENGTH	100

typedef struct
{
	char dev_id[DEV_ID_LENGTH];
	SOCKET socket_id;
}socket_t;

static socket_t socket_list[SOCKET_LIST_LENGTH];


DWORD  WINAPI  AnswerThread(LPVOID  lparam)
{
	SOCKET  ClientSocket = (SOCKET)(LPVOID)lparam;
	int count = 1;
	bool isAppSocketConnent = true;
	bool isTboxSocketConnet = false;
	
	

	while (1)
	{
		SOCKET  ClientSocket = (SOCKET)(LPVOID)lparam;

		memset(sendBuf, 0, 1000);
		memset(recvBuf, 0, 1000);
		time_t nowtime;
		struct tm* ptm;
		int reclen = SOCKET_ERROR;
		isAppSocketConnent = true;
		char *dev;
		int i;

		while (1)
		{
			reclen = recv(ClientSocket, recvBuf, 1000, 0);// 接受客户端消息
			if (reclen == 0 || reclen == SOCKET_ERROR)
			{

				for (i = 0; i < SOCKET_LIST_LENGTH; i++)
				{
					if (socket_list[i].socket_id == ClientSocket)
					{
						printf("socket_list[%d].socket_id = %d,dev id: %s, removed!\r\n", i, socket_list[i].socket_id, socket_list[i].dev_id);
						memset(socket_list[i].dev_id, 0, DEV_ID_LENGTH);
						socket_list[i].socket_id = 0;
						break;
					}
				}
				return 0;
			}
			recvBuf[reclen] = 0;

			//from pc
			dev = strstr(recvBuf, FROM_WHERE_PC_PREFIX);
			if (dev)
			{
				printf("message from PC, dev ID = %s\r\n", dev);

				for (i = 0; i < SOCKET_LIST_LENGTH; i++)
				{
					if (socket_list[i].socket_id == ClientSocket || socket_list[i].socket_id == 0)
					{
						strncpy(socket_list[i].dev_id, dev + strlen(FROM_WHERE_PC_PREFIX), DEV_ID_LENGTH);
						socket_list[i].socket_id = ClientSocket;
						printf("socket_list[%d].socket_id = %d,dev id: %s, added!\r\n", i, socket_list[i].socket_id, socket_list[i].dev_id);
						break;
					}
				}
				if(i == SOCKET_LIST_LENGTH)
					printf("no list to added!!!!!\r\n");
				continue;
			}

			//from app
			dev = strstr(recvBuf, FROM_WHERE_APP_PREFIX);
			if (dev)
			{
				printf("message from APP: %s\r\n", recvBuf);

				for (i = 0; i < SOCKET_LIST_LENGTH; i++)
				{
					if (strstr(dev + strlen(FROM_WHERE_APP_PREFIX), socket_list[i].dev_id) && socket_list[i].dev_id[0] != 0)
					{
						send(socket_list[i].socket_id, recvBuf + strlen(FROM_WHERE_APP_PREFIX) + strlen(socket_list[i].dev_id), reclen - strlen(FROM_WHERE_APP_PREFIX) - strlen(socket_list[i].dev_id), 0);
						printf("cmd send to PC:%s\r\n", recvBuf + strlen(FROM_WHERE_APP_PREFIX) + strlen(socket_list[i].dev_id));
						printf("socket_list[%d].socket_id = %d,dev id: %s, sended!\r\n", i, socket_list[i].socket_id, socket_list[i].dev_id);
						break;
					}
				}
				if (i == SOCKET_LIST_LENGTH)
					printf("no list to sended!!!!!\r\n");
				continue;
			}	
			//unknown source
			printf("message from unknown: %s\r\n", recvBuf);
		}
	}
	return  0;
}


DWORD  WINAPI  GetInput(LPVOID  lparam)
{
	while(1)
	{
		InputCommand = _getch();
		_getch();
	}	
	return 0;
}

int  main(int  argc, char*  argv[])
{
	//initialize  Winsock  
	WSADATA  wsaData;
	int  iRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iRet != NO_ERROR)
		printf("Error  at  WSAStartup()\n");

	memset(socket_list, sizeof(socket_t), 0);

#ifdef IS_CLIENT

	//create  a  socket  
	SOCKET  m_socket;

	SOCKADDR_IN addrSrv;
	int ret;
	char dev[100];

	memset((char*)&addrSrv, 0, sizeof(addrSrv));
	addrSrv.sin_addr.S_un.S_addr = inet_addr(DEST_IP);
	addrSrv.sin_family = AF_INET;
	//bzero(&addrSrv, sizeof(addrSrv));
	addrSrv.sin_port = htons(DEST_PORT);

	sockaddr_in mysock;
	memset((char*)&mysock, 0, sizeof(mysock));
	mysock.sin_family = AF_INET;
	mysock.sin_port = htons(10111);//1234是端口号
	mysock.sin_addr.s_addr = inet_addr(DEST_IP);

	while (1)
	{
		m_socket = socket(AF_INET, SOCK_STREAM, 0);
		if (m_socket == INVALID_SOCKET)
		{
			printf("Error  at  socket():%ld\n", WSAGetLastError());
			WSACleanup();
			return  0;
		}

		ret = connect(m_socket, (struct sockaddr *)&addrSrv, sizeof(addrSrv));
		//ret = connect(m_socket, (struct sockaddr*)&mysock, sizeof(mysock));

		if (ret < 0)
		{
			perror("can't connect to server");
			Sleep(1000);
			closesocket(m_socket);
			continue;
		}

		printf("connect socket OK.Socket ID :%d\n", m_socket);

		if (argc == 2)
		{
			sprintf(dev, "%s%s", FROM_WHERE_PC_PREFIX, argv[1]);
			send(m_socket, dev, strlen(dev), 0);
		}
		else
			send(m_socket, FROM_WHERE_PC, strlen(FROM_WHERE_PC), 0);

		while (1)
		{
			int reclen = recv(m_socket, recvBuf, 1000, 0);// 接受客户端消息
			if (reclen == 0 || reclen == SOCKET_ERROR)
			{
				Sleep(1000);
				closesocket(m_socket);
				break;
			}
			recvBuf[reclen] = 0;
			send(m_socket, "OK\r\n", 4, 0);
			printf("%s\r\n", recvBuf);
			system(recvBuf);
		}

	}

	

#else

	//create  a  socket  
	SOCKET  m_socket;

	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == INVALID_SOCKET)
	{
		printf("Error  at  socket():%ld\n", WSAGetLastError());
		WSACleanup();
		return  0;
	}

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(DEST_PORT);

	int ret = bind(m_socket, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));// 绑定端口
	if (ret == SOCKET_ERROR)
	{
		printf("bind()  failed.\n");
		closesocket(m_socket);
		return  0;
	}
	else
		printf("bind  OK.\n");

	//listen  on  a  socket  
	if (listen(m_socket, 20) == SOCKET_ERROR)
		printf("Error  listening  on  socket.\n");
	else
		printf("listening  ok.\n");

	//accept  a  connection  
	SOCKET  AcceptSocket;
	SOCKADDR_IN addrClient;// 连接上的客户端ip地址
	int len = sizeof(SOCKADDR);

	printf("Waiting  for  a  client  to  connect...\n");
	while (1)
	{
		AcceptSocket = SOCKET_ERROR;
		while (AcceptSocket == SOCKET_ERROR)
		{
			//AcceptSocket = accept(m_socket, NULL, NULL);
			AcceptSocket = accept(m_socket, (SOCKADDR*)&addrClient, &len);// 接受客户端连接,获取客户端的ip地址
		}
		printf("Client  Connected.\n");

		DWORD  dwThreadId;
		HANDLE  hThread = NULL;

		if (hThread)
		{
			TerminateThread(hThread, EXIT_FAILURE);
			CloseHandle(hThread);
		}

		hThread = CreateThread(NULL, NULL, AnswerThread,
			(LPVOID)AcceptSocket, 0, &dwThreadId);
		if (hThread == NULL)
		{
			printf("CreatThread  AnswerThread()  failed.\n");
		}
		else
		{
			printf("CreateThread  OK.Socket ID :%d\n", AcceptSocket);
		}
	}
	return  0;
#endif
}



