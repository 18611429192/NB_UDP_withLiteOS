#include <string.h>
typedef unsigned char uint8_t;
#define		MQTT_TypeCONNECT							1//��������  
#define		MQTT_TypeCONNACK							2//����Ӧ��  
#define		MQTT_TypePUBLISH							3//������Ϣ  
#define		MQTT_TypePUBACK								4//����Ӧ��  
#define		MQTT_TypePUBREC								5//�����ѽ��գ���֤����1  
#define		MQTT_TypePUBREL								6//�����ͷţ���֤����2  
#define		MQTT_TypePUBCOMP							7//������ɣ���֤����3  
#define		MQTT_TypeSUBSCRIBE						8//��������  
#define		MQTT_TypeSUBACK								9//����Ӧ��  
#define		MQTT_TypeUNSUBSCRIBE					10//ȡ������  
#define		MQTT_TypeUNSUBACK							11//ȡ������Ӧ��  
#define		MQTT_TypePINGREQ							12//ping����  
#define		MQTT_TypePINGRESP							13//ping��Ӧ  
#define		MQTT_TypeDISCONNECT 					14//�Ͽ�����  
 
#define		MQTT_StaCleanSession					0	//����Ự 
#define		MQTT_StaWillFlag							0	//������־
#define		MQTT_StaWillQoS								0	//����QoS���ӱ�־�ĵ�4�͵�3λ��
#define		MQTT_StaWillRetain						0	//��������
#define		MQTT_StaUserNameFlag					1	//�û�����־ User Name Flag
#define		MQTT_StaPasswordFlag					1	//�����־ Password Flag
#define		MQTT_KeepAlive								120
#define		MQTT_ClientIdentifier  "12345678"	//�ͻ��˱�ʶ�� Client Identifier
#define		MQTT_WillTopic			""				//�������� Will Topic
#define		MQTT_WillMessage		""				//������Ϣ Will Message
#define		MQTT_UserName			"xiaoxiongpai"			//�û��� User Name
#define		MQTT_Password			"test"	//���� Password


uint8_t GetDataFixedHead(uint8_t MesType,uint8_t DupFlag,uint8_t QosLevel,uint8_t Retain);
void GetDataPUBLISH(uint8_t *buff,uint8_t dup, uint8_t qos,uint8_t retain,const char *topic ,const char *msg);//��ȡ������Ϣ�����ݰ�		 	
void GetDataSUBSCRIBE(uint8_t *buff,const char *dat,unsigned int Num,uint8_t RequestedQoS);//������������ݰ� Num:������� RequestedQoS:��������Ҫ��0,1��2
void GetDataDisConnet(uint8_t *buff);//��ȡ�Ͽ����ӵ����ݰ�
void GetDataConnet(uint8_t *buff);//��ȡ���ӵ����ݰ���ȷ���ӷ���20 02 00 00
void GetDataPINGREQ(uint8_t *buff);//������������ݰ��ɹ�����d0 00
int str_to_hex(uint8_t *bufin, int len, uint8_t *bufout);
void HexStrToStr(uint8_t *source, uint8_t *dest, int sourceLen);
int hex_to_NB(uint8_t *bufin, int bufin_len, uint8_t *bufout);