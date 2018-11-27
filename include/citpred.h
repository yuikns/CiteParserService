#ifndef __CIT_PREDICT_H__
#define __CIT_PREDICT_H__


typedef struct _CitationNode
{
	char author[10][50];
	char title[150];
	char editor[10][50];
	char journal[150];
	char booktitle[150];
	char publisher[150];
	char institution[150];
	char volume[50];
	char location[50];
	char pages[50];
	char date[50];
	char url[50];
	char isbn[50];
	char tech[50];
	char other[50];
	struct _CitationNode * next;
}CitationNode;

typedef struct _OffsetCp
{
	int startOffset;
	int endOffset;
}OffsetCp;

typedef CitationNode * pCitationNode;
typedef OffsetCp * pOffsetCp;


#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

int initCitationNode(pCitationNode node);
pCitationNode addCitationNode(pCitationNode *node);
void freeCitationNode(pCitationNode *node);
pCitationNode addCitationInfo(pCitationNode *node,const char *str,int len,int id);
//int predftEnQueue(pCNSQ Q,int *currentOffset,char *mpredeli,int refAreaEnd);
pCitationNode CitationInfoPredict(int startOffset,int endOffset);
pCitationNode CitationInfoPredictFile(const char *fileName,int startOffset,int endOffset);
pCitationNode CitationInfoPredictString(const char *str,int startOffset,int endOffset);


void initCitationInfoPredict();
void cleanCitationInfoPredict();
#ifdef __cplusplus
}
#endif // __cplusplus


#endif // __CIT_PREDICT_H__
