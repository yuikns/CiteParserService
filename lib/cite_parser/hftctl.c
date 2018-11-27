#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "minEditDistance.h"
#include "virtualcontent.h"
#include "hftnpse.h"
#include "persistence.h"
#include "tokens.h"
#include "hftctl.h"
#include "myMath.h"

//for debug
#include "debuginfo.h"

_FilterData pFilterData;

FILE *fpTrain; // train.txt
FILE *fpTest; // test.txt

int fileNum = 0;
featureDataContainer mfdc; // myFeatureDataContainer
featureData mfd; // myFeatureData


OffsetCallback functionList[CALLBACK_LEN]={hasPPafterTheOffset,hasPPafterTheOffset2,
			hasYearafterTheOffset,hasNameafterTheOffset0,
			hasNameafterTheOffset1,hasNameafterTheOffset2,
			hasSeqOfTheOffset,hasSeqOfTheOffset2,
			hasSpecialKeyWords};
/*
OffsetCallback functionList[CALLBACK_LEN]={hasPPafterTheOffset,hasPPafterTheOffset2,
		hasYearafterTheOffset,hasNameafterTheOffset0,hasNameafterTheOffset1,hasSeqOfTheOffset};
*/

/**
 * Start FilterData Info Handle
 */
int initFilterData()
{
	pFilterData = NULL;
	return 1;	
}

int cleanFilterData()
{
	_FilterData head = pFilterData;
	_FilterData p;
	while(head != NULL)
	{
		p = head;
		head = head->next;
		free(p);
	}
	return 1;
}


/**
 * 	int accept ;  // acc =1 accept , acc = 0 , reject 
 *	char *cmpStr;
 *	int cmpLen;
 */
int insertFilterData(int accept,const char *cmpStr,int cmpLen)
{
	_FilterData p = pFilterData;
	
	if(pFilterData == NULL)
	{
		pFilterData = (_FilterData)malloc(sizeof(FilterData));
		pFilterData->acc = accept;
		pFilterData->cmpLen = cmpLen;
		//head->cmpStr = strdup(cmpStr);
		sprintf(pFilterData->cmpStr,"%s",cmpStr);
		pFilterData->next = NULL;
		//printf("head, done \n");
		fflush(NULL);
	}else
	{
		//printf("inserting ...");
		while(p->next != NULL){ p=p->next;}
		p->next = (_FilterData)malloc(sizeof(FilterData));
		p = p->next;
		p->acc =accept;
		p->cmpLen = cmpLen;
		sprintf(p->cmpStr,"%s",cmpStr);
		p->next = NULL;
		//printf("done\n");
	}
	return 1;
}

/**
 * Judge Is Accpeted
 */
int isAccpted(const char *str,int threshold,int *fitLen)
{
	_FilterData p = pFilterData;
	int accMin = threshold + 1;
	int rejMin = threshold + 1;
	int accMinOffset=0; 
	int rejMinOffset=0;
	int i;
	int tmpMin;
	int tmpMinOffset = 0;
	int minLen;
	int maxLen;
	int ed;
	while(p != NULL)
	{
		minLen = p->cmpLen - threshold;
		maxLen = p->cmpLen + threshold+1;
		tmpMin = threshold + 1;
		for(i=minLen;i<=maxLen;i++)
		{
			if((ed = editDistanceS(p->cmpStr,p->cmpLen,str,i)) <= threshold)
			{
				if(tmpMin > ed)
				{
					tmpMin = ed;
					tmpMinOffset = i;
				}
			}
		}
		
		if(p->acc == 1 && accMin > tmpMin)
		{
			accMinOffset = tmpMinOffset;
			accMin = tmpMin;
		}else if(p->acc == 0 && rejMin > tmpMin) 
		{
			rejMinOffset = tmpMinOffset;
			rejMin = tmpMin;
		}
		p = p-> next;
	}
	*fitLen = accMin <= rejMin ? 
			(accMin <= threshold ? accMinOffset : 0 ) :
			(rejMin <= threshold ? rejMinOffset : 0 ) ;
	return (accMin <= rejMin) && (accMin <= threshold );
}

int stackData(StackInfo *myStack,AcceptStr acceptStr,const char *content,int len,int threshold)
{
	int i;
	int fitLen;
	myStack->top = 0;
	int unlock = 1;
	for(i=0;i<len;i++)
	{
		if(isData(content[i]))
		{
			if(unlock)
			{
				if(acceptStr(content+i,threshold,&fitLen))
				{
					i += fitLen;
					if(myStack->top < MAX_STACK)
					{
						myStack->data[myStack->top] = i;
						myStack->top ++ ;
					}
				}else
				{
					i += fitLen;
				}
				unlock = 0;
			}
		}else
		{
			unlock = 1;
		}
	}
	return 1;
}


