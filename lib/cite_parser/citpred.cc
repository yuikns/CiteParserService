#include "citpred.h"
#include "crftctl.h"
#include "crfpp.h"
#include "virtualcontent.h"
#include "dirtrav.h"
#include "hftctl.h" // int getReferenceHeadOffset(void);
#include "eftfun.h" // int getReferenceEndOffset(void);
#include "strHandle.h" //spilitContent
#include "tokens.h"
#include "minEditDistance.h"
#include "crftfun.h"
#include "hftnpse.h"
#include "dict.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include <queue>

#ifndef WIN32
#include <strings.h>
#endif // WIN32

#include "debuginfo.h"
#include <iostream>

int initCitationNode(pCitationNode node)
{
	int i;
	if(node == NULL) return 0;
	for(i=0;i<10;i++) memset(node->author[i],0,50);
	for(i=0;i<10;i++) memset(node->editor[i],0,50);
	memset(node->title,0,50);
	memset(node->journal,0,50);
	memset(node->booktitle,0,50);
	memset(node->publisher,0,50);
	memset(node->institution,0,50);
	memset(node->volume,0,50);
	memset(node->location,0,50);
	memset(node->pages,0,50);
	memset(node->date,0,50);
	memset(node->url,0,50);
	memset(node->isbn,0,50);
	memset(node->tech,0,50);
	memset(node->other,0,50);
	node->next = NULL;
	return 1;	
}

int cnIsEmpty(pCitationNode node)
{
	if(node == NULL) return 1;
	if((node->author[0][0] != 0) ||(node->editor[0][0] != 0) 
		|| node->title[0] != 0 || node->journal[0] != 0
		|| node->booktitle[0] != 0 || node->publisher[0] != 0
		|| node->institution[0] != 0 || node->volume[0] != 0
		|| node->location[0] != 0 || node->pages[0] != 0
		|| node->date[0] != 0 || node->url[0] != 0
		|| node->isbn[0] != 0 || node->tech[0] != 0
		|| node->other[0] != 0 ) return 0;
	return 1;
}

pCitationNode addCitationNode(pCitationNode *node)// node , token id
{
	pCitationNode p = *node;
	if(p == NULL)
	{
		p = new CitationNode;
		
	}else if(cnIsEmpty(p))
	{
		return p;
	}else
	{
		while(p->next != NULL) p = p->next;
		p->next = new CitationNode;
		p = p->next;
	}
	*node = p;
	initCitationNode(p);
	return p;
}

void freeCitationNode(pCitationNode *node)
{
	pCitationNode p1 = *node;
	pCitationNode p2 = p1;
	while(p2 != NULL)
	{
		p2 = p2 -> next;
		free(p1);
		p1 = p2;
		
	}
	
	*node = NULL;
}

void tagFinishing(char *str)
{
	int len=strlen(str);
	int i;
	int end = 1;
	for(i=len-1;i>=0;i--)
	{
		if(isAsciiOrDigit(str[i])) end = 0;
		if(str[i] == '\n' || str[i] == '\r') str[i] = ' ';
		if(end && (str[i] == ',' || str[i] =='[' || str[i]=='{'
			|| str[i] == '}'))
			str[i] = '\0';
	}
}

void spilitName(char name[10][50],const char *src)
{
	int len = strlen(src);
	int line = 0;
	int j=0;
	int i;
	for(i=0;i<len;i++)
	{
		if(line >= 10 ) return;
		if(src[i] == ',')
		{
			name[line][j] = '\0';
			line++;
			j = 0;
			continue;
		}
		else if((strncmp(src+i,"and",3) == 0) && i > 0)
		{
			if(!isAsciiOrDigit(src[i-1]))
			{
				i+=3;
				name[line][j] = '\0';
				line++;
				j = 0;
				continue;
			}
		}
		else if(strncmp(src+i,"et",2)==0)
		{
			int k;
			for(k=i+2;k<len;k++)
			{
				if(isAsciiOrDigit(src[k]))
				{
					if(strncmp(src+k,"al",2)==0)
					{
						name[line][j] = '\0';
						line++;
						if(line < 10 ) sprintf(name[line],"et. al.");
						return;
					}
				}
			}
		}
		else
		{
			if(!(j==0 && !isAsciiOrDigit(src[i])))
			{
				name[line][j] = src[i];
				j++;
			}
		}
	}
}

pCitationNode addCitationInfo(pCitationNode *node,const char *str,int len,int id)
{
	pCitationNode p = *node;
	char src[2048];
	memcpy(src,str,len);
	src[len] = '\0';
	
	//printf("%s[%s]\n",src,id2Token(id));
	
	int page[2];
	int pn ;
	
	int vol[2];
	int vn ;
	
	switch(id)
	{
		case 0:
			int lnum;
			int ev[2];
			for(lnum = 0; lnum < len; lnum++)
			{
				if(DIGITLIKE(src[lnum])) ev[0] = 1;
				if(src[lnum]=='['||src[lnum]==']') ev[1] = 1;
			}
			if(ev[0] && ev[1]) p = addCitationNode(&p);
			break;
		case 3: // author
			p = addCitationNode(&p);
			//snprintf(p->author[0],50,"%s",src);
			spilitName(p->author,src);
			break;
		case 4: // booktitle
			tagFinishing(src);
			snprintf(p->booktitle,150,"%s",src);
			break;
		case 5: // date
			tagFinishing(src);
			snprintf(p->date,50,"%s",src);
			break;
		case 6: // editor
			//tagFinishing(src);
			//snprintf(p->editor[0],50,"%s",src);
			spilitName(p->author,src);
			break;
		case 7: // institution
			tagFinishing(src);
			snprintf(p->institution,150,"%s",src);
			break;
		case 8: // journal
			tagFinishing(src);
			snprintf(p->journal,150,"%s",src);
			break;
		case 9: // location
			tagFinishing(src);
			snprintf(p->location,50,"%s",src);
			break;
		case 11: // pages
			pn= twovalue(src,len,&page[0],&page[1]);
			if(pn == 2)
				snprintf(p->pages,50,"%d,%d",page[0],page[1]);
			else
				snprintf(p->pages,50,"%d",page[0]);
			break;
		case 12: // publisher
			tagFinishing(src);
			snprintf(p->date,150,"%s",src);
			break;
		case 13: // tech
			tagFinishing(src);
			snprintf(p->tech,50,"%s",src);
			break;
		case 14: // title
			tagFinishing(src);
			snprintf(p->title,150,"%s",src);
			break;
		case 15: // volume
			vn = twovalue(src,len,&vol[0],&vol[1]);
			if(vn == 2)
				snprintf(p->volume,50,"%d,%d",vol[0],vol[1]);
			else
				snprintf(p->volume,50,"%d",vol[0]);
			break;
		case 16: // url
			tagFinishing(src);
			snprintf(p->url,50,"%s",src);
			break;
		case 17: // isbn
			tagFinishing(src);
			snprintf(p->isbn,50,"%s",src);
			break;
	}
	*node = p;
	return p;	
}


