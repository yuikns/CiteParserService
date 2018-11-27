#ifndef __CRFT_FUN_H__
#define __CRFT_FUN_H__
#include "queue.h"
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

int digitlen(const char *str,int len);

int puredigit(const char *str,int len);

int valofdigit(const char *str,int len);

int yearlike(const char *str,int len);

int strfeature(const char *str,int len);

int monthlike(const char *str,int len);

int vollkwd(const char *str,int len);

int edsFlag(const char *str,int len);

int pagekwd(const char *str,int len);

int specialFlag(const char *str,int len);

int procFlag(const char *str,int len);

int deptFlag(const char *str,int len);

int uniFlag(const char *str,int len);
int ltdFlag(const char *str,int len);

int domainFlag(const char *str,int len);

int isArticle(const char *str,int len);

int namelike(const char *str,int len,char next,int type);

int conferencelike(const char *str,int len);


// enqueue and dequeue
int filteredTokenId(int offset);
int ftEnQueue(pCNSQ Q,int *currentOffset,char *mpredeli);
pCrfNodeSnapshot ftDeQueue(pCNSQ Q);


int twovalue(const char *str,int len,int *a,int *b);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif// __CRFT_FUN_H__