unsigned int getReferenceHeadOffset()
{
	int i;
	int mypclen = getPclen();
	unsigned int myTags;
	unsigned int nowTag;
	for(i=0;i<mypclen;i++)
	{
		myTags = *(getTags()+i);
		while((nowTag=tokenPop(&myTags)) > 0)
			if(nowTag == 1)
				return i;		
	}
	return 0;
}

inline int allZero(StackInfo info[],int len)
{
	int i;
	for(i=0;i<len;i++)
	{
		if(info[i].top >0 ) return 0;
	}
	return 1;
}

inline int maxTop(StackInfo info[],int len)
{
	int i = 0;
	int max = -1;
	int maxid = -1;
	for(i=0;i<len;i++)
	{
		if(info[i].top <= 0 ) continue;
		if(info[i].data[info[i].top-1] > max)
		{
			max = info[i].data[info[i].top-1];
			maxid = i;
		}
	}
	return maxid;
}


int rateWrite(FILE *fp,int start,double rate)
{
	int iRate = rate * 5;
	for(int i=0;i<5;i++)
	{
		fprintf(fp,"%d:%d ",start+i,iRate == i ? 1 : 0 );
	}
	return 1;
}

// rank == 1,2,3,4,5+
int rankWrite(FILE *fp,int start,int rank,int len)
{
	
	for(int i=0;i<len-1;i++)
	{
		fprintf(fp,"%d:%d ",start+i,rank == i+1 ? 1 : 0);
	}
	fprintf(fp,"%d:%d ",start+len-1,rank >= len ? 1 : 0);
	return 1;
}

inline int offsetStat(int offset,int *totalData,int *beforeData,
					OffsetCallback offsetFunction)
{
	*totalData = 0;
	*beforeData = 0;
	int nowOffset = 0;
	int tmp = 0;
	while((nowOffset=offsetFunction(nowOffset,getPclen())) != 0)
	{
		(*totalData)++;
		if(nowOffset < offset)
		if(nowOffset-tmp >= DIFFKWDTH) (*beforeData)++;
		tmp = nowOffset;
	}
	return 1;
	
}

inline int offsetBetweenStat(int offsetEnd,int offsetStart,int *dataBetween,
					OffsetCallback offsetFunction)
{
	*dataBetween = 0;
	int tmp = 0;
	if(offsetEnd < offsetStart)
	{
		int tmp = offsetEnd;
		offsetEnd = offsetStart;
		offsetStart = tmp ; 
	}
	while((offsetStart=offsetFunction(offsetStart,getPclen())) != 0)
	{
		if(offsetStart >= offsetEnd) break;
		if(offsetStart-tmp >= DIFFKWDTH) (*dataBetween) ++ ;
		tmp = offsetStart;
	}
	return 1;
}

