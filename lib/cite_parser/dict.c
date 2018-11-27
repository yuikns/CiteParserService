#include "dict.h"
#include "sqlite3.h"
#include "khash.h"
#include "strHandle.h"
#include <stdio.h>
#include <stdlib.h>



typedef struct {
	int type;//0 only name ,1 name and value
	//khash_t(dict) *h;
	int mid;
}DictVal;

const char *dictDB = "db/dict.db";
typedef sqlite3 * dbsrc;
dbsrc msrc;
khash_t(dict) *map[10];

khash_t(dict) *mymap;

int dbCreateNameDict(const char *name)
{
	//id integer  primary key,
	char query[300];
	sprintf(query,"create table %s(id integer primary key,name varchar(100) unique)",name);
	char *emsg;
	//printf("%s\n",query);
	sqlite3_exec(msrc,query,0,0,&emsg);
	if(emsg != NULL) fprintf(stderr,"E: %s",emsg);
	return 0;
}

int dbCreateValDict(const char *name)
{
	//id integer  primary key,
	char query[300];
	sprintf(query,"create table %s(id integer primary key,name varchar(100) unique,val double)",name);
	char *emsg;
	//printf("%s\n",query);
	sqlite3_exec(msrc,query,0,0,&emsg);
	if(emsg != NULL) fprintf(stderr,"E: %s",emsg);
	return 0;
}

int dbCreateTable()
{
	dbCreateNameDict("ChineseFamilyNames");
	dbCreateNameDict("CountryNames");
	dbCreateNameDict("FemaleFirstNames");
	dbCreateNameDict("MaleFirstNames");
	dbCreateNameDict("FuncWord");
	dbCreateNameDict("Months");
	dbCreateNameDict("PlaceNames");
	dbCreateNameDict("PublisherNames");
	dbCreateValDict("LastNames");
	dbCreateValDict("CityNames");
	return 0;
}


int dbConnect()
{
	// open db
	if(sqlite3_open(dictDB,&msrc) != 0) 
		fprintf(stderr,"E:db connect refuese (%d)\n",sqlite3_errcode(msrc));	
	return 1;
}

int dbLock()
{
	//return sqlite3_exec(msrc,"BEGIN TRANSACTION",0,0,0);
	return sqlite3_exec(msrc,"BEGIN",0,0,0);
}

int dbUnlock()
{
	return sqlite3_exec(msrc,"COMMIT",0,0,0);
}


int dbAddNameDict(const char *dict,const char *name)
{
	char query[1024];
	sprintf(query,"insert into %s(name) values(\'%s\')",dict,sqlite3_mprintf("%q",name));
	return sqlite3_exec(msrc,query,0,0,0);
}

int dbAddValDict(const char *dict,const char *name,double val)
{
	char query[1024];
	sprintf(query,"insert into %s(name,val) values(\'%s\',%f)",dict,sqlite3_mprintf("%q",name),val);
	return sqlite3_exec(msrc,query,0,0,0);
}


int dbHandleGetNameDict(void *ch, int argc, char **argv, char **column)
{
	int *ich = (int *)ch;
	(*ich) = 1;
	//printf("## ch is :%d|%d|%s\n",*ich,atoi(argv[0]),column[0]);
	return 0;
}


int dbHandleGetValDict(void *ch, int argc, char **argv, char **column)
{
	double *vch = (double *)ch;
	(*vch) = atof(argv[0]);
	//printf("##ch is :%f\n",*((double *)ch));
	return 0;
}


int dbGetNameDict(const char *dict,const char *name)
{
	int ch = 0;
	char query[1024];
	char *e;
	sprintf(query,"select * from %s where name = \'%s\'",dict,sqlite3_mprintf("%q",name));
	//printf("query:[%s]\n",query);
	sqlite3_exec(msrc,query,dbHandleGetNameDict,&ch,&e);
	if(e != NULL)
		fprintf(stderr,"E:%d:%s\n%s::%d!\n",sqlite3_errcode(msrc),e,__FILE__,__LINE__);
	return ch;
}

int dbGetNameLikeDict(const char *dict,const char *name)
{
	int ch = 0;
	char query[1024];
	char *e;
	sprintf(query,"select * from %s where name like \'%s\'",dict,sqlite3_mprintf("%q",name));
	//printf("query:[%s]\n",query);
	sqlite3_exec(msrc,query,dbHandleGetNameDict,&ch,&e);
	if(e != NULL)
		fprintf(stderr,"E:%d:%s\n%s::%d!\n",sqlite3_errcode(msrc),e,__FILE__,__LINE__);
	return ch;
}


