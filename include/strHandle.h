#ifndef __STR_HANDLE_H__
#define __STR_HANDLE_H__

#ifdef WIN32
#include <string.h>
#endif //WIN32

#define isDelimiter(a) (a==' '||a=='\"' || a == '\'' || a=='`' || a==',' || a=='.' \
			|| a=='\n' || a=='\t' || a=='\r' || a=='|' || a=='(' || a==')'\
			|| a=='<' || a=='>' || a=='[' || a==']' || a=='{' || a=='}' \
			|| a=='/' || a=='$' || a=='#' || a=='^' || a=='&' || a=='*'||a=='@'\
			|| a=='!' || a=='?' || a==';' || a==':' || a=='|' || a=='\\' || a=='_'\
			|| a=='+')
			
#define isBlank(a) (a==' '|| a=='\n' || a=='\t' || a=='\r')

#define isAsciiOrDigit(a) ((a>='A' && a<='Z') || (a>='a' && a<='z') || (a>='0' && a<='9'))

#define isAsciiCode(a) ((a>='A' && a<='Z') || (a>='a' && a<='z'))

#define isUppercaseCode(a) (a>='A' && a<='Z')
#define isLowercaseCode(a) (a>='a' && a<='z')

#define isConnectCh(a) (a == '~' || a == '-')

#define DIGITLIKE(x) ((x<='9'&&x>='0')||x=='l'||x=='I'||x=='i'||x=='o'||x=='O')

#define VALDIG(x) ((x<='9'&&x>='0')?(x-'0'):((x=='I'||x=='i'||x=='l'||x=='['||x==']')?1:((x=='o'||x=='O')?0:-1)))

#ifdef WIN32
#define strcasecmp(s1,s2) stricmp(s1,s2)
#define strncasecmp(s1,s2,len) strnicmp(s1,s2,len)
#endif


#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

//int spilitContent(char *dest,int dlen,const char *src,int len,char *predeli,char *nextdeli);
int spilitContent(char *dest,int dlen,const char *src,int len,char *predeli,char *nextdeli,char *np);

// 
int spilitStr(const char *str,int slen,int *flg,int *flen);

//type :1 low case  :0 big
char* strCaseTransfer(char *s, int type);


#ifdef __cplusplus
}
#endif // __cplusplus


#endif // __STR_HANDLE_H__

