#include "stdafx.h"
#include <stdio.h>
#include < time.h>  

extern char sendBuf[];
extern char recvBuf[];
bool ackerror = false;

U8 parmSet[] = { 0x23, 0x23, 0x80, 0x01, 0x54, 0x58, 0x38, 0x36, 0x36, 0x39, 0x35, 0x34, 0x32, 0x33, 0x34, 0x34, 0x33, 0x36, 0x35, 0x35, 0x39, 0x01, 0x00, 0x53, 0x18, 0x06, 0x26, 0x13, 0x53, 0x17, 0x0d, 0x01, 0x03, 0xe8, 0x02, 0x00, 0x0a, 0x03, 0x03, 0xe8, 0x04, 0x06, 0x05, 0x31, 0x32, 0x38, 0x2e, 0x34, 0x33, 0x06, 0x2f, 0x9b, 0x07, 0x56, 0x31, 0x2e, 0x30, 0x31, 0x08, 0x56, 0x31, 0x2e, 0x30, 0x32, 0x09, 0x78, 0x0b, 0x00, 0x3c, 0x0c, 0x05, 0x80, 0x56, 0x31, 0x2e, 0x30, 0x32, 0x5f, 0x4e, 0x42, 42, 0x44, 0x5f, 0x32, 0x30, 0x31, 0x37, 0x31, 0x30, 0x32, 0x34, 0x5f, 0x44, 0x57, 0x36, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0x00, 0x03 };

int sendQueryParm(void)
{
		int i, len = 0;
		U8 check_code = 0;
		//23 23 01 fe 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
		U8 test[] = { 0x23, 0x23, 0x80, 0xFE, 0x54, 0x58, 0x38, 0x36, 0x36, 0x39, 0x35, 0x34, 0x32, 0x33, 0x34, 0x34, 0x33, 0x36, 0x35, 0x35, 0x39, 0x01, 0x00, 0x1E, 0x04, 0x01, 0x01, 0x03, 0x58, 0x45, 0x00, 0x01, 0x38, 0x39, 0x38, 0x36, 0x30, 0x30, 0x31, 0x32, 0x31, 0x31, 0x31, 0x36, 0x35, 0x34, 0x30, 0x33, 0x35, 0x30, 0x35, 0x32, 0x00, 0x00, 0xC4 };
		memset(sendBuf, 0, len);
		memcpy(sendBuf, recvBuf, 100);


		len = 0;
		sendBuf[len++] = 0x23;
		sendBuf[len++] = 0x23;
		sendBuf[len++] = 0x80;
		sendBuf[len++] = 0xFE;

		//get current time
		char szCurrentDateTime[32];
		time_t nowtime;
		struct tm* ptm;
		time(&nowtime);
		ptm = localtime(&nowtime);


		len = 24;

		if (1)
		{
			sendBuf[len++] = (ptm->tm_year - 100) / 10 * 16 + (ptm->tm_year - 100) % 10;
			sendBuf[len++] = (ptm->tm_mon + 1) / 10 * 16 + (ptm->tm_mon + 1) % 10;
			sendBuf[len++] = ptm->tm_mday / 10 * 16 + ptm->tm_mday % 10;
			sendBuf[len++] = ptm->tm_hour / 10 * 16 + ptm->tm_hour % 10;
			sendBuf[len++] = ptm->tm_min / 10 * 16 + ptm->tm_min % 10;
			sendBuf[len++] = ptm->tm_sec / 10 * 16 + ptm->tm_sec % 10;
		}
		sendBuf[len++] = 13;
		sendBuf[len++] = 0x01;
		sendBuf[len++] = 0x02;
		sendBuf[len++] = 0x03;
		sendBuf[len++] = 0x04;
		sendBuf[len++] = 0x05;
		sendBuf[len++] = 0x06;
		sendBuf[len++] = 0x07;
		sendBuf[len++] = 0x08;
		sendBuf[len++] = 0x09;
		//sendBuf[len++] = 0x0A;
		sendBuf[len++] = 0x0B;
		sendBuf[len++] = 0x0C;
		sendBuf[len++] = 0x80;
		sendBuf[len++] = 0x81;

		sendBuf[22] = 0;
		sendBuf[23] = (len - 24);

		//校验码
		for (i = 2; i< len; i++)
			check_code ^= sendBuf[i];

		sendBuf[len++] = check_code;

		return len;
}