double dbGetValDict(const char *dict,const char *name)
{
	double val = 0;
	char query[1024];
	char *e;
	sprintf(query,"select val from %s where name = \'%s\'",dict,sqlite3_mprintf("%q",name));
	//printf("query:[%s]\n",query);
	sqlite3_exec(msrc,query,dbHandleGetValDict,&val,&e);
	if(e != NULL)
		fprintf(stderr,"E:%d:%s\n%s::%d!\n",sqlite3_errcode(msrc),e,__FILE__,__LINE__);
	return val;
}

double dbGetValLikeDict(const char *dict,const char *name)
{
	double val = 0;
	char query[1024];
	char *e;
	sprintf(query,"select val from %s where name like \'%s\'",dict,sqlite3_mprintf("%q",name));
	//printf("query:[%s]\n",query);
	sqlite3_exec(msrc,query,dbHandleGetValDict,&val,&e);
	if(e != NULL)
		fprintf(stderr,"E:%d:%s\n%s::%d!\n",sqlite3_errcode(msrc),e,__FILE__,__LINE__);
	return val;
}


void dbFree()
{
	sqlite3_close(msrc);
}


//////////////////////////////////////////////////////////////////////////////////////////////
// hash maps
/*
KHASH_MAP_INIT_STR(dd,double)


void testhash()
{
	int ret, is_missing;
	khiter_t k;
	char *final;
	
	
	khash_t(dd) *h = kh_init(dd);

	final = (char *)malloc(100);
	sprintf(final,"abc");
	k = kh_put(dd, h,final, &ret);
	if(!ret)  kh_del(dd, h, k);
	kh_value(h, k) = 1;
	
	
	final = (char *)malloc(100);
	sprintf(final,"def");	
	k = kh_put(dd, h,final, &ret);
	if(!ret)  kh_del(dd, h, k);
	kh_value(h, k) = 2;
	
	final = (char *)malloc(100);
	sprintf(final,"hij");
	k = kh_put(dd, h,final, &ret);
	if(!ret)  kh_del(dd, h, k);
	kh_value(h, k) = 3;

	final = (char *)malloc(100);
	sprintf(final,"zz");
	k = kh_put(dd, h,final, &ret);
	if (!ret) kh_del(dd, h, k);
	kh_value(h, k) = 10;
	
	k = kh_get(dd, h, "def");
	is_missing = (k == kh_end(h));
	printf("ismiss?%d\n",is_missing);
	//k = kh_get(dict, h, 5);
	//kh_del(dict, h, k);
	for (k = kh_begin(h); k != kh_end(h); k++)
		if (kh_exist(h, k))
		{ 
			printf("key : %s value : %f\n",kh_key(h,k),kh_value(h, k));
		}
	
	k = kh_get(dd, h, "abc");
	is_missing = (k == kh_end(h));
	printf("abc ismiss?%d\n",is_missing);
	
	k = kh_get(dd, h, "uvw");
	is_missing = (k == kh_end(h));
	printf("uvw ismiss?%d\n",is_missing);
	
	kh_destroy(dd, h);
}
*/

double val = 1;

int dbHandleHashDictLoad(void *d, int argc, char **argv, char **column)
{
	DictVal *dv = (DictVal *)d;
	int rt;
	khiter_t k;
	char *name = (char *)malloc(100);
	sprintf(name,"%s",argv[1]);
	strCaseTransfer(name,1);
	//printf("##ch is :%f\n",*((double *)ch));
	
		k = kh_put(dict, map[dv->mid],name, &rt);
		if (!rt) kh_del(dict,map[dv->mid], k);
	
		if(dv->type == 1) // 1: name 2:val
		{
			kh_value(map[dv->mid], k) = atof(argv[2]);
			//if(atof(argv[2]) != 0)
			//	printf("val:%f\n",atof(argv[2]));
		}else // 1: name
			kh_value(map[dv->mid], k) = 1;

	return 0;
}


