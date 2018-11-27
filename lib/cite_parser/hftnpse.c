#include "hftnpse.h"
#include "minEditDistance.h"
#include "virtualcontent.h"
#include "hftctl.h"
#include <stdio.h>
#include <string.h>

#include "debuginfo.h"

inline void defineStartAndEnd(int *offset,int *offend,int limit)
{
	int tmp;
	*offend = *offset + limit;
	if(*offset > *offend)
	{
		tmp = *offset;
		*offset = *offend;
		*offend = tmp;
	}
	if(*offset < 0) *offset = 0;
	if(*offend > getPclen()) *offend = getPclen();
	
}

inline int ch2int(char ch)
{
	int val;
	if(ch =='I' || ch == 'l')
		val = 1;
	if(ch == 'o'|| ch == 'O')
		val = 0;
	else 
		val = ch - '0';
	//if(val > 9 || val < 0) return -1;
	if(val > 9 || val < 0) return 0;
	return val; 
}

inline int isPageNumber(const char *content,int limit)
{
	int a = 0;
	int b = 0;
	if(limit < 3 ) return 0;
	//if(editDistanceP("nen",3,content,3)<=0)
	//	return 3;
	if(limit > 9 ) limit = 9;
	switch(limit)
	{
	case 9:
		if((editDistanceP("nnnnennnn",9,content,9)<=0)) // 1?
		{
			a = ch2int(content[0])*1000 + ch2int(content[1])*100 + 
				ch2int(content[2])*10+ch2int(content[3]);
			b = ch2int(content[5])*1000 + ch2int(content[6])*100 + 
				ch2int(content[7])*10+ch2int(content[8]);
			if(b>a && content[4] != ' ') 
			{
				//printf("[ACC]{%d-%d}",a,b);
				return 9;
			}
		}
			
	case 8:
		if(editDistanceP("nnnennnn",8,content,8)<=0) // 1?
		{
			a = ch2int(content[0])*100 + ch2int(content[1])*10 + 
				ch2int(content[2]);
			b = ch2int(content[4])*1000 + ch2int(content[5])*100 + 
				ch2int(content[6])*10+ch2int(content[7]);
			if(b>a && content[3] != ' ') 
			{
				//printf("[ACC]{%d-%d}",a,b);
				return 8;
			}
		}
			
	case 7:
		if(editDistanceP("nnnennn",7,content,7)<=0) // 1?
		{
			a = ch2int(content[0])*100 + ch2int(content[1])*10 + ch2int(content[2]);
			b = ch2int(content[4])*100 + ch2int(content[5])*10 + ch2int(content[6]);
			if(b>a && content[3] != ' ') 
			{
				//printf("[ACC]{%d-%d}",a,b);
				return 7;
			}
		}
	case 6:
		if(editDistanceP("nnennn",6,content,6)<=0) // 1?
		{
			a = ch2int(content[0])*10 + ch2int(content[1])*1;
			b = ch2int(content[3])*100 + ch2int(content[4])*10 + ch2int(content[5]);
			if(b>a && content[2] != ' ') 
			{
				//printf("[ACC]{%d-%d}",a,b);
				return 7;
			}
		}
	case 5:
		if(editDistanceP("nnenn",5,content,5)<=0) // 1?
		{
			a = ch2int(content[0])*10 + ch2int(content[1]);
			b = ch2int(content[3])*10 + ch2int(content[4]);
			if(b>a && content[2] != ' ') 
			{
				//printf("[ACC]{%d-%d}",a,b);
				return 5;
			}
		}
	case 4:
		if(fitPattern('n',content[0])&&(content[1]=='-')&&
			fitPattern('n',content[2])&&fitPattern('n',content[3]))
		{
			a = ch2int(content[0]);
			b = ch2int(content[2])*10 + ch2int(content[3]);
			if(b>a && content[1] != ' ') 
			{
				//printf("[ACC]{%d-%d}",a,b);
				return 4;
			}
		}
	case 3:
		if(fitPattern('n',content[0])&&(content[1]=='-')&&fitPattern('n',content[2]))
		{
			a = ch2int(content[0]);
			b = ch2int(content[2]);
			if(b>a && content[1] != ' ') 
			{
				//printf("[ACC]{%d-%d}",a,b);
				return 3;
			}
		}	
	default :
		return 0;
	}
	return 0;
}
/*
inline int isPageNumberS(const char *content,int limit)
{
	if(limit < 3 ) return 0;
	if(editDistanceP("nen",3,content,3)<=0)
		return 3;
	if(editDistanceP("nen",3,content,4)<=1)
		return 4;
	if(editDistanceP("nenn",4,content,4)<=1)
		return 4;
		
	if(editDistanceP("nnenn",5,content,4)<=1)
		return 4;
	if(editDistanceP("nenn",4,content,6)<=1)
		return 4;
	
	if(limit < 5) return 0;
	if(editDistanceP("nnenn",5,content,5)<=1)
		return 5;
	if(editDistanceP("nnenn",5,content,6)<=1)
		return 6;
	

	if(editDistanceP("nnnennn",7,content,6)<=1)
		return 6;
	if(editDistanceP("nnennn",6,content,6)<=1)
		return 6;
		
	if(limit < 7) return 0;
	if(editDistanceP("nnnennn",7,content,7)<=1)
		return 7;
	if(editDistanceP("nnnennn",7,content,8)<=1)
		return 8;
	
	if(editDistanceP("nnnnennnn",9,content,8)<=1)
		return 8;
	if(editDistanceP("nnnennnn",8,content,8)<=1)
		return 8;	
	if(limit < 9) return 0;
	if(editDistanceP("nnnnennnn",9,content,9)<=1)
		return 9;
	if(editDistanceP("nnnnennnn",9,content,10)<=1)
		return 9;
	
	return 0;
}*/

