#include "stdafx.h"
#include <stdio.h>
#include <Winsock2.h>
#include <time.h>  
#include<conio.h>

#pragma comment(lib, "ws2_32.lib")

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

DWORD  WINAPI  AnswerThread(LPVOID  lparam)
{
	SOCKET  ClientSocket = (SOCKET)(LPVOID)lparam;
	int count = 1;
	bool isAppSocketConnent = false;
	bool isTboxSocketConnet = false;
	

	while (1)
	{
		
		char szCurrentDateTime[32];

		SOCKET  ClientSocket = (SOCKET)(LPVOID)lparam;

		memset(sendBuf, 0, 1000);
		memset(recvBuf, 0, 1000);
		time_t nowtime;
		struct tm* ptm;
		int reclen = SOCKET_ERROR;


		while(isAppSocketConnent)
		{
			if(haveConentSendtoApp)
				send(ClientSocket, sendAppBuf, sendAppBufLen, 0);// 发送消息到客户端
			haveConentSendtoApp = false;
		}
		while (reclen == SOCKET_ERROR||reclen == 0)
		{
			reclen = recv(ClientSocket, recvBuf, 1000, 0);// 接受客户端消息
#if 1
			if (InputCommand != 0)
			{
				if (InputCommand == 'e')
				{
					ackerror = true;
					printf("select command : send error ack\n");
				}
				else if (InputCommand == 'E')
				{
					ackerror = false;
					printf("select command : send correct ack\n");\
				}
				else if (InputCommand == 's')
				{
					stopsendmessage = true;
					printf("select command : stop send ack message\n");
					InputCommand = 0;
					reclen = SOCKET_ERROR;
					continue;
				}
				else if (InputCommand == 'S')
				{
					stopsendmessage = false;
					printf("select command : send message\n");
				}
				else if (InputCommand == 'q')
				{
					queryparm = TRUE;
					printf("select command : query parm\n");
				}
				else if (InputCommand == 'Q')
				{
					queryparm = FALSE;
					printf("select command : query parm\n");
				}
				else if (InputCommand == 'c')
				{
					setcontrol = TRUE;
					printf("select command : query parm\n");
				}
				else if (InputCommand == 'C')
				{
					setcontrol = FALSE;
					printf("select command : query parm\n");
				}

				InputCommand = 0;
			}
#endif
		}

		time(&nowtime);
		ptm = localtime(&nowtime);
		sprintf(szCurrentDateTime, "%d-%.2d-%.2d %.2d:%.2d:%.2d",
			ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday,
			ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
		printf("\n------------------------------------%d-----------------------------------------\n", count++);
		printf("recv time:%s len = %d\n", szCurrentDateTime, reclen);
		for (int i = 0; i < reclen; i++)
			printf("%02x,", (unsigned char )recvBuf[i]);
		printf("\n");

		int ret = Bird_soc_tbox_rev_command_deal((U8 *)recvBuf, reclen);
		if (ret == 0)
		{
			printf("no data to send\n");// 组合消息发送出去
		}
		else if (ret > 1)
		{
			if (ret == 0xffff)
			{
				isAppSocketConnent = true;
				continue;
			}
			else
			{
				isTboxSocketConnet = true;
			}
			reclen = ret;
			send(ClientSocket, sendBuf, reclen, 0);// 发送消息到客户端
			printf("\nsend time:%s len = %d\n", szCurrentDateTime, reclen);
			for (int i = 0; i < reclen; i++)
				printf("%02x,", (unsigned char)sendBuf[i]);

		}
		else
		{
			isTboxSocketConnet = true;
			send(ClientSocket, sendBuf, reclen, 0);// 发送消息到客户端
			printf("\nsend time:%s len = %d\n", szCurrentDateTime, reclen);
			for (int i = 0; i < reclen; i++)
				printf("%02x,", (unsigned char)sendBuf[i]);
		}
		printf("\n------------------------------------------------------------------------------\n");
		//closesocket(sockConn);//断开连接
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

	DWORD  dwThreadIdGetInput;
	HANDLE  hThreadGetInput;

	hThreadGetInput = CreateThread(NULL, NULL, GetInput, NULL, 0, &dwThreadIdGetInput);

	if (hThreadGetInput == NULL)
	{
		printf("CreatThread  AnswerThread()  failed.\n");
	}
	else
	{
		printf("CreateThread  OK.Socket ID :%d\n", hThreadGetInput);
	}



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
	addrSrv.sin_port = htons(6000);

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
}



