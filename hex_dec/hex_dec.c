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
	int i;
	int n = 0;
	if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
	{
		i = 2;
	}
	else
	{
		i = 0;
	}
	for (;(s[i] != '\0') && ((s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >= 'A' && s[i] <= 'Z')) ; ++i)
	{
		if (tolower(s[i]) > '9')
		{
			n = 16 * n + (10 + tolower(s[i]) - 'a');
		}
		else
		{
			n = 16 * n + (tolower(s[i]) - '0');
		}
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