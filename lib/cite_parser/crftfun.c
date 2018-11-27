#include "crftfun.h"
#include "minEditDistance.h"
#include "strHandle.h"
#include "queue.h"
#include "dict.h"
#include "tokens.h"
#include "strHandle.h"
#include "virtualcontent.h"
#include "hftnpse.h"
#include "common.h"
#include "crftctl.h"
#include "hftctl.h" // int getReferenceHeadOffset(void);
#include "eftfun.h" // int getReferenceEndOffset(void);
#include <string.h>
#ifndef WIN32
#include <strings.h>
#endif // WIN32


// digits ..
// digit base
int digitlen(const char *str,int len)
{
	int i;
	int sum = 0;
	for(i=0;i<len;i++)
	{
		if(DIGITLIKE(str[i]))
		{
			sum++;
		}
	}
	return sum;
}


int puredigit(const char *str,int len)
{
	int i;
	int sum = 0;
	for(i=0;i<len;i++)
	{
		if(DIGITLIKE(str[i]))
		{
			sum++;
		}
	}
	if(sum == 0 ) return 0;
	if(sum < len ) return 1;
	return 2;
}

int valofdigit(const char *str,int len)
{
	int i;
	int sum = 0;
	for(i=0;i<len;i++)
	{
		if(VALDIG(str[i]) == -1)
		{
			if(len - i <= 2)
			{
				return sum;
			}
			return 0;
		}
		sum = sum * 10 + VALDIG(str[i]);
	}

	return sum;
}

int twovalue(const char *str,int len,int *a,int *b)
{
	int i;
	int sum = 0;
	int num = 1;
	for(i=0;i<len;i++)
	{
		if(VALDIG(str[i]) == -1)
		{
			if((num == 1)&&(sum>0))
			{
				num = 2;
				*a = sum;
				sum = 0;
			}
		}else
		{
			sum = sum * 10 + VALDIG(str[i]);
		}
	}
	if(sum == 0 ) num = 1;
	else *b = sum;
	return num;
}

// digit special
int yearlike(const char *str,int len)
{
	if((str[0]=='1' && str[1]=='9')||(str[0]=='2' && str[1]=='0'))
	{
		if(DIGITLIKE(str[2]) && DIGITLIKE(str[3])) return 1;
	}
	return 0;
}





// strings ..
// str base

// 纯大写:0?
// 纯小写:1?
// 仅首字母大写:2?
// 混杂:3?
// 纯数字4?
int strfeature(const char *str,int len) 
{
	int hc = 0;
	int lc = 0;
	int i;
	int fl = (str[0] <='Z' && str[0] >='A')?1:0;//首字母大写
	if(fl) hc++;
	else lc ++;
	
	for(i=1;i<len;i++)
	{
		if(str[0] <='Z' && str[0] >='A')
		{
			hc++;
			fl = 0;
		}
		if(str[0] <='z' && str[0] >='a')
		{
			lc++;
		}
	}
	
	if(fl) return 2;
	
	if(hc > 0)
	{
		if(lc > 0 ) return 3;
		else return 0;
	}else // hc == 0
	{
		if(lc > 0) return 1;
		else return 4;
	}
	
}


// str special
int monthlike(const char *str,int len)
{
	int j;
	char kwds[12][10]={"January","February","March","April","May","June","July",
			"August","September","October","November","December"};
	for(j=0;j<12;j++)
	{
		if(strncasecmp(str,kwds[j],strlen(kwds[j])) == 0) return 1;
		if((strncasecmp(kwds[j],str,3) == 0)) return 2;
	}
	if(strncasecmp("Sept",str,4) == 0) return 2;

	return 0;
}



int vollkwd(const char *str,int len)
{
	if(strcasecmp(str,"vol")==0) return 3;
	if(strcasecmp(str,"voI")==0) return 3;
	if(strcasecmp(str,"v")==0) return 3;
	if(strncasecmp(str,"volume",6)==0) return 1;
	
	if(strcasecmp(str,"number")==0) return 2;
	if(strcasecmp(str,"num")==0) return 2;
	if(strcasecmp(str,"No")==0) return 4;
	if(len == 1 && str[0] == 'n') return 4;
	
	if(strcasecmp(str,"article")==0) return 1;

	return 0;
}

