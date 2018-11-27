#ifndef __CRF_CTL_QUEUE_H__
#define __CRF_CTL_QUEUE_H__

#define CNSQSIZE 13

typedef struct
{
	int offset;
	int slen;
	char str[1024];
	char mpredeli;
	char nextdeli;
	char predeli;
	int digitl;
	int puredigit;
	int dval;
	int strtype;
	int yearlike;
	int monthlike;
	int volumnlike;
	int pagelike;
	int edsflag;
	int speflag;
	int procflag;
	int namelike;
	int isNameDict;
	int rLastNameDict;
	int isCountryDict;
	int isFunWordDict;
	int isPlaceNameDict;
	int isPubliserDict;
	int isArticle;
	int deptflag;
	int uniflag;
	int ltdflag;
	int token;
	int quotflag;//Quotation marks
	int pareSflag;//Parentheses
	int pareEflag;
	int sqbSflag;//Square brackets
	int sqbEflag;
	int braSflag;//Braces
	int braEflag;
	int stopflag; //  :0 A. Stpt. plain  :1 ','   :2'.''!''?'  
	int domainflag;
	int imprnum;// [123][456] >0 , <0 , ==0
}CrfNodeSnapshot,*pCrfNodeSnapshot;


typedef struct{
    CrfNodeSnapshot data[CNSQSIZE];  //队列的存储空间
    int front,rear;          //队头，队尾指针
}CNSQ,*pCNSQ; // CrfNodeSnapshotQueue

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

void clearQueue(pCNSQ Q);


int isEmptyQueue(pCNSQ Q);

int isFullQueue(pCNSQ Q);

int enQueue(pCNSQ Q,CrfNodeSnapshot e);

int enQueueWithDrop(pCNSQ Q,CrfNodeSnapshot e);

pCrfNodeSnapshot deQueue(pCNSQ Q);

pCrfNodeSnapshot nextNElem(pCNSQ Q,int N);

pCrfNodeSnapshot pastNElem(pCNSQ Q,int N);

int sizeQueue(pCNSQ Q);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // 