int dbHashDictLoad(const char *dict,int mid)
{
	DictVal dv;
	int ch = 0;
	char query[1024];
	char *e;
	//dv.h = map[mid];
	dv.mid = mid;
	dv.type = mid == 9 ? 1 : 0;
	sprintf(query,"select * from %s ",dict);
	//printf("query:[%s]\n",query);
	sqlite3_exec(msrc,query,dbHandleHashDictLoad,&dv,&e);
	if(e != NULL)
		fprintf(stderr,"E:%d\n%s::%d!\n",sqlite3_errcode(msrc),__FILE__,__LINE__);
		//,e
	return ch;
}


void hmDictLoad()
{
	int i;
	char dict[][50] = {"ChineseFamilyNames","CountryNames","FemaleFirstNames",
			"FuncWord","MaleFirstNames","Months","PlaceNames","PublisherNames","CityNames"};
	char vdict[50] = "LastNames";
	
	
	for(i=0;i<9;i++)
	{
		map[i] = kh_init(dict);
		dbHashDictLoad(dict[i],i);
	}
	map[9] = kh_init(dict);
	dbHashDictLoad(vdict,9);
	
	//unsigned k;
	//for (k = kh_begin(map[8]); k != kh_end(map[8]); k++)
	//	if (kh_exist(map[8], k))
	//	{ 
	//		printf("[%s]\t\t=>\t[%f]\n",kh_key(map[8],k),kh_value(map[8], k));
	//	}
}

void hmDictFree()
{
	int i;
	for(i=0;i<10;i++)
	{
		kh_destroy(dict, map[i]);
	}
}


double hmDictGet(int did,const char *str)
{
	khiter_t k;
	int len = strlen(str);
	char data[100];
	sprintf(data,"%s",str);
	int miss;
	k = kh_get(dict,map[did], str);
	miss = (k == kh_end(map[did]));
	if(!miss)  return kh_value(map[did], k);
	
	len--;
	while(len>3)
	{
		data[len] = '\0';
		k = kh_get(dict,map[did], data);
		if(k != kh_end(map[did]))  
			return kh_value(map[did], k);
		len --;
	}
	return 0;
}

//k = kh_get(dict, h, "def");
//is_missing = (k == kh_end(h));
//kh_value(h, k) = 10;
//////////////////////////////////////////////////////////////////////////////////////////////


/**
 * function for db export
 *
 */
 
/**
 * 0: "ChineseFamilyNames"
 * 1: "CountryNames"
 * 2: "FemaleFirstNames"
 * 3: "FuncWord"
 * 4: "MaleFirstNames"
 * 5: "Months"
 * 6: "PlaceNames"
 * 7: "PublisherNames"
 * 8: "CityNames"
 *
 * 9: "LastNames"
 */

int isNameInDict(const char *str)
{
	char ncstr[1024];
	sprintf(ncstr,"%s",str);
	strCaseTransfer(ncstr,1);
	return hmDictGet(4,ncstr) != 0 || hmDictGet(2,ncstr) != 0 || hmDictGet(0,ncstr) != 0;
}

int rateLastNameInDict(const char *str)
{
	char ncstr[1024];
	sprintf(ncstr,"%s",str);
	strCaseTransfer(ncstr,1);
	double r = hmDictGet(9,ncstr);
	double i;
	if(r==0) return 0;
	for(i=0;i<5;i++)
	{
		if((r>(i*0.2))&&(r<=((i+1)*0.2)))
		{
			return i+1;
		}
	}
	return 0;	
}

int isCountryInDict(const char *str)
{
	char ncstr[1024];
	sprintf(ncstr,"%s",str);
	strCaseTransfer(ncstr,1);
	return hmDictGet(1,ncstr)!= 0;
}

int isFunWordInDict(const char *str)
{
	char ncstr[1024];
	sprintf(ncstr,"%s",str);
	strCaseTransfer(ncstr,1);
	return hmDictGet(3,ncstr) != 0;
}

int isPlaceNameInDict(const char *str)
{
	char ncstr[1024];
	sprintf(ncstr,"%s",str);
	strCaseTransfer(ncstr,1);
	return hmDictGet(6,ncstr)!= 0  ||  hmDictGet(8,ncstr)!= 0 ;
}

int isPublisherInDict(const char *str)
{
	char ncstr[1024];
	sprintf(ncstr,"%s",str);
	strCaseTransfer(ncstr,1);
	return hmDictGet(7,ncstr)!= 0;
}

// no need in use
int isMonthInDict(const char *str)
{
	char ncstr[1024];
	sprintf(ncstr,"%s",str);
	strCaseTransfer(ncstr,1);
	return hmDictGet(5,ncstr)!= 0;
}







