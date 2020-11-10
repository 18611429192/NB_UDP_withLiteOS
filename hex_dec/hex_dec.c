#include <stdio.h>
#include "hex_dec.h"

int tolower(int c)
{
	if (c >= 'A' && c <= 'Z')
	{
		return c + 'a' - 'A';
}
	else
	{
		return c;
	}
}
 
 
//将十六进制的字符串转换成整数  
int Fixed_key(char s[])
{
	int n = 0;
	
	int len = strlen(s);
	
	//printf("hanshu s = %s,len = %d",s,len);
	//for(int i =0;i<len;i++){
	//	printf("s[%d] = %c",i,s[i]);
	//}
	
	for(int i =1;i<len-2;){
		n = n*10 + (s[i]-'0');
		i = i+2;
	}
	
	return n;
}



void arrayToStr(char *buf,char *out)
{
    char strBuf[33] = {0};
    char pbuf[32];
		unsigned int buflen = strlen(buf);
    int i;
    for(i = 0; i < buflen; i++)
    {
        sprintf(pbuf, "%02X", buf[i]);
        strncat(strBuf, pbuf, 2);
    }
    strncpy(out, strBuf, buflen * 2);
    //printf("out = %s\n", out);
    return ;
}