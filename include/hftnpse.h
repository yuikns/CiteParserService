#ifndef __HFTNPSE_H__
#define __HFTNPSE_H__
//head feature name, pages,sequences,and etc.

typedef struct
{
	int pLen;
	int nLen;
	int pStart;
	int prev[10];
	int next[10];
}CloseKWD;

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

//for debug
void setContent(const char * content);
void defineStartAndEnd(int *offset,int *offend,int limit);

//
int ch2int(char ch);
int isPageNumber(const char *content,int limit);

int hasPPafterTheOffset(int offset,int limit);
int hasPPafterTheOffset2(int offset,int limit);

int hasYearafterTheOffset(int offset,int limit);

int hasNameafterTheOffset0(int offset,int limit);
int hasNameafterTheOffset1(int offset,int limit);
int hasNameafterTheOffset2(int offset,int limit);

int hasSeqOfTheOffset(int offset,int limit);
int hasSeqOfTheOffset2(int offset,int limit);

int hasSpecialKeyWords(int offset,int limit);
int hasSpecialKeyWordsN(int offset,int limit);
int hasMonth(int offset,int limit);

int hasLocationafterTheOffset(int offset,int limit);

int hasWords(int offset,int limit);

int wordsNumber(int offset,int limit);

double wordsDensity(int offset,int limit);
double effectiveWordsDensity(int offset,int limit);

double asciiCodeDensity(int offset,int limit);
double dataDensity(int offset,int limit);

int keysNumber(int offset,int limit,int (*keyFind)(int,int));

typedef int (*CKWDfun)(int offset,int limit);

int getCloseKWD(int offset,CloseKWD *closeKWD,int (*callback)(int offset,int limit));

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //__HFTNPSE_H__