int gen26ToEnd(FILE *fp,featureData fd)
{
	//26 ~ 31
	int offset = fd.offset;
	int start = 26;
	int lmt = 200;
	
	char *content = getPcontent();
	int offend = getPclen();	
	int finac = 1;// :1 is not ascii code  :0 is ascii code
	// follow is not ascii code for instance : References [ right References Manual false
	for(int i=offset;i<offend;i++ )
	{
		if(content[i] != ' ')
		{
			finac = fitPattern('a',content[i]) && fitPattern('a',content[i+1]) ? 0 : 1;
			if(!finac && fitPattern('n',content[i]))
			{
				finac = 1;
			}
			break;
		}
	}
	fprintf(fp,"%d:%d ",start++,finac);
	
	//fp = stdout;
	//for(;lmt<=1000;lmt+=150)
	//{
	lmt = 200;
	
	
	fprintf(fp,"%d:%d ",start++,(hasPPafterTheOffset(offset,lmt)?1:0));
	fprintf(fp,"%d:%d ",start++,(hasPPafterTheOffset2(offset,lmt)?1:0));
	fprintf(fp,"%d:%d ",start++,(hasYearafterTheOffset(offset,lmt)?1:0));
	fprintf(fp,"%d:%d ",start++,(hasNameafterTheOffset0(offset,lmt)?1:0));
	fprintf(fp,"%d:%d ",start++,(hasNameafterTheOffset1(offset,lmt)?1:0));
	fprintf(fp,"%d:%d ",start++,(hasNameafterTheOffset2(offset,lmt)?1:0));
	
	fprintf(fp,"%d:%d ",start++,(hasPPafterTheOffset(offset,lmt) >= hasPPafterTheOffset(offset,-lmt)));
	fprintf(fp,"%d:%d ",start++,(hasPPafterTheOffset2(offset,lmt) >= hasPPafterTheOffset2(offset,-lmt)));
	fprintf(fp,"%d:%d ",start++,(hasYearafterTheOffset(offset,lmt) >= hasYearafterTheOffset(offset,-lmt)));
	fprintf(fp,"%d:%d ",start++,(hasNameafterTheOffset0(offset,lmt) >= hasNameafterTheOffset0(offset,-lmt)));
	fprintf(fp,"%d:%d ",start++,(hasNameafterTheOffset1(offset,lmt) >= hasNameafterTheOffset1(offset,-lmt)));
	fprintf(fp,"%d:%d ",start++,(hasNameafterTheOffset2(offset,lmt) >= hasNameafterTheOffset2(offset,-lmt)));


	lmt = 1000;
	
	fprintf(fp,"%d:%d ",start++,(hasSeqOfTheOffset(offset,lmt)?1:0));
	fprintf(fp,"%d:%d ",start++,(hasSeqOfTheOffset2(offset,lmt)?1:0));	
	fprintf(fp,"%d:%d ",start++,(hasSeqOfTheOffset(offset,lmt) >= hasSeqOfTheOffset(offset,-lmt)));
	fprintf(fp,"%d:%d ",start++,(hasSeqOfTheOffset2(offset,lmt) >= hasSeqOfTheOffset2(offset,-lmt)));
	//}
	
	//32
	//fprintf(fp,"%d:%f ",start++,(double)offset/getPclen());
	//fprintf(fp,"%d:%f ",start++,(double)(getPclen()-offset)/offset);
	rateWrite(fp,start,(double)offset/getPclen());
	start+=5;
	
	for(int i=0;i<CALLBACK_LEN;i++)
	{
		//rateWrite(fp,start,(fd.offset == getPclen()) ? -1 :((double)fd.fid[i][0]/fd.offset)/
		//			((fd.fid[i][1]-fd.fid[i][0])/(getPclen()-fd.offset)));
		//start+=5; //TODO ADD 
		
		fprintf(fp,"%d:%d ",start++,fd.density[i][0][1] >= fd.density[i][0][0]);
		fprintf(fp,"%d:%d ",start++,fd.density[i][1][1] >= fd.density[i][1][0]);
		fprintf(fp,"%d:%d ",start++,fd.density[i][2][1] >= fd.density[i][2][0]);
		fprintf(fp,"%d:%d ",start++,fd.vari[1] >= fd.vari[0]);
		
		//fprintf(fp,"%d:%f ",start++,quot(fd.density[i][0][1],fd.density[i][0][0]));
		//fprintf(fp,"%d:%f ",start++,quot(fd.density[i][1][1],fd.density[i][1][0]));
		//fprintf(fp,"%d:%f ",start++,quot(fd.density[i][2][1],fd.density[i][2][0]));
		//fprintf(fp,"%d:%f ",start++,quot(fd.vari[1],fd.vari[0]));
		
		int wide = 6;
		rankWrite(fp,start,fd.seq[i][0],wide);
		start+=wide;

		rankWrite(fp,start,fd.seq[i][1],wide);
		start+=wide;
		
		rankWrite(fp,start,fd.seq[i][2],wide);
		start+=wide;
		
		rankWrite(fp,start,fd.seq[i][3],wide);
		start+=wide;

	}
	return 1;
}