int predftEnQueue(pCNSQ Q,int *currentOffset,char *mpredeli,int refAreaEnd)
{
	if(isFullQueue(Q)) return 0;
	char *content = getPcontent();
	char str[SINGLEWORDLEN];
	char nextpre;
	
	CrfNodeSnapshot crfNodeSnapshot;
	
	int isPublisher = 0;
	
	//spilitContent(char *dest,int dlen,const char *src,int len)
	if((crfNodeSnapshot.offset = spilitContent(str,SINGLEWORDLEN,
			content+(*currentOffset),
			refAreaEnd-(*currentOffset),
			&(crfNodeSnapshot.predeli),
			&(crfNodeSnapshot.nextdeli),
			&nextpre)) > 0)
	{
		int slen = strlen(str);
		sprintf(crfNodeSnapshot.str,"%s",str);
		crfNodeSnapshot.slen = slen;
		//int dval;
		char partStr[1024];
		int psI = 0;
		
		// abbr
		int abbrc = 0; // in Connect status
		int abbrl = 0; // abbr length
		int abbrs = 0 ; // abbr start type
		
		crfNodeSnapshot.quotflag = 0;
		crfNodeSnapshot.pareSflag = 0;
		crfNodeSnapshot.pareEflag = 0;
		crfNodeSnapshot.sqbSflag = 0;
		crfNodeSnapshot.sqbEflag = 0;
		crfNodeSnapshot.braSflag = 0;
		crfNodeSnapshot.braEflag = 0;
		crfNodeSnapshot.stopflag = 0;
		
		for(int i=(*currentOffset);i<=crfNodeSnapshot.offset+(*currentOffset);i++)
		{
			if(i>=refAreaEnd) break;
			if(DIGITLIKE(content[i]))
			{
				partStr[psI] = content[i];
				psI++;
				
			}else if(psI>0)
			{
				partStr[psI]='\0';
				isPublisher = isPublisher || isPublisherInDict(partStr); 
			}
			switch(content[i])
			{
				case '\"':
					crfNodeSnapshot.quotflag ++;
					break;
				case '(':
					crfNodeSnapshot.pareSflag = 1;
					break;
				case ')':
					crfNodeSnapshot.pareEflag = 1;
					break;
				case '[':
					crfNodeSnapshot.sqbSflag = 1;
					break;
				case ']':
					crfNodeSnapshot.sqbEflag = 1;
					break;
				case 'I':
				case 'l':
					if(isBlank(content[i+1]) && crfNodeSnapshot.sqbSflag)
					{
						crfNodeSnapshot.sqbEflag = 1;
					}
					break;
				
				case '{':
					crfNodeSnapshot.braSflag = 1;
					break;
				case '}':
					crfNodeSnapshot.braEflag = 1;
					break;	
					
				case ',':
					// filter Abbreviation
					//if(!(abbrc && abbrl < 5 && abbrs))
					// Dept. 		et. al.
					// break: A A. ?
					if(abbrc)
					{
						if(abbrl < 6 && abbrs) break;
						if(abbrl < 3) break;
					}
					if(crfNodeSnapshot.stopflag == 2)
						crfNodeSnapshot.stopflag = 2;
					else
						crfNodeSnapshot.stopflag = 1;
					break;
				
				case '.':
					if(abbrc)
					{
						if(abbrl < 6 && abbrs) break;
						if(abbrl < 3) break;
					}
					crfNodeSnapshot.stopflag = 2;
					
					break;
				case '!':
				case '?':
					crfNodeSnapshot.stopflag = 2;
					break;
				
					
			}
			
			if(i == (*currentOffset)) // xxx. Aaa.
			{
				if(isAsciiCode(content[i]))
				{
					abbrl = 0;
					abbrc = 1;
					abbrs = isUppercaseCode(content[i]) ? 1 : 0 ;
				}
			}else if(!isAsciiOrDigit(content[i-1]) && isAsciiCode(content[i])){
				abbrc = 1;
				abbrl = 0;
				abbrs = isUppercaseCode(content[i]) ? 1 : 0 ;
			}else if(abbrc && (content[i]>='a' || content[i] <= 'z')){
				abbrl ++;
			}else
			{
				abbrc = 0;
				abbrl = 0;
			}
		}

		*currentOffset += crfNodeSnapshot.offset;

		int spstr[10];
		int splen = 0;
		
		spilitStr(str,slen,spstr,&splen); // spilit

		sprintf(crfNodeSnapshot.str,"%s",str);
		
		// = isBlank(crfNodeSnapshot.predeli)?(*mpredeli):' ';
		crfNodeSnapshot.predeli = *mpredeli;
		crfNodeSnapshot.mpredeli = *mpredeli;
		
		crfNodeSnapshot.digitl = digitlen(str,slen);
		crfNodeSnapshot.puredigit = puredigit(str,slen);
		crfNodeSnapshot.dval = valofdigit(str,slen);
		crfNodeSnapshot.strtype = strfeature(str,slen);
		crfNodeSnapshot.yearlike = yearlike(str,slen);
		crfNodeSnapshot.monthlike = monthlike(str,slen);
		crfNodeSnapshot.volumnlike = vollkwd(str,slen);
		crfNodeSnapshot.pagelike = pagekwd(str,slen);
		crfNodeSnapshot.edsflag = edsFlag(str,slen);
		crfNodeSnapshot.speflag = specialFlag(str,slen);
		crfNodeSnapshot.procflag =  procFlag(str,slen);		
		for(int z=0;z<splen && (crfNodeSnapshot.procflag == 0);z++)
		{
			//str+flag[i],flag[i+1]-flag[i]
			for(int k=z;k<splen;k++)
			{
				crfNodeSnapshot.procflag = procFlag(str+spstr[z],
								spstr[k+1]-spstr[z]);	
			}
			
		}
		//crfNodeSnapshot.namelike = hasNameafterTheOffset0((*currentOffset)
		//					-crfNodeSnapshot.offset-1,
		//					crfNodeSnapshot.offset+1);
		crfNodeSnapshot.namelike = namelike(str,slen,crfNodeSnapshot.nextdeli,
						crfNodeSnapshot.strtype);
						
		for(int z=0;z<splen && (crfNodeSnapshot.namelike == 0);z++)
		{
			//str+flag[i],flag[i+1]-flag[i]
			for(int k=z;k<splen;k++)
			{
				crfNodeSnapshot.namelike = namelike(str+spstr[z],
								spstr[k+1]-spstr[z],
								k==splen-1?crfNodeSnapshot.nextdeli:' ',
								strfeature(str+spstr[z],spstr[k+1]-spstr[z]));	
			}
			
		}
		
		
		crfNodeSnapshot.isNameDict = isNameInDict(str);
		for(int z=0;z<splen && (crfNodeSnapshot.isNameDict == 0);z++)
		{
			//str+flag[i],flag[i+1]-flag[i]
			crfNodeSnapshot.isNameDict = isNameInDict(str+spstr[z]);
		}
		
		crfNodeSnapshot.rLastNameDict = rateLastNameInDict(str);
		for(int z=0;z<splen && (crfNodeSnapshot.rLastNameDict == 0);z++)
		{
			//str+flag[i],flag[i+1]-flag[i]
			crfNodeSnapshot.rLastNameDict = rateLastNameInDict(str+spstr[z]);
		}
		
		
		crfNodeSnapshot.isCountryDict = isCountryInDict(str);
		for(int z=0;z<splen && (crfNodeSnapshot.isCountryDict == 0);z++)
		{
			//str+flag[i],flag[i+1]-flag[i]
			crfNodeSnapshot.isCountryDict = isCountryInDict(str+spstr[z]);
		}
		
		
		crfNodeSnapshot.isFunWordDict = isFunWordInDict(str);

		crfNodeSnapshot.isPlaceNameDict = isPlaceNameInDict(str);
		for(int z=0;z<splen && (crfNodeSnapshot.isPlaceNameDict == 0);z++)
		{
			//str+flag[i],flag[i+1]-flag[i]
			crfNodeSnapshot.isPlaceNameDict = isPlaceNameInDict(str+spstr[z]);
		}
		
		crfNodeSnapshot.isPubliserDict = isPublisherInDict(str) ||isPublisher;
		
		for(int z=0;z<splen && (crfNodeSnapshot.isPubliserDict == 0);z++)
		{
			//str+flag[i],flag[i+1]-flag[i]
			crfNodeSnapshot.isPubliserDict = isPublisherInDict(str+spstr[z]);
		}
		
		crfNodeSnapshot.isArticle = isArticle(str,slen);
		crfNodeSnapshot.deptflag = deptFlag(str,slen);
		crfNodeSnapshot.uniflag = uniFlag(str,slen);
		for(int z=0;z<splen && (crfNodeSnapshot.uniflag == 0);z++)
		{
			//str+flag[i],flag[i+1]-flag[i]
			for(int k=z;k<splen;k++)
			{
				crfNodeSnapshot.uniflag = uniFlag(str+spstr[z],
								spstr[k+1]-spstr[z]);	
			}
			
		}
		
		crfNodeSnapshot.ltdflag = ltdFlag(str,slen);
		for(int z=0;z<splen && (crfNodeSnapshot.ltdflag == 0);z++)
		{
			//str+flag[i],flag[i+1]-flag[i]
			for(int k=z;k<splen;k++)
			{
				crfNodeSnapshot.ltdflag = ltdFlag(str+spstr[z],
								spstr[k+1]-spstr[z]);	
			}
			
		}
		
		crfNodeSnapshot.domainflag = domainFlag(str,slen);
		
		

		if(crfNodeSnapshot.puredigit)
		{
			int vh = valofdigit(str,slen/2);
			int vl = valofdigit(str+(slen/2),(slen+1)/2);
			if(vh == 0 || vl == 0) 	crfNodeSnapshot.imprnum = 0;
			else crfNodeSnapshot.imprnum = vl > vh ? 1 : -1;
		}else
			crfNodeSnapshot.imprnum = 0;
		
		if(!isBlank(crfNodeSnapshot.nextdeli)) *mpredeli = nextpre;
		
		//*currentOffset = crfNodeSnapshot.offset;
		enQueue(Q,crfNodeSnapshot);
		return 1;
	}else
		return 0;
}



