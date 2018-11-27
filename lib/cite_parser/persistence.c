#include "persistence.h"
#include "sqlite3.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>

#define DEBUGING 0

const char *dbfile = "db/features.db";
typedef sqlite3 * dbsrc;
dbsrc featureSrc = NULL;
/**
 * atof(将字符串转换成浮点型数)
 * atoi(将字符串转换成整型数)
 * atol(将字符串转换成长整型数)
 * strtod(将字符串转换成浮点数)
 * strtol(将字符串转换成长整型数)
 * strtoul(将字符串转换成无符号长整型数)
 * toascii(将整型数转换成合法的ASCII 码字符)
 * toupper(将小写字母转换成大写字母)
 * tolower(将大写字母转换成小写字母)
 */



inline char *queryEscape(const char *str)
{
	return sqlite3_mprintf("%q",str);
}


int doCreateTable()
{
	const char *createTableQuery ="create table OneToFive(\
			id integer primary key ,\
			qid int ,\
			fileName varchar(512),\
			t0 int,\
			t1 int,\
			t2 int,\
			t3 int,\
			t4 int,\
			positive int,\
			offset long)";
	const char *createTableQuery2 = "create talbe EndOffset(\
			id integer primary key,	\
			qid int ,\
			fileName varchar(512),\
			positive int,\
			offset long)";
	sqlite3_exec(featureSrc,
			"create table OneToFiveInfo(\
				id integer primary key,\
				fileName varchar(512),\
				sampleNumber int\
				)",0,0,0);
	sqlite3_exec(featureSrc,createTableQuery,0,0,0);
	sqlite3_exec(featureSrc,createTableQuery2,0,0,0);
	return 0;
}

int dbInit()
{
	////id integer PRIMARY KEY autoincrement,
	char *err;
	const char *judgeQuery ="select * from table OneToFive";
	// open db
	if(sqlite3_open(dbfile,&featureSrc) != 0) 
		fprintf(stderr,"[db][ERROR](%d):%s\n",sqlite3_errcode(featureSrc),err);	
	// create table if not exists
	if(sqlite3_exec(featureSrc,judgeQuery,0,0,&err) == 1) doCreateTable();
	
	

	return 1;
}


int insertFeature(const char* fileName,featureData data)
{
	char query[1024];
	char *err_msg = NULL;
	int status;
	sprintf(query,"insert into OneToFive(qid,fileName,t0,t1,t2,t3,t4,positive,offset) \
					values(%d,'%s',%d,%d,%d,%d,%d,%d,%d)",
		data.qid,
		queryEscape(fileName),
		data.t[0],
		data.t[1],
		data.t[2],
		data.t[3],
		data.t[4],
		data.positive,
		data.offset);
	
	//DEBUG	
	if(DEBUGING) printf("\n---[[query|%s]]---\n",query);
	
	status = sqlite3_exec(featureSrc,query,0,0,&err_msg);
	if(err_msg != NULL)
	{
		fprintf(stderr,"query>>%s\n\n[DB]<insertFeature>:%s --%d\n",query,err_msg,__LINE__);
		if(DEBUGING) getchar();
	}
	return status == 0;
}

int cleanFeature(const char* fileName)
{
	char query[1024];
	char *err_msg = NULL;
	int status;
	sprintf(query,"delete from OneToFive where fileName='%s'",queryEscape(fileName));
	
	//DEBUG	
	if(DEBUGING) printf("\n---[[query|%s]]---\n",query);
	
	status = sqlite3_exec(featureSrc,query,0,0,&err_msg);
	if(err_msg != NULL)
	{
		fprintf(stderr,"query>>%s\n\n[DB]<cleanFeature>:%s --%d\n",query,err_msg,__LINE__);
		if(DEBUGING) getchar();
	}
	return status == 0;
}


int insertFeatureInfo(const char *fileName,int num)
{
	char *err_msg = NULL;
	char query[1024];
	int status;
	sprintf(query,"insert or replace into OneToFiveInfo(fileName,sampleNumber) values('%s',%d)",
		queryEscape(fileName),num);
	
	//DEBUG	
	if(DEBUGING) printf("\n---[[query|%s]]---\n",query);
	
	status = sqlite3_exec(featureSrc,query,0,0,&err_msg);
	if(err_msg != NULL)
	{
		fprintf(stderr,"[DB]<insertFeatureInfo>:%s --%d\n",err_msg,__LINE__);
		if(DEBUGING) getchar();
	}
	return status == 0;
}

int doGetFeatureData(void *a_param, int argc, char **argv, char **column)
{
	//printf("++\n");
	int i;
	featureDataContainer *fdc = (featureDataContainer *)a_param;
	featureData *fd;
	fdc->top ++;
	fd = &(fdc->data[fdc->top-1]);
	//printf("ok1");
	fd->qid = atoi(argv[0]);
	//printf("ok2");
	for(i=0;i<5;i++) fd->t[i] = atoi(argv[i+1]);
	//printf("ok3");
	fd->positive = atoi(argv[6]);
	//printf("ok4");
	fd->offset = atoi(argv[7]);
	//printf("ok5");
	return 0;
}

int doGetFeatureDataInfo(void *a_param, int argc, char **argv, char **column)
{
	//printf("~~\n");
	int *num =(int *)a_param;
	//for(int i=0;i<argc;i++)
	//	printf("%s %s \t",argv[i],column[i]);
	//printf("\n");
	*num = atoi(argv[0]);
	return 0;
}

int getFeature(const char *fileName,featureDataContainer *myFeatureDataContainer)
{
	int num;
	char query[1024];
	char *err_msg;
	//int status;
	sprintf(query,"select sampleNumber from OneToFiveInfo where fileName = '%s'",
				queryEscape(fileName));
	
	
	//DEBUG	
	if(DEBUGING) printf("\n---[[query|%s]]---\n",query);
	
	sqlite3_exec(featureSrc,query,doGetFeatureDataInfo,&num,&err_msg);
	
	//printf("##%d##\n",sqlite3_exec(featureSrc,query,doGetFeatureDataInfo,&num,&err_msg));
	//printf("num is:%d\n",num);
	err_msg = NULL;
	if(err_msg != NULL)
	{
		fprintf(stderr,"[DB]<getFeature>:%s --%d\n",err_msg,__LINE__);
		if(DEBUGING) getchar();
	}
	
	myFeatureDataContainer->top = 0;
	sprintf(query,"select qid,t0,t1,t2,t3,t4,positive,offset \
			from OneToFive where fileName = '%s'",queryEscape(fileName));
			//		from OneToFive ");
	sqlite3_exec(featureSrc,query,doGetFeatureData,myFeatureDataContainer,&err_msg);
	if(err_msg != NULL)
	{
		fprintf(stderr,"[DB]<getFeature>(1):%s\n",err_msg);
		if(DEBUGING) getchar();
	}
	
	
	return num == (myFeatureDataContainer->top);
}


