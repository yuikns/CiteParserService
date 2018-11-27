#ifndef __PERSISTENCE_H__
#define __PERSISTENCE_H__


//sFeatureData{int t[LENOFT];};
#ifndef LENOFT
#define LENOFT 5
#endif //LENOFT

#ifndef ENDLEN
#define ENDLEN 28
#endif //ENDLEN
//
#ifndef ENDCALLBACKLEN //ENDCALLBACKLEN
#define ENDCALLBACKLEN 10
#endif // ENDCALLBACKLEN

#ifndef ENDCTNMAX
#define ENDCTNMAX 1024
#endif // ENDCTNMAX

#ifndef CALLBACK_LEN //for start?
#define CALLBACK_LEN 9
//#define CALLBACK_LEN 6
#endif // CALLBACK_LEN


typedef struct sFeatureData //start feature data
{
	int qid;
	//char fileName[1024];	
	int t[LENOFT];
	int positive;
	int offset;
	int adjacencyOffset[2]; //0: prev 1:next 
	int fid[CALLBACK_LEN][2];//feature info data fid[x][0] == beforeData fid[x][1] == totalData
	//fid means all collected key words before && total of the offset
	int seq[CALLBACK_LEN][4];
	//CALLBACK_LEN :functions 
	//4:0)all 1)prev&&next 2)words 3)seq of vari
	double density[CALLBACK_LEN][3][2];//6:method 3: all prev&next(kwd) prev&next(100*words)  2: prev next
	double vari[2];
} featureData;

typedef struct
{
	int top;
	featureData data[1024]; // now max is 266?
} featureDataContainer;

//feature of endness
typedef struct sEndFeatureData //start feature data
{
	int qid;
	//char fileName[1024];	
	int t[ENDLEN];
	int positive;
	int offset;
	int adjacencyOffset[2]; //0: prev 1:next 
	int fid[ENDCALLBACKLEN][2];//feature info data fid[x][0] == beforeData fid[x][1] == totalData
	//fid means all collected key words before && total of the offset
	int seq[ENDCALLBACKLEN][4];
	//CALLBACK_LEN :functions 
	//4:0)all 1)prev&&next 2)words 3)seq of vari
	double density[ENDCALLBACKLEN][3][2];//6:method 3: all prev&next(kwd) prev&next(100*words)  2: prev next
	double vari[2];
} endFeatureData;

typedef struct
{
	int top;
	endFeatureData data[ENDCTNMAX]; // now max is ENDCTNMAX ~ 1024
} endFeatureDataContainer;

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

//init
int dbInit(void);
int doCreateTable(void);


//int doGetFeatureData(void *a_param, int argc, char **argv, char **column);
//int doGetFeatureDataInfo(void *a_param, int argc, char **argv, char **column);

int insertFeatureInfo(const char *fileName,int num);
int insertFeature(const char* fileName,featureData data);
int cleanFeature(const char* fileName);
int getFeature(const char *fileName,featureDataContainer *data);

char *queryEscape(const char *str);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __PERSISTENCE_H__

