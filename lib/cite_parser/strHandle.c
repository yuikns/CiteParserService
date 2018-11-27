#include "strHandle.h"
#include <ctype.h>

int checkCharType(char a)
{ // 0 , 0 1 , l o
	if(a>'1'&&a<='9') return 1;
	else if(!DIGITLIKE(a) && isAsciiOrDigit(a)) return 2;
	return 0;
}

int spilitContent(char *dest,int dlen,const char *src,int len,char *predeli,char *nextdeli,char *np)
{
	int i;
	int j = 0;
	int k;
	int flag = 0;
	*predeli = *nextdeli = ' ';
	int connect = 0;
	int datatype = 0; // 0 unset 1 digit 2 ascii
	for(i = 0 ; i < len ; i ++)
	{
		if(isConnectCh(src[i])) {
			connect = 1;
			if(i>0)
			{
				//if(DIGITLIKE(src[i-1])&&DIGITLIKE(src[i+1]))
				if(DIGITLIKE(src[i+1]))
				{
					connect = 0;
					*nextdeli = '-';
					break;
				}
			}
		}
		if(!flag)
		{
			if(!isDelimiter(src[i]))
			{
				j = 0;
				flag = 1;
			}else if(!isBlank(src[i]))
			{
				*predeli = src[i];
			}
		}else if(isDelimiter(src[i]) && !connect)
		{
			for(k=i+1;k<len;k++)
			{
				if(isAsciiOrDigit(src[k])) break;
				if(isConnectCh(src[k]))
				{
					connect = 1;
					break;
				}else if(isDelimiter(src[k]) && !isBlank(src[k]))
					break;
				
			}
			if(!connect)
			{
				*nextdeli = src[i];
				break;
			}else
			{
				i = k;
			}
		}else if(datatype != 0 && checkCharType(src[i]) != 0 && 
			 checkCharType(src[i]) != datatype)
		{
			break;
		}
		
		if(isDelimiter(src[i]) || isConnectCh(src[i]))
		{
			continue;
		}
		if(j>=dlen) return 0;

			datatype = checkCharType(src[i]);
		
		dest[j] = src[i];
		j++;
		connect = 0;
	}
	if(!flag) return 0;
	
	dest[j] = '\0';
	j++;
	if(isBlank(*nextdeli))
	{
		for(;i<len;i++)
		{
			if(isAsciiOrDigit(src[i])){ break;}
			if(!isBlank(src[i]) && !isConnectCh(src[i]))
			{
				*nextdeli = src[i];
				break;
			}
		}
	}
	while(i<len){ 
		if(!isDelimiter(src[i])) break; 
		else if(!isBlank(src[i])) *np = src[i];
		i++;
	}
	return i;
}

int charType(char ch)
{
	if(ch >='A' && ch <= 'Z') return 1;
	if(ch >='a' && ch <= 'z') return 2;
	if(ch >='0' && ch <= '9') return 3;
	return 0;
}

int spilitStr(const char *str,int slen,int *flg,int *flen)
{
	int i;
	int ntype;
	int ltype = -1;
	if(slen <= 0) return 0;
	*flen = 0;
	for(i=0;i<slen;i++)
	{
		ntype = charType(str[i]);
		//if((ntype != ltype)&&!((i==1)&&(ltype==1)))
		if((ntype != ltype)&&(ltype!=1))
		{
			flg[*flen] = i;
			(*flen) ++;
		}
		ltype = ntype;
	}
	flg[*flen] = slen;
	return 1;
}

inline char* strCaseTransfer(char *s, int type)
{
	char *str = s;
	char *res = s;
	if (type == 0)
	{
		while(*s) *str++ = toupper(*s++);
	}else{
		while(*s) *str++ = tolower(*s++);
	}
	return res;
}

/*
int printStr(const char *str,int len)
{
	int i;
	for(i=0;i<len;i++) putchar(str[i]);
	putchar('\n');
	return 1;
}

int main(int argc , char * argv[])
{
	char str[1024] = "aaAaAAAaaAa";
	int flag[10];
	int flagLen;
	int i;
	printf("%s|%d\n",str,strlen(str));
	spilitStr(str,strlen(str),flag,&flagLen);
	for(i=0;i<flagLen;i++)
	{
		printf("%d->%d\t",flag[i],flag[i+1]);
		printStr(str+flag[i],flag[i+1]-flag[i]);
	}
	return 0;
}
*/