// 123-345
inline int hasPPafterTheOffset(int offset,int limit)
{
	int i;	
	int offend;
	char *content = getPcontent();
	defineStartAndEnd(&offset,&offend,limit);
	int pageOffset;
	for(i=offset;i<offend;i++)
	{
		if(fitPattern('n',content[i]))
			if((pageOffset = isPageNumber(content+i,offend-i)) != 0)
			{
				//printfContextS(i,"success,before");
				//printfContextS(i+pageOffset+1,"success,next");			
				return i+pageOffset+1;
			}		
	}
	return 0;
}

// pp. 123-345 || page2.
inline int hasPPafterTheOffset2(int offset,int limit)
{
	int j;
	int i;	
	int offend;
	char *content = getPcontent();
	defineStartAndEnd(&offset,&offend,limit);
	int pageOffset;
	for(i=offset;i<offend;i++)
	{
		if(content[i]=='p' || content[i] == 'P')
		{
			if(content[i+1] == 'p'  || content[i+1] == 'P' || content[i+1] == '.')
			{
			
				for(j=3;!fitPattern('n',content[i+j]);j++)
				{
					if(i+j > offend-2) return 0;
					if(j > 4 && 
						(fitPattern('a',content[i+j]) ||
							 fitPattern('e',content[i+j])))
						continue;
				}
				if((pageOffset = isPageNumber(content+i+j,offend-i-j))!= 0) 
					return i+j+pageOffset;
				else
					return i+j+1;
			}else if(editDistanceS("page",4,content+i,4) <= 1) // threshold == 1
			{
				for(j=4;!fitPattern('n',content[i+j]);j++)
				{
					if(i+j > offend-2) return 0;
					if(j > 6 && 
						(fitPattern('a',content[i+j]) ||
							 fitPattern('e',content[i+j])))
						continue;
				}	
				if((pageOffset = isPageNumber(content+i+j,offend-i-j))!= 0) 
					return i+j+pageOffset+1;
				else
					return i+j+1;
			}
		}
	}
	return 0;
}

int hasYearafterTheOffset(int offset,int limit)
{
	//int j;
	int i;	
	int offend;
	char *content = getPcontent();
	defineStartAndEnd(&offset,&offend,limit);
	for(i=offset;i<offend-3;i++) // 19xx i_max = 1
	{
		if((content[i]=='1' && content[i+1]=='9')||
			(content[i]=='2' && content[i+1]=='0') ||
			(content[i]=='~' && content[i+1]=='9'))
		{
			if(fitPattern('n',content[i+2])&&fitPattern('n',content[i+3])) 
				return i+4;
		}
	}
	return 0;
}