int sendControl(void)
{
	int i, len = 0;
	U8 check_code = 0;

	memset(sendBuf, 0, len);
	memcpy(sendBuf, recvBuf, 100);

	len = 0;
	sendBuf[len++] = 0x23;
	sendBuf[len++] = 0x23;
	sendBuf[len++] = 0x82;
	sendBuf[len++] = 0xFE;

	//get current time
	char szCurrentDateTime[32];
	time_t nowtime;
	struct tm* ptm;
	time(&nowtime);
	ptm = localtime(&nowtime);


	len = 24;

	if (1)
	{
		sendBuf[len++] = (ptm->tm_year - 100) / 10 * 16 + (ptm->tm_year - 100) % 10;
		sendBuf[len++] = (ptm->tm_mon + 1) / 10 * 16 + (ptm->tm_mon + 1) % 10;
		sendBuf[len++] = ptm->tm_mday / 10 * 16 + ptm->tm_mday % 10;
		sendBuf[len++] = ptm->tm_hour / 10 * 16 + ptm->tm_hour % 10;
		sendBuf[len++] = ptm->tm_min / 10 * 16 + ptm->tm_min % 10;
		sendBuf[len++] = ptm->tm_sec / 10 * 16 + ptm->tm_sec % 10;
	}
	//0x01，远程升级
	//0x02，终端关机
	//0x03，终端复位
	//0x04，恢复出厂
	//0x05，断开数据连接
	//0x06，终端报警
	//0x07，开启抽样检测链路

	sendBuf[len++] = 0x05; 

	sendBuf[22] = 0;
	sendBuf[23] = (len - 24);

	//校验码
	for (i = 2; i< len; i++)
		check_code ^= sendBuf[i];

	sendBuf[len++] = check_code;

	return len;
}
//终端上报的登入请求
void TB_Soc_Send_login_ReqBuffer(U32 len,bool issendservicetime)
{
	int i;
	U8 check_code = 0;
	//23 23 01 fe 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	U8 test[] = { 0x23, 0x23, 0x01, 0x01, 0x54, 0x58, 0x38, 0x36, 0x36, 0x39, 0x35, 0x34, 0x32, 0x33, 0x34, 0x34, 0x33, 0x36, 0x35, 0x35, 0x39, 0x01, 0x00, 0x1E, 0x04, 0x01, 0x01, 0x03, 0x58, 0x45, 0x00, 0x01, 0x38, 0x39, 0x38, 0x36, 0x30, 0x30, 0x31, 0x32, 0x31, 0x31, 0x31, 0x36, 0x35, 0x34, 0x30, 0x33, 0x35, 0x30, 0x35, 0x32, 0x00, 0x00, 0xC4 };

	memset(sendBuf, 0, len);
	//起始符
	memcpy(sendBuf, recvBuf, len);

	//sendBuf[3] = 0x01;

	if (ackerror)
	{
		sendBuf[3] = 0x02;
		printf("\nsend error ack message!\n");
	}
	else
		sendBuf[3] = 0x01;

	//get current time

	char szCurrentDateTime[32];
	time_t nowtime;
	struct tm* ptm;
	time(&nowtime);
	ptm = localtime(&nowtime);

	if (issendservicetime)
	{
		sendBuf[24] = (ptm->tm_year - 100) / 10 * 16 + (ptm->tm_year - 100) % 10;
		sendBuf[25] = (ptm->tm_mon + 1) / 10 * 16 + (ptm->tm_mon + 1) % 10;
		sendBuf[26] = ptm->tm_mday / 10 * 16 + ptm->tm_mday % 10;
		sendBuf[27] = ptm->tm_hour / 10 * 16 + ptm->tm_hour % 10;
		sendBuf[28] = ptm->tm_min / 10 * 16 + ptm->tm_min % 10;
		sendBuf[29] = ptm->tm_sec / 10 * 16 + ptm->tm_sec % 10;
	}
	//校验码
	for (i = 2; i<len-1; i++)
		check_code ^= sendBuf[i];


	sendBuf[len-1] = check_code;

	//memcpy(sendBuf, test, len);
}