int pagekwd(const char *str,int len)
{
	if(strncasecmp(str,"pp",2)==0) return 1;
	if(strcasecmp(str,"p")==0) return 1;
	if(strncasecmp(str,"page",4)==0) return 1;
	return 0;
}



int edsFlag(const char *str,int len)
{
	if(len < 2 ) return 0;
	if(strncasecmp(str,"ed",len)==0) return 1;
	if(strncasecmp(str,"cd",len)==0) return 1;
	if(len < 3 ) return 0;
	if(strncasecmp(str,"eds",len)==0) return 1;
	if(len < 6 ) return 0;
	if(strncasecmp(str,"editor",len)==0) return 1;
	if(strncasecmp(str,"editors",len)==0) return 1;
	return 0;
}

int procFlag(const char *str,int len)
{
	if(len < 2) return 0; 
	if(strncasecmp(str,"in",len)==0) return 1;
	/*
	if(len > 2) 
		if(str[1]<='z'&&str[1]>='a' && 
			str[2]<='Z' && str[2]>='A' && (strncasecmp(str,"in",2)==0))
				return 1;
	*/
	if(len < 4) return 0;
	if(strncasecmp(str,"proc",len)==0) return 2;
	if(strncasecmp(str,"proceeding",len)==0) return 3;
	if(strncasecmp(str,"proceedings",len)==0) return 4;
	return 0;
}

int specialFlag(const char *str,int len)
{
	// Journal
	if(strncasecmp(str,"ISO",3)==0) return 1; // ISO/XXX TECH
	if(strncasecmp(str,"IEEE",4)==0) return 2;
	if(strncasecmp(str,"ACM",3)==0) return 3;
	
	// pages
	if(strncasecmp(str,"pp",2)==0) return 10;
	if(strcasecmp(str,"p")==0) return 11;
	if(strcasecmp(str,"pages")==0) return 12;
	if(strcasecmp(str,"page")==0) return 13;

	// url
	if(strncasecmp(str,"http",4)==0) return 20;
	if(strncasecmp(str,"ftp",4)==0) return 21;
	if(strncasecmp(str,"www",4)==0) return 22;

	
	//if(strncasecmp(str,"SIGCSE",6)==0) return 9;
	//if(strncasecmp(str,"Bulletin",8)==0) return 10;
	
	// publisher 
	if(strncasecmp(str,"Press",5)==0) return 30;
	if(strncasecmp(str,"Publishing",10)==0) return 31;
	if(strncasecmp(str,"Publisher",9)==0) return 32;
	if(strcmp(str,"Pub")==0) return 32;
	


	// isbn	
	if(strncasecmp(str,"ISBN",11)==0) return 40;
	
	
	// tech
	if(strncasecmp(str,"technical",9) == 0 ) return 50;
	if(strcasecmp(str,"tech") == 0 ) return 50;
	
	if(strncasecmp(str,"request",9) == 0 ) return 52;
	if(strcasecmp(str,"rfc") == 0 ) return 53;
	if(strcmp(str,"TR") == 0 ) return 54; // tech tech report
	if(strncasecmp(str,"report",6) == 0 ) return 55; // technical report
	if(strcasecmp(str,"rept") == 0 ) return 55;
	if(strcasecmp(str,"rpt") == 0 ) return 55;
	
	if(strcasecmp(str,"Thesis") == 0 ) return 25;
	
	if(strncasecmp(str,"group",5) == 0 ) return 56; // INSTITUTION xxx group


	// INSTITUTION
	if(strncasecmp(str,"research",9) == 0 ) return 51;
	if(strncasecmp(str,"communications",10) == 0 ) return 60;

	
	// tech
	
	
	if(strncasecmp(str,"MIT",3)==0) return 70;
	
	
	//institute
	if(strncasecmp(str,"lab",3)==0) return 71;
	if(strcasecmp(str,"Library") == 0 ) return 72;
	if(strcasecmp(str,"Laboratory")==0) return 73;
	if(strcasecmp(str,"Center")==0) return 74;
	
	if(strcasecmp(str,"institute")==0) return 75;
	
	if(strcasecmp(str,"association")==0) return 76;
	
	
	if(strcasecmp(str,"Corporation")==0) return 77;
	if(strcmp(str,"Co")==0) return 77;
	if(strcasecmp(str,"Organiza­tion")==0) return 78;
	
	if(strncasecmp(str,"Journal",7)==0) return 79;

	if(strncasecmp(str,"conference",10)==0) return 90;
	if(strcasecmp(str,"conf")==0) return 90;
	
	//
	if(strcasecmp(str,"of") == 0 ) return 80;
	
	
	
	//Comp
	//Corporation
	return 0;
}