// predict one file
pCitationNode CitationInfoPredict(int startOffset,int endOffset)
{
	char samplestr[2048];
	char strpart[1024];
	int refAreaStart;
	int refAreaEnd;
	int currentOffset;
	int lastOffset;
	char mpredeli=' ';
	CNSQ preCNSQ;
	CNSQ nextCNSQ;
	
	clearQueue(&preCNSQ);
	clearQueue(&nextCNSQ);
	
	std::queue<OffsetCp> offsetCpQueue; // queue of start and end offset
	OffsetCp offsetCp; //  start and end offset
	
	pCrfNodeSnapshot pCNS;
	
	
	//
	refAreaStart = startOffset;
	refAreaEnd = endOffset;
	currentOffset = refAreaStart;

	CRFPP::Tagger *tagger = CRFPP::createTagger("-m model/cite.model -v 3 -n2");
	tagger->clear();

	if (!tagger) {
		std::cerr << CRFPP::getTaggerError() << std::endl;
		return 0;
	}
	
	
	int httpStatus = 0;

	int quotStatus[3];//  AT/NA IN/NIN OUT/NOUT
	int quotTime = 0;
	
	int pareStatus[3];// AT/NA IN/NIN OUT/NOUT 
	int paraFlag = 0;
	
	int sqbStatus[3];// AT/NAT IN/NIN OUT/NOUT 
	int sqbFlag = 0;
	
	int braStatus[3];// AT/NAT IN/NIN OUT/NOUT
	int braFlag = 0;
	
	
	// to make sure data is like this : )<<ignore this |from here to calculate>> ((()))
	int paraCache = 0; 
	int sqbCache = 0;
	int braCache = 0;
	
	int isbnEffect = 0;
	
	lastOffset = refAreaStart;
	//for(int i=refAreaStart;i<refAreaEnd;i++)
	//	putchar(*(getPcontent()+i));
	
	//printf("\n\n");
	//make queue full
	while(predftEnQueue(&nextCNSQ,&currentOffset,&mpredeli,refAreaEnd));
	while((pCNS = ftDeQueue(&nextCNSQ)) != NULL)
	{
		offsetCp.startOffset = lastOffset;
		offsetCp.endOffset = pCNS->offset == 0 ?
			refAreaEnd:
			offsetCp.startOffset+pCNS->offset;
		
		offsetCpQueue.push(offsetCp);
		//printf("%d-%d~%d",offsetCp.startOffset,offsetCp.endOffset,pCNS->offset);
		lastOffset = offsetCp.endOffset;
		//printf("(%d)",lastOffset);
		
		//printf("\n");
		
		memset(samplestr,0,2048); // init
		// 0. PREPARE : FLAGS
		
		
		// 0.0 PAST ONE INFO && NEXT ONE INFO
		pCrfNodeSnapshot lpCNS = pastNElem(&preCNSQ,1);  // previous one node
		pCrfNodeSnapshot npCNS = nextNElem(&nextCNSQ,1); // next one node
		
		// 0.1 TRAVERSAL ALL PAST && NEXT INFO , GET FLAGS
		int thesisFlag = 0;
		int ltdFlag = 0;
		int edsFlag = 0;
		int uniFlag = 0;
		int mitFlag = 0;
		int groupFlag = 0;
		int pressFlag = 0;
		int confFlag = 0; // conferences
		int orgFlag = 0; // organization (as,etc.)
		int labFlag = 0; // 
		int techFlag = 0;
		int repFlag = 0;
		int procFlag = 0;
		int andFlag = 0;
		//int resFlag = 0; // research
		
		int nextPDigit = 0; // next pure digit
		int domainFlag = 0;
		int domainNoStop = 1;
		int inStatus = 0;
		int i;
		int edNoStop = 1;
		
		
		
		int stopEffect = 0; // XXXX XXXX XXXX
			// 2:  ',' 
			// 1:  '.','?','!'..  
		
		// 0.1.1 NEXT 
		for(i=1;i < sizeQueue(&nextCNSQ); i++)
		{
			pCrfNodeSnapshot tCNS = (i==0)? pCNS : nextNElem(&nextCNSQ,i);

			if(stopEffect < 2) // 0 , 1
			{
				int sw = 1+stopEffect; // stop weight 1,2
				if(tCNS->speflag == 55 ) // rep/rept == report
				{
					repFlag = sw;
				}
				if((tCNS->speflag == 50 ) // tech
					|| (tCNS->speflag == 53 ) // rfc 
					|| (tCNS->speflag == 54 )) // tr
				{
					techFlag = sw;
				}
				
				// 71: lab
				// 72: Library
				// 73: Laboratory
				// 74: Center
				// 75: institute [X?] << institute
				if(((tCNS->speflag >= 71 )&&(tCNS->speflag < 75 ))
					|| 0) // institute
				{
					labFlag = sw;
				}
				if((tCNS->speflag == 77 ) //Corporation / Co.
					|| (tCNS->speflag == 78 )) // Organiza­tion
				{
					orgFlag = sw;
				}
				
				if((tCNS->speflag == 30 ) // Press
					|| (tCNS->speflag == 31 ) // Publishing
					|| (tCNS->speflag == 32 ))  // Publisher/Pub.
				{
					pressFlag = sw;
				}
				
				if((tCNS->speflag == 90) // conference
					|| (tCNS->speflag == 79) // Journal
					|| (tCNS->speflag == 60)) // communications
				{
					confFlag = sw; // journal
				}
				
				if((tCNS->deptflag == 1)  // dept.
					||(tCNS->speflag == 56) // group
					||(tCNS->speflag == 75) //institute
					) 
				{
					groupFlag = sw; // institute
				}
				
				if(tCNS->procflag == 1) // In xxx
				{
					inStatus = sw;
				}
				if(tCNS->procflag > 1) // Proc.
				{
					procFlag = sw;
				}
				
				if(tCNS->edsflag == 1 ) edsFlag = sw;
				//if(tCNS->edsflag == 1 && edNoStop) edsFlag = sw;
				
				if(tCNS->uniflag == 1) uniFlag = sw; // un of xxx in
									// un of ... press 
				
				if(tCNS->speflag == 25) thesisFlag = sw;
				
				if(tCNS->speflag == 70) mitFlag = sw;
				
				if(tCNS->ltdflag) ltdFlag = sw;
				
				
				// name & name
				if(strcmp(tCNS->str,"and") == 0) andFlag = sw;
			}
			if(stopEffect < 1) // 0
			{
			
			}
			///////////////////////////////////////////////////////////////////////
			
			if(!isBlank(tCNS->predeli) && (tCNS->predeli !=':')
							&& (tCNS->predeli !='/')
							&& (tCNS->predeli !='.')
							&& (tCNS->predeli !=',')) // for err
			{
				domainNoStop = 0;
			}
			
			if(domainNoStop)
			{
				if((tCNS->speflag == 20 || tCNS->speflag == 21) 
					&&!isBlank(tCNS->nextdeli))
					httpStatus = 1;
				if((tCNS->speflag == 22)&&(tCNS->nextdeli !=':')) httpStatus = 1;
				if(tCNS->domainflag)
					domainFlag = 1;
			}
			
			if(tCNS->puredigit > 0 && !tCNS->yearlike) nextPDigit ++ ;
			
			// stop effect
			if(i>0)
			{
				if(tCNS->stopflag  == 1) stopEffect = (stopEffect == 2) ? 2 : 1;
				if(tCNS->stopflag  == 2) stopEffect = 2;
			}
		}
		
		// 0.1.2 PREVIOUS
		httpStatus = 0;
		int seqFlag = 0;
		
		int puredata = 1;
		
		paraFlag = 0;
		sqbFlag = 0;
		braFlag = 0;
		paraCache = 0; 
		sqbCache = 0;
		braCache = 0;
		
		stopEffect = 0;
		domainNoStop = 1;
		edNoStop = 1;
		
		// << -- | 
		for(i=0;i < sizeQueue(&preCNSQ) ; i++)
		{
			pCrfNodeSnapshot tCNS = (i==0) ? pCNS : pastNElem(&preCNSQ,i);
			
			if(i>0)
			{
				if(tCNS->stopflag  == 1) stopEffect = (stopEffect == 2) ? 2 : 1;
				if(tCNS->stopflag  == 2) stopEffect = 2;
			}
	
			if(!tCNS->puredigit && tCNS->strtype!=4) puredata = 0;
			if( puredata && tCNS->speflag == 40) isbnEffect = 1;
				
			if(!isBlank(tCNS->predeli) && (tCNS->predeli !=':')
							&& (tCNS->predeli !='/')
							&& (tCNS->predeli !='.')
							&& (tCNS->predeli !=',')) // for err
			{
				domainNoStop = 0;
			}
			
				
			
			if(domainNoStop)
			{
				if((tCNS->speflag == 20 || tCNS->speflag == 21) 
					&&!isBlank(tCNS->nextdeli))
					httpStatus = 2;
				if((tCNS->speflag == 22)&&(tCNS->nextdeli !=':')) httpStatus = 2;
			}
	
			
			////////////////////////////////////////////////////////////////////
			
			
			if(stopEffect < 2) // 0 , 1
			{
				int sw = 3+stopEffect; // stop weight 3,4
				
				if(tCNS->speflag == 55 ) // rep/rept == report
				{
					repFlag = sw;
				}
				if((tCNS->speflag == 50 ) // tech
					|| (tCNS->speflag == 53 ) // rfc 
					|| (tCNS->speflag == 54 )) // tr
				{
					techFlag = sw;
				}
				
				// 71: lab
				// 72: Library
				// 73: Laboratory
				// 74: Center
				// 75: institute [X?]
				if(((tCNS->speflag >= 71 )&&(tCNS->speflag < 75 ))
					|| 0) // institute
				{
					labFlag = sw;
				}
				if((tCNS->speflag == 77 ) //Corporation / Co.
					|| (tCNS->speflag == 78 )) // Organiza­tion
				{
					orgFlag = sw;
				}
				
				if((tCNS->speflag == 30 ) // Press
					|| (tCNS->speflag == 31 ) // Publishing
					|| (tCNS->speflag == 32 ))  // Publisher/Pub.
				{
					pressFlag = sw;
				}
				
				if((tCNS->speflag == 90) // conference
					|| (tCNS->speflag == 79) // Journal
					|| (tCNS->speflag == 60)) // communications
				{
					confFlag = sw; // journal
				}
				
				if((tCNS->deptflag == 1)  // dept.
					||(tCNS->speflag == 56) // group
					||(tCNS->speflag == 75) //institute
					) 
				{
					groupFlag = sw; // institute
				}
				
				if(tCNS->procflag == 1) // In xxx
				{
					inStatus = sw;
				}
				
				if(tCNS->edsflag == 1  && edNoStop) edsFlag = sw;
				//if(tCNS->edsflag == 1 ) edsFlag = sw;
				
				if(tCNS->uniflag == 1) uniFlag = sw; // un of xxx in
									// un of ... press 
			
				if(tCNS->procflag > 1) // Proc.
				{
					procFlag = sw;
				}
				
				if(tCNS->speflag == 25) thesisFlag = sw;
				
				if(tCNS->speflag == 70) mitFlag = sw;
				
				if(tCNS->ltdflag) ltdFlag = sw;
				
				if((strcmp(tCNS->str,"and") == 0) && (i < 2)) 
					andFlag = sw;
			}
			if(stopEffect < 1) // 0
			{
			
			}
			
			////////////////////////////////////////////////////////////////////		

			
			// couple delimiter
			if(tCNS->pareEflag) paraCache++;
			if(tCNS->pareSflag)
			{
				paraFlag = paraFlag + 1 - paraCache;
				paraCache = 0;
			}
			if(tCNS->sqbEflag) sqbCache++;
			if(tCNS->sqbSflag)
			{
				sqbFlag = sqbFlag + 1 - sqbCache;
				sqbCache = 0;
			}
			if(tCNS->braEflag) braCache++;
			if(tCNS->braSflag)
			{
				braFlag = braFlag + 1 - braCache;
				braCache = 0;
			}
			
			if(i<3 && tCNS->sqbEflag)
			{
				seqFlag = 1;
			}
			
			
			////////////////END STOP//////////////////////
			if(tCNS->puredigit == 2 )
			{
				edNoStop = 0;
			}
			// [In] xxx
			if(tCNS->procflag == 1) edNoStop = 0;
		}

		pareStatus[0] = paraFlag > 0 ;
		sqbStatus[0] = sqbFlag > 0;
		braStatus[0] = braFlag > 0;

		
		
		
		// 0.2 SOME MIX FEATURE
		// \"
		quotTime--;
		
		if(quotTime <= 0)
		{
			quotStatus[0] = 0;
			quotTime = 0;
		}else
		{
			quotStatus[0] = 1;
		}
		quotStatus[1] = 0;
		quotStatus[2] = 0;
		if(pCNS->quotflag%2 == 1)
		{
			quotStatus[0] = !quotStatus[0];
			if(quotStatus[0])
				quotTime = 10;
			else
				quotTime = -10;
		}else if(pCNS->quotflag > 0 && pCNS->quotflag%2 == 0)
		{
			quotStatus[1] = quotStatus[2] = 1;
			quotTime = 0;
		}
		if(quotTime == 10) quotStatus[1] =  1;
		else if(quotTime == -10) quotStatus[2] = 1;
		//((quotTime==10||quotTime==-10)?(quotStatus?"IN":"OUT"):(quotStatus?"AT":"NA"))
		
		
		//////////////////////////////////////////////////////////////////////
		
		pareStatus[1] = pareStatus[2] = 0;
		// para
		if(pCNS->pareSflag == 1)
		{
			pareStatus[1] = 1;
		}
		if(pCNS->pareEflag == 1)
		{
			pareStatus[2] = 1;
		}
		
		//////////////////////////////////////////////////////////////////////
		// sqb
		sqbStatus[1] = sqbStatus[2] = 0;
		if(pCNS->sqbSflag == 1)
		{
			sqbStatus[1] = 1;
		}
		
		if(pCNS->sqbEflag == 1)
		{
			sqbStatus[2] = 1;
		}
		
		
		//////////////////////////////////////////////////////////////////////
		// bra
		braStatus[1] = braStatus[2] = 0;
		if(pCNS->braSflag == 1)
		{
			braStatus[1] = 1;
		}
		
		if(pCNS->braEflag == 1)
		{
			braStatus[2] = 1;
		}
		
		// xxxx ( abcd )  or xxxx , (abcd)
		// in template
		int contentConnect = 0;
		if(pareStatus[0] == 1)
		{
			for(i=1;i < sizeQueue(&preCNSQ) ; i++)
			{
				pCrfNodeSnapshot tCNS = pastNElem(&preCNSQ,i);
				if(tCNS->predeli == '(')
				{
					tCNS = pastNElem(&preCNSQ,i+1);
					if(tCNS->nextdeli == '(') contentConnect = 1;
					break;
				}
			}
		}
		
		
		// combined string , combine with next string
		char combinedStr[1024];
		sprintf(combinedStr,"%s%s",pCNS->str,((npCNS==NULL)?"":npCNS->str));
		
		
		// 1. OUTPUT : PRINT FEATURES
		
		// basic
		// 0: string data
		sprintf(strpart,"%s\t",pCNS->str); 
		strcat(samplestr,strpart);
		
		// 1: length of string data 0,1,2,3,4,5,6 >6
		sprintf(strpart,"%d\t",pCNS->slen<7?pCNS->slen:9); 
		strcat(samplestr,strpart);

		
		// base::string
		// 2: string type 0:AAA 1:aaa 2:Aaa 3:aAa 4:123
		sprintf(strpart,"%d\t",pCNS->strtype);
		strcat(samplestr,strpart);
		
		// 3: prefix 
		sprintf(strpart,"%c/%c\t",tolower(pCNS->str[0]),(pCNS->slen>1)?tolower(pCNS->str[1]):'X');
		strcat(samplestr,strpart);
		// 4: sufix 
		sprintf(strpart,"%c/%c\t",(pCNS->slen>1)?tolower(pCNS->str[pCNS->slen-2]):'X',
					tolower(pCNS->str[pCNS->slen-1]));
		strcat(samplestr,strpart);
		
		// base::digit
		// 5: digit value  > 0 ?
		sprintf(strpart,"%d\t",pCNS->dval > 0 );
		strcat(samplestr,strpart);
		
		// 6: digit bigger than previours one
		sprintf(strpart,"%d\t",lpCNS==NULL?-1:(lpCNS->dval == 0?-1:(pCNS->dval > lpCNS->dval)));
		strcat(samplestr,strpart);
		
		// 7: next one is bigger than this digit 
		sprintf(strpart,"%d\t",npCNS==NULL?-1:(npCNS->dval == 0?-1:(npCNS->dval > pCNS->dval)));
		strcat(samplestr,strpart);
		
		// 8: digit a improve digit ? 123456 456 > 123
		sprintf(strpart,"%d\t",pCNS->imprnum );
		strcat(samplestr,strpart);
		
		// 9: is pure digit ? see 'I' 'l' 'O' etc. as digit
		sprintf(strpart,"%d\t",pCNS->puredigit);
		strcat(samplestr,strpart);
		
		
		// base::delimiter
		// 10: last delimiter
		sprintf(strpart,"%d\t",pCNS->predeli);
		strcat(samplestr,strpart);
		
		// 11: last useful delimiter
		sprintf(strpart,"%d\t",pCNS->mpredeli);
		strcat(samplestr,strpart);
		
		// 12: next delimiter
		sprintf(strpart,"%d\t",pCNS->nextdeli);
		strcat(samplestr,strpart);
		
		
		
		// base::string orthographic
		// 13: year like || month like >> time like
		sprintf(strpart,"%d\t",(pCNS->yearlike > 0 )|| (pCNS->monthlike > 0 ));
		strcat(samplestr,strpart);
		
		// 14: volume like  start of volume ?  vol. X num. no. number
		if(pCNS->volumnlike < 3) sprintf(strpart,"%d\t",pCNS->volumnlike);
		else if(!isBlank(pCNS->nextdeli)) sprintf(strpart,"%d\t",pCNS->volumnlike-2);
		else sprintf(strpart,"0\t");
		strcat(samplestr,strpart);
		
		// 15: page like
		sprintf(strpart,"%d\t",pCNS->pagelike);
		strcat(samplestr,strpart);
		
		// base::dict
		// 16: name in dict
		sprintf(strpart,"%d\t",pCNS->isNameDict || pCNS->rLastNameDict > 0);
		strcat(samplestr,strpart);
		
		// 17: place in dict
		pCNS->isPlaceNameDict = pCNS->isPlaceNameDict || isPlaceNameInDict(combinedStr);		
		pCNS->isCountryDict = pCNS->isCountryDict || isCountryInDict(combinedStr);
		
		sprintf(strpart,"%d\t",pCNS->isPlaceNameDict>0 || pCNS->isCountryDict > 0);
		strcat(samplestr,strpart);
		
		
		// 18: publisher in dict
		pCNS->isPubliserDict = pCNS->isPubliserDict || isPublisherInDict(combinedStr);
		sprintf(strpart,"%d\t",pCNS->isPubliserDict);
		strcat(samplestr,strpart);
		
		// 19: fun word in dict 
		sprintf(strpart,"%d\t",pCNS->isFunWordDict);
		strcat(samplestr,strpart);
		
		// base::couple flag ststus
		
		// 20,21,22 quots AT IN OUT
		sprintf(strpart,"%d\t",quotStatus[0]);
		strcat(samplestr,strpart);
		sprintf(strpart,"%d\t",quotStatus[1]);
		strcat(samplestr,strpart);
		sprintf(strpart,"%d\t",quotStatus[2]);
		strcat(samplestr,strpart);
		
		
		// 23,24,25 Parentheses AT IN OUT
		sprintf(strpart,"%d\t",pareStatus[0]);
		strcat(samplestr,strpart);
		sprintf(strpart,"%d\t",pareStatus[1]);
		strcat(samplestr,strpart);
		sprintf(strpart,"%d\t",pareStatus[2]);
		strcat(samplestr,strpart);
		
		
		// 26,27,28 Square brackets AT IN OUT
		sprintf(strpart,"%d\t",sqbStatus[0]);
		strcat(samplestr,strpart);
		sprintf(strpart,"%d\t",sqbStatus[1]);
		strcat(samplestr,strpart);
		sprintf(strpart,"%d\t",sqbStatus[2]);
		strcat(samplestr,strpart);
		
		// 29,30,31 Braces AT IN OUT
		sprintf(strpart,"%d\t",braStatus[0]);
		strcat(samplestr,strpart);
		sprintf(strpart,"%d\t",braStatus[1]);
		strcat(samplestr,strpart);
		sprintf(strpart,"%d\t",braStatus[2]);
		strcat(samplestr,strpart);
		
		// base::flags
		
		// 32 basic flags
		// special flag (mixed)
		sprintf(strpart,"%d\t",pCNS->speflag);
		strcat(samplestr,strpart);
		
		// 33 stop flag  && effect
		sprintf(strpart,"%d/%d/%d/%d\t",pCNS->stopflag,
					(quotStatus[0]||pareStatus[0]||sqbStatus[0]||braStatus[0]),
					(quotStatus[1]||pareStatus[1]||sqbStatus[1]||braStatus[1]),
					(quotStatus[2]||pareStatus[2]||sqbStatus[2]||braStatus[2]));
		strcat(samplestr,strpart);
		
		// 34 eds flag
		sprintf(strpart,"%d\t",edsFlag);
		strcat(samplestr,strpart);
		
		// 35: name like
		sprintf(strpart,"%d\t",pCNS->namelike);
		strcat(samplestr,strpart);
		
		
		// extend::flags effect
		// 36 number of next pure digit
		sprintf(strpart,"%d\t",nextPDigit);
		strcat(samplestr,strpart);
		
		
		// 37 http effect  domain
		sprintf(strpart,"%d/%d\t",httpStatus,domainFlag);
		strcat(samplestr,strpart);
		
		// extend::mix effect
		// 38 [abc def] author @ abc
		if(pCNS->mpredeli == '[' && npCNS != NULL)
		{
			if(npCNS->nextdeli == ']') sprintf(strpart,"1\t");
			else sprintf(strpart,"0\t");
		}else
			sprintf(strpart,"0\t");
		strcat(samplestr,strpart);
		
		// [abc def] author @ def
		// 39
		if(pCNS->nextdeli == ']' && seqFlag == 1)
		{
			sprintf(strpart,"1\t");
		}else
			sprintf(strpart,"0\t");
		strcat(samplestr,strpart);
		

		// 40 article xxxx, A process of ...
		sprintf(strpart,"%d/%d\t",pCNS->isArticle,pCNS->stopflag);
		strcat(samplestr,strpart);
		
		
		// 41 tech
		int phdflag = 0;
		if((strcasecmp("ph",pCNS->str)== 0 && npCNS->str[0] == 'D')
			|| strcasecmp("phD",pCNS->str)== 0)
			phdflag = 1;
		else
			phdflag = 0;

		sprintf(strpart,"%d\t",phdflag);
		strcat(samplestr,strpart);
		
		
		// 42 xxx thesis thesis : 25
		sprintf(strpart,"%d\t",thesisFlag);
		strcat(samplestr,strpart);
		
		// 43 inc ltd limited  : ltdflag 1 2 3
		sprintf(strpart,"%d\t",ltdFlag);
		strcat(samplestr,strpart);
		
		// 44 45 46 ACM / ICPC / IEEE
		sprintf(strpart,"%d\t",pCNS->speflag == 1); // ISO
		strcat(samplestr,strpart);
		sprintf(strpart,"%d\t",pCNS->speflag == 2); // IEEE
		strcat(samplestr,strpart);
		sprintf(strpart,"%d\t",pCNS->speflag == 3); // ACM
		strcat(samplestr,strpart);
		
		// 47 CNAC AECSA SRCD ... // contain C
		//sprintf(strpart,"%d\t",pCNS->strtype == 0 &&  pCNS->slen < 6 && pCNS->slen > 2);
		sprintf(strpart,"%d\t",conferencelike(pCNS->str,pCNS->slen));
		strcat(samplestr,strpart);
		
		// 48 technical report
		sprintf(strpart,"%d\t",techFlag || phdflag || repFlag);
		strcat(samplestr,strpart);
		//sprintf(strpart,"%d/\t",repFlag);
		
		
		// 49 MIT
		sprintf(strpart,"%d\t",mitFlag);
		strcat(samplestr,strpart);
		
		// 50 51 52 university of 
		sprintf(strpart,"%d\t",pCNS->uniflag);
		strcat(samplestr,strpart);
		
		sprintf(strpart,"%d\t",uniFlag);
		strcat(samplestr,strpart);
		
		
		//sprintf(strpart,"%d/%d\t",pCNS->predeli,pCNS->uniflag);
		sprintf(strpart,"%d/%d\t",lpCNS == NULL ? 3 : lpCNS->stopflag,pCNS->uniflag);
		strcat(samplestr,strpart);
		
		
		// (August 1-2 2013)
		// how about ?
		
		// 53 xxxx , (adfadf) or xxxx (adfasf)
		sprintf(strpart,"%d\t",contentConnect);
		strcat(samplestr,strpart);
		
		// rfc || request (in tr) 
		//sprintf(strpart,"%d\t",rfc);
		
		// 54 55 56 57 58 59 in proceedings of
		// 54
		sprintf(strpart,"%d\t",pCNS->procflag == 1); // In point
		strcat(samplestr,strpart);
		// 55
		sprintf(strpart,"%d\t",(pCNS->procflag == 1) && 
				((npCNS->namelike)||(npCNS->isNameDict )
					|| (npCNS->rLastNameDict > 0)));
		strcat(samplestr,strpart);
		
		// 56
		sprintf(strpart,"%d\t",inStatus);
		strcat(samplestr,strpart);
		
		// 57
		sprintf(strpart,"%d\t",inStatus && ((pCNS->namelike)||(pCNS->isNameDict ) || (pCNS->rLastNameDict > 0)));
		strcat(samplestr,strpart);
		
		// 58
		sprintf(strpart,"%d\t",(pCNS->procflag == 1) || procFlag);
		strcat(samplestr,strpart);
		
		// 59
		sprintf(strpart,"%d\t",procFlag);
		strcat(samplestr,strpart);
		
		// 60  department of / dept. of
		sprintf(strpart,"%d\t",pCNS->deptflag);
		strcat(samplestr,strpart);
		
		// 61 press
		sprintf(strpart,"%d\t",pressFlag);
		strcat(samplestr,strpart);
		
		
		// 62 conf / journal  
		sprintf(strpart,"%d\t",confFlag);
		strcat(samplestr,strpart);

		
		// 63 org
		sprintf(strpart,"%d\t",orgFlag || labFlag);
		strcat(samplestr,strpart);
		
		// 64 group
		sprintf(strpart,"%d\t",groupFlag);
		strcat(samplestr,strpart);
		
		// 65 lib ins/pub
		sprintf(strpart,"%d\t",labFlag);
		strcat(samplestr,strpart);
		
		// 66 isbn
		sprintf(strpart,"%d\t",isbnEffect);
		strcat(samplestr,strpart);
		
		// 67 group lab or dept
		sprintf(strpart,"%d\t",labFlag > 0 || groupFlag > 0 || uniFlag > 0);
		strcat(samplestr,strpart);
		
		
		// 68 eds point
		sprintf(strpart,"%d/%d\t",pCNS->edsflag,pCNS->nextdeli);// 
		strcat(samplestr,strpart);
		
		
		// 69
		sprintf(strpart,"%d/%d\t",pCNS->isPubliserDict,
				pressFlag &&(labFlag > 0 || groupFlag > 0 || uniFlag > 0) ); 
				// university , 
		strcat(samplestr,strpart);
		
		// 70 et , al
		if(lpCNS != NULL && npCNS != NULL)
		{
			if((strcmp(lpCNS->str,"et")==0) && (strcmp(pCNS->str,"al")==0))
				sprintf(strpart,"1\t");
			else if((strcmp(pCNS->str,"et")==0) && (strcmp(npCNS->str,"al")==0))
				sprintf(strpart,"2\t");
			else 
				sprintf(strpart,"0\t");
		}else
			sprintf(strpart,"0\t");
		strcat(samplestr,strpart);
		
		
		// 71
		// jump to note : Available ... Submitted Submitted to 
		//Unpublished Published as
		if(lpCNS != NULL && npCNS != NULL)
		{
			if ( (strcmp(pCNS->str,"Available") == 0)
				|| ((strcmp(pCNS->str,"Submitted")== 0)&&(strcmp(npCNS->str,"to")))
				|| (strcmp(pCNS->str,"Unpublished")== 0)
				|| (strcmp(pCNS->str,"Published")== 0)
				|| ((strcmp(pCNS->str,"To")== 0) 
					&& (strcmp(npCNS->str,"appear")== 0))
				)
			{
				sprintf(strpart,"1\t");
			}else
				sprintf(strpart,"0\t");
			
		}else
			sprintf(strpart,"0\t");
		// note start
		strcat(samplestr,strpart);
		
		
		// 72 and
		sprintf(strpart,"%d\t",andFlag);
		strcat(samplestr,strpart);
	
		// 73 endSign
		sprintf(strpart,"%d\t",pCNS->stopflag);
		strcat(samplestr,strpart);
		
		// add tr
		tagger->add(samplestr);

		
		//enQueue && store
		enQueueWithDrop(&preCNSQ,*pCNS);
		predftEnQueue(&nextCNSQ,&currentOffset,&mpredeli,refAreaEnd);
	}
	
	////////////////////////////////////////////////////////////////////////////
	if (! tagger->parse()) return 0; // parse
	// end input features, output result
	pCitationNode nodeHead = NULL;
	pCitationNode nowNode;
	nowNode = addCitationNode(&nodeHead);
	int nowid = 0;
	int lastid;
	char strelem[1024];
	char *content = getPcontent();
	int partlen;
	lastid = token2Id(tagger->y2(0));
	memset(strelem,0,1024);
	partlen = 0;
	for (size_t i = 1; i <= tagger->size(); i++ ) {
		nowid = token2Id(tagger->y2(i));
		offsetCp = offsetCpQueue.front();
		content = getPcontent()+offsetCp.startOffset;
		memcpy(strelem+partlen,
			content,
			offsetCp.endOffset - offsetCp.startOffset);
		partlen += offsetCp.endOffset - offsetCp.startOffset;
		if(nowid != lastid)
		{
			//addCitationInfo(pCitationNode *node,const char *str,int len,int id)
			addCitationInfo(&nowNode,strelem,partlen,lastid);
			memset(strelem,0,1024);
			partlen = 0;
		}
		offsetCpQueue.pop();
		lastid = nowid;
	}
	addCitationInfo(&nowNode,strelem,partlen,nowid);
	delete tagger;
	return nodeHead;
}




pCitationNode CitationInfoPredictFile(const char *fileName,int startOffset,int endOffset)
{
	pCitationNode p;
	initContent();
	if(!DEBUGFLAG) setNoParse(1); //for release
	if(!parseFile(fileName)) return NULL;
	p = CitationInfoPredict(startOffset,endOffset);
	cleanContent();
	return p;
}

pCitationNode CitationInfoPredictString(const char *str,int startOffset,int endOffset)
{
	pCitationNode p;
	//dict prepare
	initContent();
	if(!DEBUGFLAG) setNoParse(1); //for release	
	setFileContent(str);
	p = CitationInfoPredict(startOffset,endOffset);
	cleanContent();
	//dict free
	return p;	
}


// pre and post

void initCitationInfoPredict()
{
	dbConnect();
	hmDictLoad();
	dbFree();
}

void cleanCitationInfoPredict()
{
	hmDictFree();
}