// Find String like :
// XXXX, X.
int hasNameafterTheOffset0(int offset,int limit)
{
	//int j;
	int i;	
	int offend;
	char *content = getPcontent();
	defineStartAndEnd(&offset,&offend,limit);
	int tag = 0;
	for(i=offset;i<offend;i++)
	{
		if(i-tag>1) i--;
		tag = i;
		if(content[i]<'A' || content[i]>'Z') continue;
		while(fitPattern('a',content[i]))
		{
			i++;
			if(i > offend-3) return 0;
		}
		if(i-tag>=1 &&(','==content[i]))
		{
			i++;
			while(content[i]==' ') {
				i++;
				if(i > offend - 2) return 0;
			}
			if(content[i]<'A' || content[i]>'Z') continue;
			if(fitPattern('a',content[i])&&('.' == content[i+1]))
			{
				return i+2;
			}
		}
	}
	return 0;
}

// Find String like :
// XXXX, X.
int hasNameafterTheOffset1(int offset,int limit)
{
	//int j;
	int i;	
	int offend;
	char *content = getPcontent();
	defineStartAndEnd(&offset,&offend,limit);
	int tag = 0;
	for(i=offset;i<offend;i++)
	{
		if(i-tag>1) i--;
		tag = i;
		if(content[i]<'A' || content[i]>'Z') continue;
		while(fitPattern('a',content[i]))
		{
			i++;
			if(i > offend-3) return 0;
		}
		if(i-tag>=1 &&(','==content[i] || '.'==content[i] || ' '==content[i]))
		{
			i++;
			while(content[i]==' ') {
				i++;
				if(i > offend - 2) return 0;
			}
			if(content[i]<'A' || content[i]>'Z') continue;
			if(fitPattern('a',content[i])&&fitPattern('e',content[i+1]))
			{
				return i+2;
			}else
			{
				while(!fitPattern('e',content[i]) && content[i] != ' ') i++;
				return i;
			}
		}
	}
	return 0;
}

// Find String like :
// XXXX, X.
int hasNameafterTheOffset2(int offset,int limit)
{
	//int j;
	int i;	
	int offend;
	char *content = getPcontent();
	defineStartAndEnd(&offset,&offend,limit);
	int tag = 0;
	for(i=offset;i<offend;i++)
	{
		//putchar(content[i]);
		//fflush(NULL);
		if(i-tag>1) i--;
		tag = i;
		if(content[i]<'A' || content[i]>'Z') continue;
		while(fitPattern('a',content[i]))
		{
			i++;
			if(i > offend-3) return 0;
		}
		if(i-tag>=1 &&(fitPattern('e',content[i])|| content[i] == ' '))
		{
			i++;
			while(content[i]==' ') {
				i++;
				if(i > offend - 2) return 0;
			}
			if(fitPattern('a',content[i])&&fitPattern('e',content[i+1]))
			{
	//			printf("2『");
	//			for(j=tag;j<=i+1;j++) putchar(content[j]);
	//			printf("』");
				return i+2;
			}else
			{
				while(!fitPattern('e',content[i]) && content[i] != ' ') i++;
				return i;
			}
		}
	}
	return 0;
}


// XXXX, X.
//if(!parseFile("string.txt")) printf("EO");
//int offset = hasLocationafterTheOffset(0,1000);
//printf("is: %s",getPcontent()+offset);
//adfadfas [Masdfadf, MA] adfasdf
int hasLocationafterTheOffset(int offset,int limit)
{
	//int j;
	int i;	
	int offend;
	char *content = getPcontent();
	defineStartAndEnd(&offset,&offend,limit);
	int tag = 0;
	for(i=offset;i<offend;i++)
	{
		//putchar(content[i]);
		//fflush(NULL);
		if(i-tag>1) i--;
		tag = i;
		if(content[i]<'A' || content[i]>'Z') continue;
		while(fitPattern('a',content[i]))
		{
			i++;
			if(i > offend-3) return 0;
		}
		if(i-tag>=1 &&(fitPattern('e',content[i])|| content[i] == ' '))
		{
			i++;
			while(content[i]==' ') {
				i++;
				if(i > offend - 2) return 0;
			}
			if(content[i]>='A' && content[i]<='Z' && 
				content[i+1]>='A' && content[i+1]<='Z' )
			{
	//			printf("2『");
	//			for(j=tag;j<=i+1;j++) putchar(content[j]);
	//			printf("』");
				return i+2;
			}
		}
	}
	return 0;
}

