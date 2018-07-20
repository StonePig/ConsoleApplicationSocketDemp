#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <string>

#define RECEIVE_DATA_LENGTH 1000


U8 realinfoDisplay(U16 len);
extern void bird_set_aes128_encode(U8 *data, U16 l);
extern void bird_set_inv_aes128_encode(U8 *data, U16 l);
extern void TB_Soc_Send_login_ReqBuffer(U32 len,bool issendservicetime);
extern int sendQueryParm(void);
extern int sendControl(void);
//int reclen;
extern bool queryparm;
extern bool setcontrol;
extern char sendBuf[];
extern char recvBuf[];
extern char sendAppBuf[];
extern int sendAppBufLen;
extern bool haveConentSendtoApp;


int Bird_soc_tbox_rev_command_deal(U8 *p_receive_data, U32 receive_length)
{
	U32 length = 0;
	U8 command[2];
	U8 time_rec[6] = { 0 };
	U8 time[6] = { 0 };
	U8 vin[17];
	U8 encrypt[1];
	U8 datalength[2];
	U16 data_len = 0;
	U8 rest_buf[RECEIVE_DATA_LENGTH];


	if (memcmp(p_receive_data, "123456", 6) == 0)
	{
		return 0xffff;
	}
	else
	{
		//sendAppBufLen = strlen("test...");
		//memcpy(sendAppBuf, "test...", sendAppBufLen);
		//haveConentSendtoApp = true;
		///return 1;
	}
	if (receive_length<19)
		return 0;
	p_receive_data = p_receive_data + 2;
	receive_length = receive_length - 2;

	memset(command, 0, sizeof(command));
	memcpy(command, p_receive_data, 2);
	length = length + 2;

	memset(vin, 0, sizeof(vin));
	memcpy(vin, p_receive_data + length, 17);
	length = length + 17;

	memset(encrypt, 0, sizeof(encrypt));
	memcpy(encrypt, p_receive_data + length, 1);
	length = length + 1;

	memset(datalength, 0, sizeof(datalength));
	memcpy(datalength, p_receive_data + length, 2);
	length = length + 2;

	data_len = datalength[0] * 256 + datalength[1];


	if (receive_length - length == data_len + 1)
	{
		memset(rest_buf, 0, sizeof(rest_buf));
		memcpy(rest_buf, p_receive_data + length, data_len);
	}
	else
	{
		printf("recive message length error\n");
		return 0;
	}

	if (encrypt[0] == 0x03)
		bird_set_inv_aes128_encode(rest_buf, data_len / 16 + 1);

	if ((command[0] == 0x01) && (command[1] == 0xfe))
	{
		printf("login command\n");
		sendAppBufLen = strlen("login");
		memcpy(sendAppBuf, "login", sendAppBufLen);
		haveConentSendtoApp = true;
		TB_Soc_Send_login_ReqBuffer(receive_length+2,true);
		return 1;
	}
	else if ((command[0] == 0x04) && (command[1] == 0xfe))
	{
		printf("logout command\n");
		TB_Soc_Send_login_ReqBuffer(receive_length + 2 , true);
		return 1;
	}
	else if ((command[0] == 0x07) && (command[1] == 0xfe))
	{
		if (queryparm)
		{
			printf("query parm command\n");
			return(sendQueryParm());
			
		}
		else if (setcontrol)
		{
			printf("set control command\n");
			return(sendControl());

		}
		else
		{
			printf("heart command\n");
			TB_Soc_Send_login_ReqBuffer(receive_length + 2, true);
		}
		return 1;
	}
	else if ((command[0] == 0x08) && (command[1] == 0xfe))
	{
		printf("timing command\n");
		TB_Soc_Send_login_ReqBuffer(receive_length + 2, true);
		return 1;
	}
	else if ((command[0] == 0x02) && (command[1] == 0xfe))
	{
		printf("realinfo command\n");
		TB_Soc_Send_login_ReqBuffer(receive_length + 2 , false);
		realinfoDisplay(receive_length + 2);
		return 1;
	}
	else if ((command[0] == 0x03) && (command[1] == 0xfe))
	{
		printf("resendinfo command\n");
		TB_Soc_Send_login_ReqBuffer(receive_length + 2 , false);
		return 1;
	}
	else if ((command[0] == 0x80) && (command[1] == 0x01 || command[1] == 0x02))
	{
		printf("search command\n");
		//TB_Soc_Send_login_ReqBuffer(receive_length + 2, true);
		return 0;
	}
	else if ((command[0] == 0x81) && (command[1] == 0xfe))
	{
		printf("set command\n");
		TB_Soc_Send_login_ReqBuffer(receive_length + 2, true);
		return 1;
	}
	else if ((command[0] == 0x82) && (command[1] == 0xfe))
	{
		printf("control command\n");
		TB_Soc_Send_login_ReqBuffer(receive_length + 2, true);
		return 1;
	}
	else if ((command[0] == 0xC0) && (command[1] == 0xfe))
	{
		printf("self detect command\n");
		TB_Soc_Send_login_ReqBuffer(receive_length + 2, true);
		return 1;
	}
	else 
	{
		printf("other command: %d %d\n", command[0], command[1]);
		//TB_Soc_Send_login_ReqBuffer(receive_length + 2);
		return 0;
	}
	return 0;
}


