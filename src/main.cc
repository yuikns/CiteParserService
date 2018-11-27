#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "dict.h"
#include "strHandle.h"
#include "crftctl.h"
#include "virtualcontent.h"

#include "khash.h"
#include "queue.h"
#include "citpred.h"
#include <iostream>

#include "hftctl.h"
#include "eftfun.h"

void printCitation(pCitationNode p)
{
	if(p == NULL) return ;
	int i;
	std::cout << "------------------" << std::endl;
	for(i=0;i<10;i++)
	{
		if(p->author[i][0] != 0) std::cout << "author["<<(i+1)
				<<"]: \t" << p->author[i] << std::endl;
		else break;
	}
	
	if(p->title[0] != 0) std::cout << "title:\t" << p->title << std::endl;
	if(p->journal[0] != 0) std::cout << "journal:\t" << p->journal << std::endl;
	if(p->booktitle[0] != 0) std::cout << "booktitle:\t" << p->booktitle << std::endl;
	if(p->publisher[0] != 0) std::cout << "publisher:\t" << p->publisher << std::endl;
	if(p->institution[0] != 0) std::cout << "institution:\t" << p->institution << std::endl;
	if(p->volume[0] != 0) printf("volume:\t%s\n",p->volume);
	if(p->location[0] != 0) std::cout << "location:\t" << p->location << std::endl;
	if(p->pages[0] != 0) std::cout << "pages:\t" << p->pages << std::endl;
	if(p->date[0] != 0) std::cout << "date:\t" << p->date << std::endl;
	if(p->url[0] != 0) std::cout << "url:\t" << p->url << std::endl;
	if(p->isbn[0] != 0) std::cout << "isbn:\t" << p->isbn << std::endl;
	if(p->tech[0] != 0) std::cout << "tech:\t" << p->tech << std::endl;
	
	for(i=0;i<10;i++)
	{
		if(p->editor[i][0] != 0) std::cout << "editor("<<(i+1)
				<<"): \t" << p->editor[i] << std::endl;
		else break;
	}
	
	
	
	printCitation(p->next);
}

int main(int argc,char *argv[])
{
    /*
	dbConnect();
	hmDictLoad();
	dbFree();
	
	genCRFSample("data/");
	
	hmDictFree();
	*/
	//const char *testfile="data/fa/A case study of file system workload in a large-scale distributed environment.txt";
	//const char *testfile="3D gadgets for business process visualization&#8212;a case study.txt";
	//initContent();
	//parseFile(testfile);
	//int startoffset = getReferenceHeadOffset();
	//int endoffset = getReferenceEndOffset();
	//cleanContent();
	
	const char * testfile = "../test/f.txt";
	FILE * f = fopen(testfile,"rb");
	fseek(f, 0L, SEEK_END);
	size_t startoffset = 0;
	size_t endoffset = ftell(f);
	rewind(f);
	char * buff = (char * )malloc(sizeof(char) * endoffset );
	fread(buff,endoffset,sizeof(char),f);
	fclose(f);
	
	initCitationInfoPredict();
	
	for(int i = 0 ; i < 3 ; i ++ )
	{
	    pCitationNode p ;
        printf("test: %s %lu:%lu\n",testfile,startoffset,endoffset);
        fflush(NULL);
	    //p = CitationInfoPredictFile(testfile,startoffset,endoffset);
	    p = CitationInfoPredictString(buff,startoffset,endoffset);
	    //std::cout<<std::endl;
	    printCitation(p);
	    freeCitationNode(&p);
	    //getchar();
	}
	free(buff);
	cleanCitationInfoPredict();
	return 0;
}

/*
// TEST OF SPILIT CONTENT

*/