int hasSeqOfTheOffset(int offset,int limit)
{
	int offend;
	char *content = getPcontent();
	defineStartAndEnd(&offset,&offend,limit);
	for(int i=offset;i<offend;i++)
	{
		if(content[i]=='['||content[i]=='|' || content[i] == 'r')
		{
			for(int j=i+1;j<offend && j < i+4;j++)
			{
				if(content[j] == ']' || content[j] == '|')
				{
					if((j-i)==1) break;
					
					//printf("${");
					//for(int k=i;k<=j;k++) putchar(content[k]);
					//printf("}");
					return j;
				}
				if((content[j]<'0' || content[j] >'9') 
					&& content[j] != 'i' && content[j] != 'l'
					&& content[j] != 'O' && content[j] != 'o')
					break;
			}
		}else if(fitPattern('n',content[i]))
		{
			i++;
			while(fitPattern('n',content[i])) i++;
			if(content[i] == '.' || content[i] == ',') return i+1;
		}
	}
	return 0;
}


int hasSeqOfTheOffset2(int offset,int limit)
{
	int offend;
	char *content = getPcontent();
	defineStartAndEnd(&offset,&offend,limit);
	int fail = 0;
	for(int i=offset;i<offend;i++)
	{
		if(content[i]=='['||content[i]=='|' || content[i] == 'r')
		{
			for(int j=i+1;j<offend && j < i+6;j++)
			{
				switch(content[j])
				{
					case ']':
					case '|':
					case '!':
						if((j-i)==1)
						{
							fail = 1;
							break;
						}
						return j;
					case '(':
					case ')':
					case ',':
					case '~':
					case '@':
					case '#':
					case '%':
					case '$':
					case '`':
					case '*':
						fail = 1;
						break;
				}
				if(fail == 1)
				{
					fail = 0;
					break;
				}
			}
		}else if(fitPattern('n',content[i]))
		{
			i++;
			while(fitPattern('n',content[i])) i++;
			if(content[i] == '.' || content[i] == ',') return i+1;
		}
	}
	return 0;
}

//int strncmp(const char *s1, const char *s2, size_t n);
#ifndef TESTMATSTR //editDistanceS
#define TESTMATSTR(a,b) {if(strncmp(a,b,strlen(a)) == 0 ) return i + strlen(a);}
//#define TESTMATSTR(a,b) {if(editDistanceS(a,b,strlen(a),strlen(a)) <= 0 ) return i + strlen(a);}
#endif
#ifndef ETTESTSTR //editDistanceS
//#define TESTMATSTR(a,b) {if(strncmp(a,b,strlen(a)) == 0 ) return i + strlen(a);}
#define ETTESTSTR(a,b) {if(editDistanceS(a,strlen(a),b,strlen(a)) <= 0 ) return i + strlen(a);}
#endif

int ONOROFF = 0;

