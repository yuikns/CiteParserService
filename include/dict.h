#ifndef __DICT_H__
#define __DICT_H__
//init
#include "khash.h"
KHASH_MAP_INIT_STR(dict,double)

 #ifdef __cplusplus
extern "C"
{
#endif // __cplusplus



// DB part
int dbConnect(void);
void dbFree(void);
int dbLock(void);
int dbUnlock(void);

int dbCreateNameDict(const char *name);
int dbCreateValDict(const char *name);
int dbCreateTable(void);

int dbAddNameDict(const char *dict,const char *name);
int dbAddValDict(const char *dict,const char *name,double val);


//int dbHandleGetNameDict(void *ch, int argc, char **argv, char **column);
//int dbHandleGetValDict(void *ch, int argc, char **argv, char **column);


int dbGetNameDict(const char *dict,const char *name);
int dbGetNameLikeDict(const char *dict,const char *name);
double dbGetValDict(const char *dict,const char *name);
double dbGetValLikeDict(const char *dict,const char *name);


int dbHandleHashDictLoad(void *d, int argc, char **argv, char **column);
int dbHashDictLoad(const char *dict,int mid);

// HASH part

void hmDictLoad(void);
void hmDictFree(void);
double hmDictGet(int did,const char *str);

//dict
int isNameInDict(const char *str);
int rateLastNameInDict(const char *str);
int isCountryInDict(const char *str);
int isFunWordInDict(const char *str);
int isPlaceNameInDict(const char *str);
int isPublisherInDict(const char *str);
int isMonthInDict(const char *str);

void freeDict();

#ifdef __cplusplus
}
#endif // __cpluseplus

#endif //  __DICT_H__