//实时信息
U8 realinfoDisplay(U16 len)
{
	U16 length = 0;
	U16 i,j,n,cmd = 0x01;

	//VIN
	printf("VIN : %s\n", &recvBuf[4]);

	//数据单元长度
	length = recvBuf[22] / 256 + recvBuf[23] % 256;

	for (i = 30; i < length;)
	{
		if (recvBuf[i] == 0x01)
		{
			cmd = 1;
			printf("整车数据 :\n");
			i++;
			printf("车辆状态 :%d ", recvBuf[i++]);
			printf("充电状态 :%d ", recvBuf[i++]);
			printf("运行状态 :%d ", recvBuf[i++]);
			printf("车速 :%dKM/H", (recvBuf[i]*256+ recvBuf[i+1])/10);
			i = i + 2;
			printf("累计里程 :%dKM ", ((recvBuf[i] << 24) + (recvBuf[i + 1] << 16) + (recvBuf[i + 2] << 8) + recvBuf[i + 3]) /10);
			i = i + 4;
			printf("总电压 :%dV", (recvBuf[i] * 256 + recvBuf[i + 1]) / 10);
			i = i + 2;
			printf("总电流 :%dV", (recvBuf[i] * 256 + recvBuf[i + 1]) / 10);
			i = i + 2;
			printf("SOC :%d ", recvBuf[i++]);
			printf("DC/DC :%d ", recvBuf[i++]);
			printf("档位 :%d ", recvBuf[i++]);
			printf("绝缘电阻 :%dKOU", (recvBuf[i] * 256 + recvBuf[i + 1]));
			i = i + 2;
			printf("加速踏板行程值 :%d ", recvBuf[i++]);
			printf("制动踏板状态 :%d ", recvBuf[i++]);

		}
		else if (recvBuf[i] == 0x02)
		{
			cmd = 2;
			printf("\n驱动电机数据 :\n");
			i++;
			n = recvBuf[i++];
			printf("驱动电机总数 : %d\n",n);
			for (j = 0; j < n; j++)
			{
				i = i + 12;
			}
		}
		else if (recvBuf[i] == 0x03)
		{
			cmd = 3;
			printf("\n燃料电池数据 :\n");
			i++;
			n = recvBuf[i + 6]*256+ recvBuf[i + 7];
			i = i + 18 + n;
			
		}
		else if (recvBuf[i] == 0x04)
		{
			cmd = 4;
			printf("\n发动机数据 :\n");
			i++;
			printf("发动机状态 :%d ", recvBuf[i++]);
			printf("曲轴转速 :%d r/min", (recvBuf[i] * 256 + recvBuf[i + 1]));
			i = i + 2;
			printf("燃料消耗率 :%d L/100KM", (recvBuf[i] * 256 + recvBuf[i + 1]) / 100);
			i = i + 2;

		}
		else if (recvBuf[i] == 0x05)
		{
			cmd = 5;
			printf("\n车辆位置数据 :\n");
			i++;
			printf("位置状态 :%d ", recvBuf[i++]);
			float jd = (float)((recvBuf[i] << 24) + (recvBuf[i + 1] << 16) + (recvBuf[i + 2] << 8) + recvBuf[i + 3]) / 1000000;
			printf("经度 :%9.6f ", jd);
			i = i + 4;
			jd = (float)(((recvBuf[i] &0xFF) << 24) | ((recvBuf[i + 1] & 0xFF) << 16) | ((recvBuf[i + 2] & 0xFF) << 8) + (recvBuf[i + 3] & 0xFF)) / 1000000;
			printf("纬度 :%9.6f ", jd);
			i = i + 4;
		}
		else if (recvBuf[i] == 0x06)
		{
			cmd = 6;
			printf("\n极值数据 :\n");
			i++;
			i = i + 14;
		}
		else if (recvBuf[i] == 0x07)//9 bytes
		{
			cmd = 7;
			printf("\n报警数据 :\n");
			i++;
			i = i + 9;

		}
		else if (recvBuf[i] == 0x08)
		{
			cmd = 8;
			printf("\n可充电储能装置电压数据 :\n");
			i++;
			return 1;
		}
		else if (recvBuf[i] == 0x09)
		{
			cmd = 9;
			printf("\n可充电储能装置温度数据 :\n");
			i++;
			return 1 ;
		}
		else
		{
			cmd++;

		}
	}
	return 1;
}