int hasSpecialKeyWords(int offset,int limit) // in references
{
	int i;
	int offend;
	char *content = getPcontent();
	defineStartAndEnd(&offset,&offend,limit);
	for(i=offset;i<offend;i++)
	{
		if(content[i-1]==' ')
		{
			TESTMATSTR("ISO",content+i);
			TESTMATSTR("IEEE",content+i);
			TESTMATSTR("ACM",content+i);
			TESTMATSTR("Inc.",content+i);
			TESTMATSTR("Proc.",content+i);
			TESTMATSTR("pp.",content+i);
			TESTMATSTR("Ph.D. Thesis",content+i);
			TESTMATSTR("http://",content+i);
			TESTMATSTR("No.",content+i);
			
			
			ETTESTSTR("Vol.",content+i);
			ETTESTSTR("Volumn",content+i);
			ETTESTSTR("SIGCSE",content+i);
			ETTESTSTR("Bulletin",content+i);
			ETTESTSTR("Press",content+i);
			ETTESTSTR("PROCEEDINGS",content+i);

			
			//23(3) // volume
			int z = i;
			int tmp = z;
			while(fitPattern('n',content[z]) && z < offend -2)
			{
				z++;
				if(z>=offend || z-tmp > 2) break;
			}
			if(z>=offend-2) break;
			if(content[z] == '(')
			{
				z++;
				tmp = z;
				while(fitPattern('n',content[z]))
				{
					z++;
					if(z>=offend || z-tmp > 2) break;
				}
				if(z>=offend) break;
				if(content[z] == ')') return z+1;
			}
		}
	}
	return 0;
}




int hasSpecialKeyWordsN(int offset,int limit) // not in references
{
	int i;
	int offend;
	char *content = getPcontent();
	defineStartAndEnd(&offset,&offend,limit);
	for(i=offset;i<offend;i++)
	{
		if(content[i-1]==' ')
		{
			TESTMATSTR("Fig.",content+i);
			TESTMATSTR("Fig,",content+i);
			TESTMATSTR("Figure",content+i);
			TESTMATSTR("Here ",content+i);
			TESTMATSTR("We ",content+i);			
			TESTMATSTR("Are ",content+i);
			//TESTMATSTR("A ",content+i);
			TESTMATSTR("An ",content+i);
			TESTMATSTR("Our ",content+i);
			ETTESTSTR("Samples ",content+i);
			ETTESTSTR("A Proof of ",content+i);
			ETTESTSTR("In this ",content+i);
			ETTESTSTR("is a ",content+i);
		}
	}
	return 0;
}

int hasMonth(int offset,int limit)
{
	int i;
	int j;
	int offend;
	char *content = getPcontent();
	char kwds[12][10]={"January","February","March","April","May","June","July",
			"August","September","October","November","December"};
	defineStartAndEnd(&offset,&offend,limit);
	for(i=offset;i<offend;i++)
	{
		if(content[i-1]==' ')
		{
			for(j=0;j<12;j++)
			{
				TESTMATSTR(kwds[j],content+i);
				if((strncmp(kwds[j],content+i,3) == 0)&& content[i+3]=='.') 
					return i + 4;
			}
			if(strncmp("Sept",content+i,4) == 0) return i+5;
			
		}
	}
	return 0;
}


int hasWords(int offset,int limit)
{
	int i;
	int offend;
	int sum;
	int diff;
	char key;
	char *content = getPcontent();
	defineStartAndEnd(&offset,&offend,limit);
	for(i=offset;i<offend;i++)
	{
		sum = 0;
		diff= 0;
		while(fitPattern('a',content[i]) && i<offend-1)
		{
			sum++;
			i++;
			if(diff == 0) key = content[i];
			else if(key != content[i])
			{
				diff++;
				key = content[i];
			}
		}
		if(sum >= 4 && diff>2) return sum+1+i;
	}
	return 0;	
}

/*
int i;
int offend;
char *content = getPcontent();
int in = 0 ;
int num = 0;
defineStartAndEnd(&offset,&offend,limit);
for(i=offset;i<offend-3;i++)
{
	if(in)
	{
		if(!fitPattern('a',content[i]))
		{
			in = 0;
		}
	}else if(fitPattern('a',content[i])&&fitPattern('a',content[i+1])
		&&fitPattern('a',content[i+2])&&fitPattern('a',content[i+3]))
	{
		in = 1;
		num ++ ;
	}
}
return num;
*/
int wordsNumber(int offset,int limit)
{
	return keysNumber(offset,limit,hasWords);
}




/*
	int pLen;
	int nLen;
	int prev[10];
	int next[10];
 */
