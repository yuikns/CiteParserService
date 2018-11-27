#ifndef __MINIMUM_EDIT_DISTANCE_H__
#define __MINIMUM_EDIT_DISTANCE_H__

#ifndef __cplusplus
typedef int bool;
enum true_or_false{false,true};
#endif //__cplusplus


#ifndef INSERT_COST
#define INSERT_COST 1
#endif // INSERT_COST

#ifndef DELETE_COST
#define DELETE_COST 1
#endif // DELETE_COST

#ifndef SUBSTITUTE_COST 
#define SUBSTITUTE_COST 1
#endif // SUBSTITUTE_COST

#ifndef _MIN
#define _MIN(x,y,z) ((x<y)?((x<z)?x:z):((y<z)?y:z))
#endif // _MIN(x,y,z)

#ifndef _MAX
#define _MAX(x,y,z) ((x>y)?((x>z)?))
#endif // _MAX

#ifndef MIN2
#define MIN2(x,y) ((x<y)?(x):(y))
#endif //


#ifndef MAX2
#define MAX2(x,y) ((x>y)?(x):(y))
#endif //

#ifndef MIDDLEOF
#define MIDDLEOF(a,b,c)  (a >= b && a <= c) // a is at the range of  [b,c]
#endif 

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus


//inline int editDistance(const char *t,const char * s);
//inline int editDistanceS(const char *t,const char * s); // not sinsitive
int editDistance(const char *t,int tlen,const char * s,int slen);
int editDistanceS(const char *t,int tlen,const char * s,int slen);

/**
 * author fang
 */
int editDistanceT(const char *t,int tlen,const char * s,int slen,int threshold);

/**
 * pattern 
 * 
 * "aaaeaaa" equals "abc-def" 
 */
int editDistanceP(const char *t,int tlen,const char * s,int slen);


//inline
int isSame(char a,char b);

// is ascii or num
int isData(char ch);

/**
 * a is pattern
 * b is data
 *
 * about a,
 * : a Ascii
 * : n Number
 * : d Data(ascii or number)
 * : e dElimiter
 * : s Space
 */
int fitPattern(char a,char b);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __MINIMUM_EDIT_DISTANCE_H__

