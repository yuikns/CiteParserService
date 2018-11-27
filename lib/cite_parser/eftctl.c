#include "eftctl.h"
#include "eftfun.h"
#include "hftctl.h"
#include "debuginfo.h"
#include "persistence.h"
#include "virtualcontent.h"
#include "tokens.h"
#include "minEditDistance.h"
#include "common.h"
#include "hftnpse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *flog;

void openLogFile()
{
	flog = fopen("afterend.log","w");
}


void closeLogFile()
{
	fclose(flog);
}

#define ONLY(x) if(strncmp(x,fileName,strlen(x))!=0) return 1;

int errs = 0;

int genEndSampleCtl(const char* fileName,int isDir)
{
	//
	/**
	 * todo list:
	 * data/oraby/A framework for assisted exploration with collaboration.txt
	 *
	 */
	//ONLY("data/orbz_sec/Animating facial expressions.txt");
       	//ONLY("data/orbz_sec/Antecedents and consequences of job satifaction among information center employees.txt");
	FILE *fp;
	static int id = 0;
	int trainOrTest;
	unsigned int targetOffset;
	unsigned int startOffset;
	endFeatureDataContainer *_mfdc =getEndFeatureDataContainer();
	//endFeatureDataContainer tmpCter ;
	memset(_mfdc,0,sizeof(endFeatureDataContainer));
	//memset(&tmpCter,0,sizeof(endFeatureDataContainer));
	// ignore dir
	if(isDir)
        {
                printf("ignore dir:%s\n",fileName);
                return 1;
        }
        
        // train or test
	if(rand()%2) //train is 50% and test is 50%
	{
		fp = getTrainFile();
		trainOrTest = 1;
	}else
	{
		fp = getTestFile();
		trainOrTest = 0;
	}
	
	//printf("[%d] %s:%s",id,(trainOrTest?"train":"test"),fileName);
	printf("[%d] %s:%s",id,(trainOrTest?"train":"test"),queryEscape(fileName));
	
	//printf("\n");
	//TODO 	
        
        // parse tag or etc ,move data to RAM
 	initContent();
	if(!parseFile(fileName))
	{
		fprintf(stderr,"[[error parsing file : #%s#]]",fileName);
		//getchar();
		return 0;
	}
	
	targetOffset = getReferenceEndOffset();
 	startOffset = getReferenceHeadOffset();
	//step 1: offsets generate
	//if(!getFeature(fileName,_mfdc)) // get One To Five from DB
	//TODO read and write offsets from | into db 
	{
		cleanFeature(fileName);	
		// get head of references
		basicFilter(_mfdc,startOffset);
		combineOffsets(_mfdc);
		
		//!INSERT INTO DB
		/*
		for(int i = 0; i < _mfdc->top;i++)
		{
			
			if(!insertFeature(fileName,_mfdc->data[i]))
			{
				fprintf(stderr,"[DB] insertFeature()(1): error --%d",__LINE__);
				//getchar();
			}
		}*/
		
		//tmpCter = *_mfdc;
		//makeSequenceForCombinedOffsets(&tmpCter);
	}


	// log TODO
	//fprintf(flog,"##[[DATA:%s|\n%20s]]\n",fileName,getPcontent()+targetOffset);
		
	// 0 empty for valued -- index of all offsets
	// 1 acknowledgements etc.
	// 2 table , he is figure ... (a list)
	// 3 end of year // before end of content
	// 4 end of page // before end of content
	// 5 end of page2 // before end of content
	// 6 end of article
	// 7 end year before ack
	// 8 end year before table
	// 9 end year before ack or table
	// 10 end page before ack
	// 11 end page before table
	// 12 end page before ack or table
	// 13 end page2 before ack
	// 14 end page2 before table
	// 15 end page2 before ack or table
	// 16 index of year
	// 17 index of page
	// 18 index of page2
	
	//step 3: write into file
	fprintf(fp,"# %s \n",fileName);
	//int pptag = 0;
	int start = 1;
	for(int i = 0; i < _mfdc->top ;i++)
	{
		printf(".");
		//adjust offset 
		int adja = hasYearafterTheOffset(_mfdc->data[i].offset-4,30);
		int adjb = hasPPafterTheOffset(_mfdc->data[i].offset-4,30);
		int adj = MAX2(adja,adjb);
		//"%c1 ",);
		//printf("[%c1 ]DIFF:%d-%d",!haveDiffernecesD(_mfdc->data[i].offset,targetOffset)?'+':'-'
		//		,_mfdc->data[i].offset,adj);
		//offset+=diff;
		if(adj != 0) _mfdc->data[i].offset = adj;
		//printfContextS(_mfdc->data[i].offset,"CONTEXTS");
		
		
		
		//positive
		fprintf(fp,"%c1 ",!haveDiffernecesD(_mfdc->data[i].offset,targetOffset)?'+':'-');
		//if(!haveDifferneces(_mfdc->data[i].offset,targetOffset)) pptag ++;
		start = 1;
		for(int j=0;j<ENDLEN;j++)
		{
			//break;//when debuging .. .. ingore these info
			switch(j)
			{
			//*
			// f 0
			case 0:
				//rateWrite(fp,start,(double)_mfdc->data[i].t[0]/_mfdc->top);
				//start+=5;
				//rankWrite(fp,start,_mfdc->top-_mfdc->data[i].t[0]+1,5);
				rankWriteNoMore(fp,start,_mfdc->top-_mfdc->data[i].t[0]+1,5);
				start+=5;
				break;
			// */
			
			//*
			// f 1
			case 1:
			case 2:
				// 1 acknowledgements etc.
				// 2 table , he is figure ... (a list)
				//fprintf(fp,"%d:%d ",start++,_mfdc->data[i].t[j]);
				rankWrite(fp,start,_mfdc->data[i].t[j],3);
				start += 3;
				
				//if(_mfdc->data[i].t[j])
				//{
					//printf("{%d-diff:%d}",j,ABSDIFF(_mfdc->data[_mfdc->top].offset,targetOffset));
				//	if(diffs >ABSDIFF(_mfdc->data[i].offset,targetOffset))
				//	{
				//		diffs = ABSDIFF(_mfdc->data[i].offset,targetOffset);
				//		diffsj = j;
				//	}
				//}
				break;
			// */
			
			//* 
			// f 2
			case 3:
			case 4:
			case 5:
				// 3 end of year // before end of content
				// 4 end of page // before end of content
				// 5 end of page2 // before end of content
				// 0 , 1 , 2
				rankWrite(fp,start,_mfdc->data[i].t[j]+1,3);
				start += 3;
				break;
			// */
	
			//*
			// f 3
			case 6: // end of article
				// 0 , 1 , 2
				fprintf(fp,"%d:%d ",start++,chkSpecialFlag()?
						//_mfdc->data[i].t[j]:
						0:
						_mfdc->data[i].t[j]*2);
				break;
			// */
			
			//*
			// f 4
			case 7:
			case 8:
			case 10:
			case 11:
			case 13:
			case 14:
				// 9 end year before ack or table
				// 12 end page before ack or table
				// end year before article
				// 0 , 1 , 2,
				rankWrite(fp,start,_mfdc->data[i].t[j],5);
				start += 5;
				break;	
			// */
			
			//*		
			case 9:
			case 12:
			case 15:
				// 9 end year before ack or table
				// 12 end page before ack or table
				// end year before article
				// 0 , 1 , 2,
				//rankWrite(fp,start,_mfdc->data[i].t[j],3);
				rankWrite(fp,start,_mfdc->data[i].t[j-1]+_mfdc->data[i].t[j-2],5);
				start += 5;
				break;
			// */
			
			//*
			// f5
			//
			//
			//
			
			//
			case 16:
			case 17:
			case 18:
				powerWriteNoMore(fp,start,_mfdc->data[i].t[j],5);
				start+=5;
				break;
			// */
			
			//*
			case 19:
			case 20:
			case 22:
			case 23:
			case 25:
			case 26:
			// */
			//*
			case 21:
			case 24:
			case 27:
				// the min the better
				// ABSDIFF of INDEX
				// problem
				
				//if(_mfdc->data[i].t[j]!=0)
				//{
				//	printf("[E:%d(i:%dj:%d)]",_mfdc->data[i].t[j],i,j);
				//}
				powerWriteNoMore(fp,start,_mfdc->data[i].t[j],8);
				start+=8;
				break;
			// */
			}
		
			
		}
		/*
		rankWrite(fp,start,_mfdc->data[i].t[7]+_mfdc->data[i].t[8]
					+_mfdc->data[i].t[10]+_mfdc->data[i].t[11]
					+_mfdc->data[i].t[13]+_mfdc->data[i].t[14]
					+_mfdc->data[i].t[3]+_mfdc->data[i].t[4]
					+_mfdc->data[i].t[5],8);
		*/
		//start += 8;
			
		setNextElemOffset(i<_mfdc->top-1?_mfdc->data[i+1].offset:0);
		start = genNextDataForEndfeature(fp,_mfdc->data[i],start);
		
		
		// endness of data
		fprintf(fp," #%d",_mfdc->data[i].offset);
		fprintf(fp,"\n");
		// debuginfo to file
		/*
		printfContextF(_mfdc->data[i].offset,"DEBUG INFO",fp);
		fprintf(fp,"[0:%d]",_mfdc->top-_mfdc->data[i].t[0]+1);
		for(int j=1;j<ENDLEN;j++)
			fprintf(fp,"[%d:%d]",j,_mfdc->data[i].t[j]);
		fprintf(fp,"\n");
		//*/
	}
	
	
	//DEBUG
	
	//if(!pptag)
	//{
	//	printf("[~]");
	//}
	//step 4: finish handle
	//printf("(%d)",pptag);
	printf(" ok\n");
	id++;
	
	
		
	fflush(NULL);
	cleanContent();
	selfAddFileNum();
	return 1;
}