int getCloseKWD(int offset,CloseKWD *closeKWD,CKWDfun ckfunc)
{
	int offend = getPclen();
	int nowOffset;
	closeKWD->pLen = 0;
	closeKWD->nLen = 0;
	// calculate before
	int i;
	int x = 0;
	//printf("<");
	for(i=0;i<offset;i++)
	{
		if((nowOffset = ckfunc(i,offset-i)) > 0)
		{
			x ++ ;
			//printf("(%d",nowOffset);
			i = nowOffset ;
			if(closeKWD->pLen)
			{
				//printf("D%d^%d)",(closeKWD->pLen-1)%10,closeKWD->prev[(closeKWD->pLen-1)%10]);
				if((nowOffset - closeKWD->prev[(closeKWD->pLen-1)%10]) > 10)
				{
					closeKWD->prev[(closeKWD->pLen)%10] = nowOffset;
					closeKWD->pLen ++;
				}
			}else
			{
				closeKWD->prev[(closeKWD->pLen)%10] = i;
				//printf("N%d^%d-%d)",i,(closeKWD->pLen)%10,);
				closeKWD->pLen ++;
			}
			
		}else
			break;
	}
	//printf(">");
	//printf("<LL>%d^%d#",closeKWD->pLen,x);
	if(closeKWD->pLen > 10)
	{
		closeKWD->pStart = closeKWD->pLen % 10;
		closeKWD->pLen = 10;
	}else
	{
		closeKWD->pStart = 0;	
	}
	// calculate next
	for(int i=offset;i<offend;i++)
	{
		if((nowOffset = ckfunc(i,offend-i)) > 0)
		{
			i = nowOffset;
			if(closeKWD->nLen)
			{
				if((nowOffset - closeKWD->next[closeKWD->nLen-1]) > 10)
				{
					closeKWD->next[closeKWD->nLen] = nowOffset;
					closeKWD->nLen ++;
				}
			}else
			{
				closeKWD->next[closeKWD->nLen] = i;
				closeKWD->nLen ++;
			}
			
		}else
			break;
		if(closeKWD->nLen >= 10) break;
	}
	
	return 1;
}


double asciiCodeDensity(int offset,int limit)
{
	int i;	
	int offend;
	int num = 0;
	int length;
	char *content = getPcontent();
	defineStartAndEnd(&offset,&offend,limit);
	length = offend - offset;
	for(i=offset;i<offend;i++)
	{
		if(fitPattern('a',content[i])) num++;
		if(content[i]==' ') length = 0;
	}
	if(length == 0) return 1;
	return (double)num/length;
}


double dataDensity(int offset,int limit)
{
	int i;	
	int offend;
	int num = 0;
	int length;
	char *content = getPcontent();
	defineStartAndEnd(&offset,&offend,limit);
	length = offend - offset;
	for(i=offset;i<offend;i++)
	{
		if(fitPattern('d',content[i])) num++;
	}
	return (double)num/length;
}

inline double wordsDensity(int offset,int limit)
{
	int offend;
	int num = wordsNumber(offset,limit);
	defineStartAndEnd(&offset,&offend,limit);
	return (double)num/(offend-offset);
}


inline double effectiveWordsDensity(int offset,int limit)
{
	int i;
	int offend;
	int sum;
	int allsum = 0;
	int diff;
	char key;
	int allcontent;
	char *content = getPcontent();
	defineStartAndEnd(&offset,&offend,limit);
	allcontent = offset;
	for(i=offset;i<offend;i++)
	{
		if(!fitPattern('a',content[i]))
		{
			allcontent --;
			continue;
		}
		sum = 0;
		diff= 0;
		while(fitPattern('a',content[i]) && i<offend-1)
		{
			sum++;
			i++;
			if(diff == 0) key = content[i];
			else if(key != content[i])
			{
				diff++;
				key = content[i];
			}
		}
		if(sum >= 4 && diff>2)
		{
			allsum+=sum;
		}
	}
	if(allcontent == 0) return -1;
	return allsum/allcontent;
}


int keysNumber(int offset,int limit,int (*keyFind)(int,int))
{
	int offend;
	int num = 0;
	//int len = getPclen();
	defineStartAndEnd(&offset,&offend,limit);
	while((offset=keyFind(offset,offend-offset))>0)
	{
		//if(offend - offset < 0 || offset > len) return num;
		num++;
	}
	return num;	
}