//int bingo = 0;
//int generateSample(const char* fileName,int isDir)
int genStartSampleCtl(const char* fileName,int isDir)
{
	static int id = 1;
	FILE *fp;
	int trainOrTest;
        if(isDir)
        {
                printf("ignore dir:%s\n",fileName);
                return 1;
        }
	if(rand()%2) //train is 50% and test is 50%
	{
		fp = fpTrain;
		trainOrTest = 1;
	}else
	{
		trainOrTest = 0;
		fp = fpTest;
	}
	
	
	
        printf("[%d] %s:%s",id,(trainOrTest?"train":"test"),fileName);
        fflush(NULL);
        
        // parse tag or etc ,move data to RAM
 	initContent();
	if(!parseFile(fileName))
	{
		fprintf(stderr,"[[error parsing file : #%s#]]",fileName);
		getchar();
		return 0;
	}
	
	// search from db
	if(!getFeature(fileName,&mfdc)) // get One To Five from DB
	{
		int i;
		//int j;
		int threshold;
		unsigned int refOffset;
		StackInfo info[FEATURE_SIZE];
		for(threshold=0;threshold<FEATURE_SIZE;threshold++) 
			stackData(&info[threshold],isAccpted,getPcontent(),getPclen(),threshold);
			
		unsigned int count[FEATURE_SIZE];
		//unsigned int status[FEATURE_SIZE];
		int maxid;
		int maxoffset;
	
		int isPositive;
		
		// DB data set
		mfd.qid = id;
	
		refOffset = getReferenceHeadOffset();
		
		//init count 
		for(i=0;i<FEATURE_SIZE;i++) count[i]=1;
		
		//data
		mfdc.top = 0;
		
		while(!allZero(info,FEATURE_SIZE))
		{	
			maxid = maxTop(info,FEATURE_SIZE);
			maxoffset = info[maxid].data[info[maxid].top-1];
			isPositive = VALUESDIFF(refOffset,maxoffset) < 10 ;
			
			mfd.positive = isPositive;
			mfd.offset = maxoffset;
			
			for(i=0;i<FEATURE_SIZE;i++)
			{
				//status[i] = 0;
				mfd.t[i] = 0;
				if(VALUESDIFF(info[i].data[info[i].top-1],maxoffset)<10)
				{
					info[i].top--;
					//status[i] = count[i];
					//DB data set
					mfd.t[i] = count[i];//1,2,3,...
					
					count[i]++;//++
				}
			
			}

			if(!insertFeature(fileName,mfd))
			{
				fprintf(stderr,"[DB] insertFeature()(1): error --%d",__LINE__);
				//getchar();
			}
		
			mfdc.data[mfdc.top] = mfd;
			mfdc.top ++ ;
			
		}
		
		//db insert
		if(!insertFeatureInfo(fileName,mfdc.top))
		{
			fprintf(stderr,"[DB] insertFeatureInfo()(2): error %d",__LINE__);
			getchar();
		}
	} // end of GET DATA FROM DB
	

	
	printf(".");fflush(NULL);//TODO TIPS
	
	// generate feature info data
	int beforeData;
	int totalData;
	for(int i=0;i<mfdc.top;i++)
	{
		
		for(int j=0;j<CALLBACK_LEN;j++)
		{
			offsetStat(mfdc.data[i].offset,
					&totalData,&beforeData,functionList[j]);
			mfdc.data[i].fid[j][0] = beforeData;
			mfdc.data[i].fid[j][1] = totalData;
		}
	}
	
	//density and etc
	
	printf(".");fflush(NULL);//TODO TIPS
	
	// adjacencyOffset setting ... 
	for(int i=0;i<mfdc.top;i++)
	{
		mfdc.data[i].adjacencyOffset[0] = 0;
		mfdc.data[i].adjacencyOffset[1] = getPclen();
		for(int j=0;j<mfdc.top;j++)
		{
			if(i == j) continue;
			if(mfdc.data[i].offset < mfdc.data[i].offset && mfdc.data[i].adjacencyOffset[0] < mfdc.data[i].offset)
				mfdc.data[i].adjacencyOffset[0] = mfdc.data[i].offset ;
			else if(mfdc.data[i].offset > mfdc.data[i].offset && mfdc.data[i].adjacencyOffset[1] > mfdc.data[i].offset)
				mfdc.data[i].adjacencyOffset[1] = mfdc.data[i].offset ;
		}
	}
	
	
	printf(".");fflush(NULL);//TODO TIPS
	
	int diff;
	int lmt;
	//density && variance setting ... 
	CloseKWD ckwd;
	for(int i=0;i<mfdc.top;i++)
	{
		for(int j=0;j<CALLBACK_LEN;j++)
		{
			mfdc.data[i].density[j][0][0] = (double)mfdc.data[i].fid[j][0]/NOTZERO(mfdc.data[i].offset);
			mfdc.data[i].density[j][0][1] = (double)(mfdc.data[i].fid[j][1]-mfdc.data[i].fid[j][0])/NOTZERO(getPclen()-mfdc.data[i].fid[j][0]);
			
			offsetBetweenStat(mfdc.data[i].offset,mfdc.data[i].adjacencyOffset[0],&diff,functionList[j]);
			//mfdc.data[i].density[j][1][0] = (double)(mfdc.data[i].offset-mfdc.data[i].adjacencyOffset[0])/NOTZERO(diff);
			mfdc.data[i].density[j][1][0] = (double)diff/NOTZERO(VALUESDIFF(mfdc.data[i].offset,mfdc.data[i].adjacencyOffset[0]));
			
			offsetBetweenStat(mfdc.data[i].offset,mfdc.data[i].adjacencyOffset[1],&diff,functionList[j]);
			mfdc.data[i].density[j][1][1] = (double)diff/NOTZERO(VALUESDIFF(mfdc.data[i].adjacencyOffset[1],mfdc.data[i].offset));
			
			
			lmt = 300;
			// prev && next 300* words
			offsetBetweenStat(mfdc.data[i].offset,LMB(mfdc.data[i].offset-lmt),&diff,functionList[j]);
			mfdc.data[i].density[j][2][0] = (double)diff/NOTZERO(VALUESDIFF(mfdc.data[i].offset,LMB(mfdc.data[i].offset-lmt)));
			
			offsetBetweenStat(mfdc.data[i].offset,LMB(mfdc.data[i].offset+lmt),&diff,functionList[j]);
			mfdc.data[i].density[j][1][1] = (double)diff/NOTZERO(VALUESDIFF(LMB(mfdc.data[i].offset+lmt),mfdc.data[i].offset));
			
			// variance 
			//int getCloseKWD(int offset,CloseKWD *closeKWD,int (*callback)(int offset,int limit));
			getCloseKWD(mfdc.data[i].offset,&ckwd,functionList[j]);
			mfdc.data[i].vari[0] = getVariance(ckwd.prev,ckwd.pLen);
			mfdc.data[i].vari[1] = getVariance(ckwd.next,ckwd.nLen);
		}
	}
	
	
	// sequence setting ... 
	printf(".");fflush(NULL);//TODO TIPS
	for(int i=0;i<mfdc.top;i++)
	{
		for(int k=0;k<CALLBACK_LEN;k++)
		{
			for(int z=0;z<4;z++) mfdc.data[i].seq[k][z] = 1;
		}
		
		for(int j=0;j<mfdc.top;j++)
		{
			if(i==j) continue;
			for(int k=0;k<CALLBACK_LEN;k++)
			{
				if(mfdc.data[i].density[k][0][1]/NOTZERO(mfdc.data[i].density[k][0][0])
					< mfdc.data[j].density[k][0][1]/NOTZERO(mfdc.data[j].density[k][0][0]))
					mfdc.data[i].seq[k][0] ++;
				
				if(mfdc.data[i].density[k][1][1]/NOTZERO(mfdc.data[i].density[k][1][0])
					< mfdc.data[j].density[k][1][1]/NOTZERO(mfdc.data[j].density[k][1][0]))
					mfdc.data[i].seq[k][1] ++;
				
				if(mfdc.data[i].density[k][2][1]/NOTZERO(mfdc.data[i].density[k][2][0])
					< mfdc.data[j].density[k][2][1]/NOTZERO(mfdc.data[j].density[k][2][0]))
					mfdc.data[i].seq[k][2] ++;	
				
				if(mfdc.data[i].vari[1]/NOTZERO(mfdc.data[i].vari[0])
					< mfdc.data[j].vari[1]/NOTZERO(mfdc.data[j].vari[0]))
					mfdc.data[i].seq[k][3] ++;
			}
		}
	}
	
	
	
	printf(".");fflush(NULL);//TODO TIPS
	// write sample
	fprintf(fp,"#paper %d[%s] ",id,fileName);
	for(int i=0;i<mfdc.top;i++) fprintf(fp,"[%d:%d]",i,mfdc.data[i].offset);
	fprintf(fp,"\n");
	
	for(int i=0;i<mfdc.top;i++)
	{
		
		if(REMOVE_ONLY_FIVE)
		{
			if(mfdc.data[i].t[0]==0&&mfdc.data[i].t[1]==0&&mfdc.data[i].t[2]==0&&mfdc.data[i].t[3]==0)
				continue;
		}
		//fprintf(fp,"%c1 qid:%d ",mfdc.data[i].positive?'+':'-',i);
		fprintf(fp,"%c1 ",mfdc.data[i].positive?'+':'-');
		for(int j=0;j<FEATURE_SIZE;j++)
			rankWrite(fp,j*5+1,mfdc.data[i].t[j],5);
		//	fprintf(fp,"%d:%d ",j+1,mfdc.data[i].t[j]);
			
		gen26ToEnd(fp,mfdc.data[i]);
		fprintf(fp,"\n");
		
		printf(".");fflush(NULL);//TODO TIPS
	}
		
	
	
	printf("  [done]\n");
        cleanContent();
        selfAddFileNum();
        id++;
        return 1;
}


// set and get
int getFileNum(){return fileNum;}
void selfAddFileNum() { fileNum++;}
void setTrainFile(FILE *fp){ fpTrain = fp;}
void setTestFile(FILE *fp){fpTest = fp;}
FILE * getTrainFile(){return fpTrain;}
FILE * getTestFile(){return fpTest;}


