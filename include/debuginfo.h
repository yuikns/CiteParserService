#ifndef __DEBUG_INFO_H__
#define __DEBUG_INFO_H__

#ifndef DEBUGFLAG
#define DEBUGFLAG 1
#endif //DEBUGFLAG


#ifndef SWAP
#define SWAP(a,b) {a=a^b;b=a^b;a=a^b;}
#endif //SWAP

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
// for debug
int printfContext(int offset);
int printfContextS(int offset,const char *str);
int printfContextF(int offset,const char *str,FILE *fp);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __DEBUG_INFO_H__
