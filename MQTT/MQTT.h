#include <string.h>
typedef unsigned char uint8_t;
#define		MQTT_TypeCONNECT							1//请求连接  
#define		MQTT_TypeCONNACK							2//请求应答  
#define		MQTT_TypePUBLISH							3//发布消息  
#define		MQTT_TypePUBACK								4//发布应答  
#define		MQTT_TypePUBREC								5//发布已接收，保证传递1  
#define		MQTT_TypePUBREL								6//发布释放，保证传递2  
#define		MQTT_TypePUBCOMP							7//发布完成，保证传递3  
#define		MQTT_TypeSUBSCRIBE						8//订阅请求  
#define		MQTT_TypeSUBACK								9//订阅应答  
#define		MQTT_TypeUNSUBSCRIBE					10//取消订阅  
#define		MQTT_TypeUNSUBACK							11//取消订阅应答  
#define		MQTT_TypePINGREQ							12//ping请求  
#define		MQTT_TypePINGRESP							13//ping响应  
#define		MQTT_TypeDISCONNECT 					14//断开连接  
 
#define		MQTT_StaCleanSession					0	//清理会话 
#define		MQTT_StaWillFlag							0	//遗嘱标志
#define		MQTT_StaWillQoS								0	//遗嘱QoS连接标志的第4和第3位。
#define		MQTT_StaWillRetain						0	//遗嘱保留
#define		MQTT_StaUserNameFlag					1	//用户名标志 User Name Flag
#define		MQTT_StaPasswordFlag					1	//密码标志 Password Flag
#define		MQTT_KeepAlive								120
#define		MQTT_ClientIdentifier  "12345678"	//客户端标识符 Client Identifier
#define		MQTT_WillTopic			""				//遗嘱主题 Will Topic
#define		MQTT_WillMessage		""				//遗嘱消息 Will Message
#define		MQTT_UserName			"xiaoxiongpai"			//用户名 User Name
#define		MQTT_Password			"test"	//密码 Password


uint8_t GetDataFixedHead(uint8_t MesType,uint8_t DupFlag,uint8_t QosLevel,uint8_t Retain);
void GetDataPUBLISH(uint8_t *buff,uint8_t dup, uint8_t qos,uint8_t retain,const char *topic ,const char *msg);//获取发布消息的数据包		 	
void GetDataSUBSCRIBE(uint8_t *buff,const char *dat,unsigned int Num,uint8_t RequestedQoS);//订阅主题的数据包 Num:主题序号 RequestedQoS:服务质量要求0,1或2
void GetDataDisConnet(uint8_t *buff);//获取断开连接的数据包
void GetDataConnet(uint8_t *buff);//获取连接的数据包正确连接返回20 02 00 00
void GetDataPINGREQ(uint8_t *buff);//心跳请求的数据包成功返回d0 00
int str_to_hex(uint8_t *bufin, int len, uint8_t *bufout);
void HexStrToStr(uint8_t *source, uint8_t *dest, int sourceLen);
int hex_to_NB(uint8_t *bufin, int bufin_len, uint8_t *bufout);