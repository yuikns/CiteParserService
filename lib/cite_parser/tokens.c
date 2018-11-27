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
#include "tokens.h"
#include "ini.h"
#include <string.h>
#include <stdlib.h>


static int doTokenInit(void* user, const char* section, const char* name,const char* value)
{
	struct tokens* pRepo = (struct tokens* )user;
	int id = atoi(value);
	#define MSEC(s) strcmp(section, s) == 0
	if(MSEC("tokens"))
	{
		if(id > TOKEN_MAX_LEN -1) return 0;
		sprintf(pRepo->tokens[id],"%s",name);
		//TODO Make sure strdup is multi-platformed
		pRepo->ac[id] = 1;
	}
	return 1;
}

int tokenInit()
{
	int i;
	tkRepo.max = TOKEN_MAX_LEN;
	for(i=0;i<TOKEN_MAX_LEN;i++) tkRepo.ac[i] = 0;
	if (ini_parse("token.ini", doTokenInit, &tkRepo) < 0) {
		fprintf(stderr,"Can't load 'token.ini'\n");
		return 0;
	}
	/**
	for(i = 0 ; i < tkRepo.max ; i++)
	{
		if(tkRepo.ac[i]) printf("%s=%d\n",tkRepo.tokens[i],i);
	}
	*/
	
	return 1;
}

inline char *id2Token(int id)
{
	return tkRepo.ac[id] ?  tkRepo.tokens[id] : NULL;
}

int token2Id(const char *token)
{
	int i;
	for(i=0;i<TOKEN_MAX_LEN;i++)
		if(strcmp(tkRepo.tokens[i],token) == 0)
			return i;
	return 0;
}


//static int nowToken;

int tokenPush(unsigned int *tp,unsigned int id)
{
	unsigned int max_uint = 0-1;
	if(id == 0 || id > TOKEN_MAX_LEN) return -2;
	if((*tp )>(max_uint / TOKEN_MAX_LEN)) return -1;
	//printf("do push: %d %d ->|",*tp,id);
	*tp = *tp * TOKEN_MAX_LEN;
	*tp = *tp + id;
	//printf("%d\n",*tp);
	return 1;
}

unsigned int tokenPop(unsigned int *tp)
{
	unsigned int id = *tp % (TOKEN_MAX_LEN) ;
	//printf("do pop : %d ~~~",*tp);
	*tp /= TOKEN_MAX_LEN;
	//printf("%d %d\n",*tp,id);
	return id;
}



