#include "common.h"
#include "virtualcontent.h"
#include "minEditDistance.h"
#include "hftctl.h"
#include <stdio.h>


//TODO MAY USELESS ... 
//int getLastElement(int (*callback)(int offset,int limit));

int haveDifferneces(int dest,int src) // extend
{
	if(src<0) src = 0;
	if(dest<0) dest = 0;
	if(src>=getPclen()) src = getPclen()-1;
	if(dest>=getPclen()) dest = getPclen()-1;

	if(INABSDIFF(dest,src)) return 0;

	int th = 0;
	char *content = getPcontent();

	if(dest < src)
	{
		int tmp = src;
		src = dest;
		dest = tmp;
	}
	for(int i=src;i<dest-1;i++)
	{
		/*
		// no ascii code
		//if(!(content[i]>='a'&&content[i]<='z')||(content[i]>='A'&&content[i]<='Z')) th++;
		if(content[i] != ' ' && content[i] != '\r' && content[i] != '\n' &&fitPattern()) th++;
		
		if(th>2) return 1;
		*/
		
		if(fitPattern('a',content[i])&&fitPattern('a',content[i+1])) 
		{
			//return 1;
			th++;
			if(th > 0) return 1;
			i+=2;
		}
	}
	return 0;
}

int haveDiffernecesD(int dest,int src) // extend
{
	if(src<0) src = 0;
	if(dest<0) dest = 0;
	if(src>=getPclen()) src = getPclen()-1;
	if(dest>=getPclen()) dest = getPclen()-1;

	if(EXINABSDIFF(dest,src)) return 0;

	int th = 0;
	char *content = getPcontent();

	if(dest < src)
	{
		int tmp = src;
		src = dest;
		dest = tmp;
	}
	for(int i=src;i<dest-1;i++)
	{
		/*
		// no ascii code
		//if(!(content[i]>='a'&&content[i]<='z')||(content[i]>='A'&&content[i]<='Z')) th++;
		if(content[i] != ' ' && content[i] != '\r' && content[i] != '\n' &&fitPattern()) th++;
		
		if(th>2) return 1;
		*/
		
		if(fitPattern('a',content[i])&&fitPattern('a',content[i+1])) 
		{
			//return 1;
			th++;
			if(th > 0) return 1;
			i+=2;
		}
	}
	return 0;
}

int haveDiffernecesH(int dest,int src)
{
	if(src>=getPclen()) src = getPclen()-1;
	if(src<0) src = 0;
	if(dest<0) dest = 0;
	
	if(dest>=getPclen()) dest = getPclen()-1;
	char *content = getPcontent();

	if(dest < src)
	{
		int tmp = src;
		src = dest;
		dest = tmp;
	}
	if((dest - src) <= thresholdForDifferneces) return 0;
	for(int i=src;i<dest;i++)
	{
		// no ascii code
		//if(!(content[i]>='a'&&content[i]<='z')||(content[i]>='A'&&content[i]<='Z')) th++;
		if(content[i] != ' ' && content[i] != '\r' && content[i] != '\n' 
			&& content[i] != '\t') return 1;
	}
	return 0;
}

int haveDiffernecesE(int dest,int src) // extend
{
	if(src<0) src = 0;
	if(dest<0) dest = 0;
	if(src>=getPclen()) src = getPclen()-1;
	if(dest>=getPclen()) dest = getPclen()-1;

	if(EXINABSDIFF(dest,src)) return 0;

	int th = 0;
	char *content = getPcontent();

	if(dest < src)
	{
		int tmp = src;
		src = dest;
		dest = tmp;
	}
	for(int i=src;i<dest-1;i++)
	{
		/*
		// no ascii code
		//if(!(content[i]>='a'&&content[i]<='z')||(content[i]>='A'&&content[i]<='Z')) th++;
		if(content[i] != ' ' && content[i] != '\r' && content[i] != '\n' &&fitPattern()) th++;
		
		if(th>2) return 1;
		*/
		
		if(fitPattern('d',content[i])&&fitPattern('d',content[i+1])) 
		{
			//return 1;
			th++;
			if(th > 3) return 1;
			i+=2;
		}
	}
	return 0;
}
/*
int haveDiffernecesH(int dest,int src)
{
	if(src>=getPclen()) src = getPclen()-1;
	if(src<0) src = 0;
	if(dest<0) dest = 0;
	
	if(dest>=getPclen()) dest = getPclen()-1;
	int th = 0;
	char *content = getPcontent();

	if(dest < src)
	{
		int tmp = src;
		src = dest;
		dest = tmp;
	}
	if((dest - src) <= thresholdForDifferneces) return 0;
	for(int i=src;i<dest;i++)
	{
		// no ascii code
		if(content[i] != ' ' && content[i] != '\r' && content[i] != '\n') return 1;
	}
	return 0;
}
*/


int allMarked(int a[],int len)
{
	int i;
	for(i=0;i<len;i++)
	{
		if(a[i]==0) return 0;
	}
	return 1;
}







int powerWrite(FILE *fp,int start,int value,int len)
{
	int i;
	value = value < 0 ? -value : value;
	for(i=0;value>0;i++)
	{
		value /= 2;
	}
	//printf("[pw:%d]",i);
	rankWrite(fp,start,i,len);
	return 1;
}

int powerWriteNoMore(FILE *fp,int start,int value,int len)
{
	int i;
	value = value < 0 ? -value : value;
	for(i=0;value>0;i++)
	{
		value /= 2;
	}
	//printf("[pw:%d]",i);
	//printf("write:%d",i);
	rankWriteNoMore(fp,start,i,len);
	return 1;
}

int rankWriteNoMore(FILE *fp,int start,int rank,int len)
{
	
	for(int i=0;i<len;i++)
	{
		fprintf(fp,"%d:%d ",start+i,rank == i+1 ? 1 : 0);
	}
	return 1;
}




















