#include "MQTT.h"
#include<stdio.h>
#include <ctype.h>

//uint8_t duizhao[16]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};

uint8_t GetDataFixedHead(uint8_t MesType,uint8_t DupFlag,uint8_t QosLevel,uint8_t Retain)
{
	uint8_t dat = 0;
	dat = (MesType & 0x0f) << 4;
	dat |= (DupFlag & 0x01) << 3;
	dat |= (QosLevel & 0x03) << 1;
	dat |= (Retain & 0x01);
	return dat;
}
void GetDataConnet(uint8_t *buff)//��ȡ���ӵ����ݰ���ȷ���ӷ���20 02 00 00
{
	unsigned int i,len,lennum = 0;
	uint8_t *msg;
	buff[0] = GetDataFixedHead(MQTT_TypeCONNECT,0,0,0);
	buff[2] = 0x00;
	buff[3] = 0x04;
	buff[4] = 'M';
	buff[5] = 'Q';
	buff[6] = 'T';
	buff[7] = 'T';
	buff[8] = 0x04;//Э�鼶�� Protocol Level
	buff[9] = 0 | (MQTT_StaCleanSession << 1) | (MQTT_StaWillFlag << 1)
							| (MQTT_StaWillQoS << 3) | (MQTT_StaWillRetain << 5) 
							|	(MQTT_StaPasswordFlag << 6) |(MQTT_StaUserNameFlag << 7);//���ӱ�־
	buff[10] = MQTT_KeepAlive >> 8;
	buff[11] = MQTT_KeepAlive;
	len = strlen(MQTT_ClientIdentifier);
	buff[12] = len >> 8;
	buff[13] = len;
	msg = MQTT_ClientIdentifier;
	for(i = 0;i<len;i++)
	{
		buff[14+i] =  msg[i];
	}
	lennum += len;
	if(MQTT_StaWillFlag)
	{
		len = strlen(MQTT_WillTopic);
		buff[13 + lennum + 1] = len >> 8;
		buff[13 + lennum + 2] = len;
		lennum += 2;
		msg = MQTT_WillTopic;
		for(i = 0;i<len;i++)
		{
			buff[14+lennum+i] =  msg[i];
		}
		lennum += len;
		len = strlen(MQTT_WillMessage);
		buff[12] = len >> 8;
		buff[13] = len;
		lennum += 2;
		msg = MQTT_WillMessage;
		for(i = 0;i<len;i++)
		{
			buff[14+lennum+i] =  msg[i];
		}
		lennum += len;
	}
	if(MQTT_StaUserNameFlag)
	{
		len = strlen(MQTT_UserName);
		buff[13 + lennum + 1] = len >> 8;
		buff[13 + lennum + 2] = len;
		lennum += 2;
		msg = MQTT_UserName;
		for(i = 0;i<len;i++)
		{
			buff[14+lennum+i] =  msg[i];
		}
		lennum += len;
		
	}
	if(MQTT_StaPasswordFlag)
	{
		len = strlen(MQTT_Password);
		buff[13 + lennum + 1] = len >> 8;
		buff[13 + lennum + 2] = len;
		lennum += 2;
		msg = MQTT_Password;
		for(i = 0;i<len;i++)
		{
			buff[14+lennum+i] =  msg[i];
		}
		lennum += len;
	}
	buff[1] = 13 + lennum - 1;
}
void GetDataDisConnet(uint8_t *buff)//��ȡ�Ͽ����ӵ����ݰ�
{
	buff[0] = 0xe0;
	buff[1] = 0;
}
void GetDataPINGREQ(uint8_t *buff)//������������ݰ��ɹ�����d0 00
{
	buff[0] = 0xc0;
	buff[1] = 0;
}
/*
	�ɹ�����90 0x 00 Num RequestedQoS
*/
void GetDataSUBSCRIBE(uint8_t *buff,const char *dat,unsigned int Num,uint8_t RequestedQoS)//������������ݰ� Num:������� RequestedQoS:��������Ҫ��0,1��2
{
	unsigned int i,len = 0,lennum = 0; 
	buff[0] = 0x82;
	len = strlen(dat);
	buff[2] = Num>>8;
	buff[3] = Num;
	buff[4] = len>>8;
	buff[5] = len;
	for(i = 0;i<len;i++)
	{
		buff[6+i] = dat[i];
	}
	lennum = len;
	buff[6 + lennum ] = RequestedQoS;
	buff[1] = lennum + 5;
}
void GetDataPUBLISH(uint8_t *buff,uint8_t dup, uint8_t qos,uint8_t retain,const char *topic ,const char *msg)//��ȡ������Ϣ�����ݰ�
{
	unsigned int i,len=0,lennum=0;
	buff[0] = GetDataFixedHead(MQTT_TypePUBLISH,dup,qos,retain);
	len = strlen(topic);
	buff[2] = len>>8;
	buff[3] = len;
	for(i = 0;i<len;i++)
	{
		buff[4+i] = topic[i];
	}
	lennum = len;
	len = strlen(msg);
	for(i = 0;i<len;i++)
	{
		buff[4+i+lennum] = msg[i];
	}
	lennum += len;
	buff[1] = lennum + 2;
}



int str_to_hex(uint8_t *bufin, int len, uint8_t *bufout)
{
    int i = 0;
    if (NULL == bufin || len <= 0 || NULL == bufout)
    {
        return -1;
    }
    for(i = 0; i < len; i++)
    {
        sprintf(bufout+i*2, "%02X", bufin[i]);
    }
    return 0;
}




/*
void HexStrToStr(uint8_t *source, uint8_t *dest, int sourceLen)
{
    short i;
    uint8_t highByte, lowByte;
    for (i = 0; i < sourceLen; i += 2)
    {
        highByte = toupper(source[i]);
        lowByte  = toupper(source[i + 1]);
        if (highByte > 0x39)
            highByte -= 0x37;
        else
            highByte -= 0x30;
        if (lowByte > 0x39)
            lowByte -= 0x37;
        else
            lowByte -= 0x30;
        dest[i / 2] = (highByte << 4) | lowByte;
    }
    return ;
}

*/










