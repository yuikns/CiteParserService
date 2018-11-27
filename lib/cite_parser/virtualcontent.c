/**
 * The MIT License (MIT)
 *
 *
 * Copyright (C) 2013 Yu Jing (yujing5b5d@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute,sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED,INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "virtualcontent.h"
#include "tokens.h"

#define MAX_TOKEN_LENGTH 20

const char *test_file="data/Database.txt";

static char *content;
static int clen; //content's length

static int pclen; // pure content's length
static char *pcontent; // pure content
static unsigned int *offset; // offset is for pcontent 
static unsigned int *tags; // record the tag

static int contentType ;  // 0:inited and not setted content  1:getFrom setted data 2:getFrom file
static int noparse;

char *getContent()
{
	return contentType==0?NULL:content;
}

unsigned int *getOffset(){ return offset; }
unsigned int *getTags(){ return tags; }
int getClen() { return clen;}
int getPclen() { return pclen; }

char *getPcontent()
{ 
	return contentType==0?NULL:pcontent;
}


int readFileToParse(const char * fileName)
{
	unsigned int flen;
	FILE *fp = fopen(fileName,"r");
	if(fp == NULL)
	{
		fprintf(stderr,"open file %s error\n",fileName);
		return 0;
	}
	// get length 
	fseek(fp,0L,SEEK_END); 
	flen=ftell(fp); 
	fseek(fp,0L,SEEK_SET);
	// end : get length 
	
	//init space 
	clen = flen;
	content = (char *)calloc(flen+1,sizeof(char)); // +1 for '\0'
	memset(content,0,flen);
	
	if(!noparse)
	{
		pcontent = (char *)calloc(flen+1,sizeof(char));
		offset = (unsigned int *)calloc(flen,sizeof(unsigned int));
		tags = (unsigned int *)calloc(flen,sizeof(unsigned int));
		memset(pcontent,0,flen);
		memset(offset,0,flen*sizeof(unsigned int));
		memset(tags,0,flen*sizeof(unsigned int));
	}else
	{
		pcontent = content;
		pclen = clen;
	}
	

	fread(content,flen,1,fp);//read content
	
	fclose(fp);
	return 1;
}

int setFileContent(const char *str)
{
	if(str == NULL ) return 0;
	tokenInit();
	if(contentType == 1)
	{
		clen = 0;
		pclen = 0;
		if(pcontent != NULL) free(pcontent);
		pcontent = content = NULL;	
	}

	pclen = clen = strlen(str);
	pcontent = (char *)calloc(pclen+1,sizeof(char));
	
	memcpy(pcontent,str,pclen);
	content = pcontent;
	contentType = 1;
	return 1;
}

int setNoParse(int np)
{
	noparse = np;
	return 1;
}

inline int getTokenId(char *tkname,unsigned int len)
{
	int i;
	for(i = 0 ; i < tkRepo.max ; i++)
	{
		if(tkRepo.ac[i])
		{
			if(len != strlen(tkRepo.tokens[i])) continue;
			if(strncmp(tkRepo.tokens[i],tkname,len) == 0)
			{
				return token2Id(tkRepo.tokens[i]);
			}
		}
	}
	return 0;
}

int listStack(unsigned int repo)
{
	return 0;
	int i;
	printf("in stack : ");
	while((i=tokenPop(&repo)) != 0)
	{
		printf("%10s\t",id2Token(i));
	}
	printf("\n");
	return 1;
}


int parseFile(const char * fileName)
{
	unsigned int flen;
	unsigned int i;
	unsigned int j = 0;
	unsigned int k = 0;
	unsigned int nowTag;
	int tmp;
	int tmp2;
	int fcontinue;
	tokenInit();
	if(contentType == 1)
	{
		clen = 0;
		pclen = 0;
		free(pcontent);
		pcontent = content = NULL;	
	}
	if(!readFileToParse(fileName)) return 0;
	contentType = 2;
	if(noparse == 1) return 1;
	flen = clen;

	
	
	nowTag = 0;
	for(i=0;i<flen;i++)
	{
		fcontinue = 0;
		if(content[i] == '<')
		{
			if(content[i+1] == '/')
			{
				for(j=0;j<MAX_TOKEN_LENGTH;j++)
				{
					if(content[i+j+2] == '>')
					{
						tmp = getTokenId(content+i+2,j);
						//for(k=0;k<j;k++) putchar(content[i+2+k]);
						//putchar('\t');
						if(tmp != 0)
						{
							listStack(nowTag);
							//printf("out : [%d][%s]\n",tmp,id2Token(tmp));
							tmp2 = tokenPop(&nowTag);
							//printf("POP\t[%d][%d?]\n",tmp2,tmp);

							if(tmp != tmp2)
							{
								//fprintf(stderr,"tmp,tmp2: [%d]:[%d]%s\n",tmp,tmp2,id2Token(tmp));
								fprintf(stderr,"~error: %s(%d) require : %d\n",id2Token(tmp),tmp,tmp2);
								putchar('\n');
								printf(". . . ");
								for(j=0;j<30;j++)
									putchar(content[i+j]);
								printf(". . . ");
								putchar('\n');
								tokenPush(&nowTag,tmp2);
								return 0;
								//break;
							}else
							{
								/*
								if(mark)
								{
									printf("\n\n\nout:%s\n\n",id2Token(tmp));
									getchar();
								}
								*/
								i+=j+2;
								//continue;
								fcontinue = 1;
								break;
							}
						}
					}
				}
				if(fcontinue) continue;
			}else
			{
				for(j=0;j<MAX_TOKEN_LENGTH;j++)
				{
					if(content[i+j+1] == '>')
					{		
						tmp = getTokenId(content+i+1,j);
						//for(k=0;k<j;k++) putchar(content[i+1+k]);
						//putchar('\t');
						if(tmp != 0)
						{
							/*
							int zz;
							if(mark)
							{
							printf("\n\nCDATA[[");
							for(zz=i;zz<i+j+1;zz++) putchar(content[zz]);
							printf("]]\n\n\nin:%s\n\n",id2Token(tmp));
							
							}
							*/
							//printf("PUSH[%d]\n",tmp);
							//printf("in : [%d][%s]\n",tmp,id2Token(tmp));
							tokenPush(&nowTag,tmp);
							listStack(nowTag);
							i+=j+1;
							//continue;
							fcontinue = 1;
							break;
						}
					}
				}
				if(fcontinue) continue;
			}
			
		}
		pcontent[k] = content[i];
		offset[k] = i;
		tags[k] = nowTag;
		//putchar(pcontent[k]);
		k++;
	}
	pclen = k+1;
	
	return 1;
}

int initContent()
{
	content = NULL;
	pcontent = NULL;
	offset = NULL;
	tags = NULL;
	clen = 0;
	pclen = 0;
	noparse = 0;
	contentType = 0;
	return 1;
}

int cleanContent()
{
	#define doClean(s) {if(s != NULL) { free(s);s=NULL;}}
	doClean(content);
	clen = 0;
	pclen = 0;
	noparse = 0;
	
	if(contentType == 2 && noparse == 0)
	{
		doClean(pcontent);
		doClean(offset);
		doClean(tags);
	}
	contentType = 0;
	return 1;
}