int deptFlag(const char *str,int len)
{
	
	if(strncmp(str,"Dept",len)==0) return 2;
	if(strncasecmp(str,"Department",len)==0) return 1;
	return 0;
}


int uniFlag(const char *str,int len)
{
	if(strncasecmp(str,"universit",9)==0) return 1;
	if(strncasecmp(str,"univ",len)==0) return 1;
	return 0;
}

int ltdFlag(const char *str,int len)
{
	if(strncasecmp(str,"ltd",3)==0) return 1;
	if(strncasecmp(str,"Itd",3)==0) return 1;
	if(strncasecmp(str,"inc",3)==0) return 2;
	if(strncasecmp(str,"lnc",3)==0) return 2;
	if(strcasecmp(str,"Limited")==0) return 3;
	return 0;
}


int isArticle(const char *str,int len)
{
	if(strncasecmp(str,"A",len)==0) return 1;
	if(strncasecmp(str,"AN",len)==0) return 1;
	if(strncasecmp(str,"The",len)==0) return 1;
	if(strncasecmp(str,"On",len)==0) return 2; // simial function
	return 0;
}

int domainFlag(const char *str,int len)
{
	if(strcmp(str,"net")==0) return 1;
	if(strcmp(str,"edu")==0) return 1;
	if(strcmp(str,"com")==0) return 1;
	if(strcmp(str,"uk")==0) return 1;
	if(strcmp(str,"ac")==0) return 1;
	if(strcmp(str,"html")==0) return 1;
	if(strcmp(str,"gov")==0) return 1;
	
	return 0;
}

int namelike(const char *str,int len,char next,int type)
{
	if((len==1) && (str[0]>='A' && str[0]<='Z') && (next=='.'||next==',')) return 1;
	if((len>1) && (type==2)) return 1;
	
	if(strcasecmp(str,"and") == 0) return 2;
	return 0;
}

int conferencelike(const char *str,int len)
{
	int i;
	int containC = 0;
	int WC = 0; // wrong character
	if(len < 3 || len > 6) return 0;
	for(i=0;i<len;i++)
	{
		if(str[i] == 'C') containC = 1;
		if(str[i]>'Z' || str[i] < 'A') WC ++ ;
		if(WC > 1) return 0;
	}
	return containC;
}



// enqueue and dequeue

int filteredTokenId(int offset)
{
	int mypclen = getPclen();
	unsigned int myTags;
	unsigned int nowTag;
	if(offset >= mypclen) return 0;
	myTags = *(getTags()+offset);
	int finalTag = 0;
	while((nowTag=tokenPop(&myTags)) > 0)
	{
		//ignored: 
		// note 10
		// RefA 1
		// SinRef 2
		// tech 13
		if(nowTag != 1 && nowTag != 2 && nowTag != 10 ) // ingore RefA , SinRef , note
		{
			finalTag =  nowTag;
		}	
	}		
	return finalTag;
}


int ftEnQueue(pCNSQ Q,int *currentOffset,char *mpredeli)
{
	if(isFullQueue(Q)) return 0;

	int refAreaEnd = getReferenceEndOffset();
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
			&nextpre)) != 0)
	{
		int slen = strlen(str);
		sprintf(crfNodeSnapshot.str,"%s",str);
		crfNodeSnapshot.slen = slen;
		//int dval;
		int tkcheck=0; // get offset of token
		int hstkcheck = 0;
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
			if(!isDelimiter(content[i]) && !hstkcheck)
			{
				tkcheck = i;
				//break;
				hstkcheck = 1;
			}
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

		crfNodeSnapshot.token = filteredTokenId(tkcheck);//offsum+(offset+1)/2
		
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


pCrfNodeSnapshot ftDeQueue(pCNSQ Q)
{
	if(isEmptyQueue(Q)) return NULL;
	return deQueue(Q);
}


