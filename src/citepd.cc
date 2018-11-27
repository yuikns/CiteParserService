#include "argcvnw.h"
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h> // for read
#include <string.h>
#include <stdlib.h> // free


#include <time.h>

#include <syslog.h> // syslog

#include <sys/wait.h> // wait

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
 
#include <sys/types.h>
#include <unistd.h>

#include <vector>
#include <string>

#include "cjson.h"

// parser service start
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
// parser service end


#define MAXLINE 1024
#define Q_SIZE_LIMIT 100000


using std::vector;
using std::string;

//static string query_base;
const static std::string end_flag("#MTFBWU#");

void cpsd(const char * port);
char * rpc_query(const char * query,size_t query_len);

char * rpc_query(const char * query,size_t query_len)
{
    //fprintf(stdout,"tohandle :Q[%s(%lu)]\n",query,query_len);
	//time_t ct;
	//time(&ct);
	//struct tm * ct_s = gmtime(&ct);
	//char time_buff[1024];
	//sprintf(time_buff,"%d %d,%d GMT",ct_s->tm_mon + 1,ct_s->tm_mday,ct_s->tm_year + 1900);
	
	pCitationNode pHead ;
	pHead = CitationInfoPredictString(query,0,query_len);
	//std::cout<<std::endl;
	
	//cJSON *status;
	cJSON *root,*result,*result_item;
	char * out;
	root=cJSON_CreateObject();
	//cJSON_AddItemToObject(root, "status", status=cJSON_CreateObject());
	cJSON_AddItemToObject(root, "ref", result=cJSON_CreateArray());
	
	size_t all_rst_size = 0;
	pCitationNode p = pHead;
	while(p != NULL)
	{
		all_rst_size ++ ;
		cJSON_AddItemToArray(result, result_item=cJSON_CreateObject());
		// authors
		if(p->author[0][0] != 0)
		{
			cJSON * authors;
			//cJSON * author;
			cJSON_AddItemToObject(result_item, "authors", authors=cJSON_CreateArray());
			for(int i = 0 ; i < 10 ; i ++)
			{
				if(p->author[i][0] != 0)
				{
					//cJSON_AddItemToArray(authors, author=cJSON_CreateString(p->author[i]));
					cJSON_AddItemToArray(authors,cJSON_CreateString(p->author[i]));
				}
			}
		}
		
		// 
		//if(p->title[0] != 0) std::cout << "title:\t" << p->title << std::endl;
		if(p->title[0] != 0)
			cJSON_AddItemToObject(result_item, "title", cJSON_CreateString(p->title));
		//if(p->journal[0] != 0) std::cout << "journal:\t" << p->journal << std::endl;
		if(p->journal[0] != 0)
			cJSON_AddItemToObject(result_item, "journal", cJSON_CreateString(p->journal));
		//if(p->booktitle[0] != 0) std::cout << "booktitle:\t" << p->booktitle << std::endl;
		if(p->booktitle[0] != 0)
			cJSON_AddItemToObject(result_item, "booktitle", cJSON_CreateString(p->booktitle));
		//if(p->publisher[0] != 0) std::cout << "publisher:\t" << p->publisher << std::endl;
		if(p->publisher[0] != 0)
			cJSON_AddItemToObject(result_item, "publisher", cJSON_CreateString(p->publisher));
		//if(p->institution[0] != 0) std::cout << "institution:\t" << p->institution << std::endl;
		if(p->institution[0] != 0)
			cJSON_AddItemToObject(result_item, "institution", cJSON_CreateString(p->institution));
		//if(p->volume[0] != 0) printf("volume:\t%s\n",p->volume);
		if(p->volume[0] != 0)
			cJSON_AddItemToObject(result_item, "volume", cJSON_CreateString(p->volume));
		//if(p->location[0] != 0) std::cout << "location:\t" << p->location << std::endl;
		if(p->location[0] != 0)
			cJSON_AddItemToObject(result_item, "location", cJSON_CreateString(p->location));
		//if(p->pages[0] != 0) std::cout << "pages:\t" << p->pages << std::endl;
		if(p->pages[0] != 0)
			cJSON_AddItemToObject(result_item, "pages", cJSON_CreateString(p->pages));
		//if(p->date[0] != 0) std::cout << "date:\t" << p->date << std::endl;
		if(p->date[0] != 0)
			cJSON_AddItemToObject(result_item, "date", cJSON_CreateString(p->date));
		//if(p->url[0] != 0) std::cout << "url:\t" << p->url << std::endl;
		if(p->url[0] != 0)
			cJSON_AddItemToObject(result_item, "url", cJSON_CreateString(p->url));
		//if(p->isbn[0] != 0) std::cout << "isbn:\t" << p->isbn << std::endl;
		if(p->isbn[0] != 0)
			cJSON_AddItemToObject(result_item, "isbn", cJSON_CreateString(p->isbn));
		//if(p->tech[0] != 0) std::cout << "tech:\t" << p->tech << std::endl;
		if(p->tech[0] != 0)
			cJSON_AddItemToObject(result_item, "tech", cJSON_CreateString(p->tech));
		
		if(p->editor[0][0] != 0)
		{
			cJSON * editors;
			cJSON_AddItemToObject(result_item, "editors", editors=cJSON_CreateArray());
			for(int i = 0 ; i < 10 ; i ++)
			{
				if(p->editor[i][0] != 0)
				{
					cJSON_AddItemToArray(editors,cJSON_CreateString(p->editor[i]));
				}
			}
		}
		
		p = p->next;		
	}
	
	//cJSON_AddStringToObject(status, "time_stamp",time_buff);
	//cJSON_AddNumberToObject(status, "cite_size",all_rst_size);
	
	out=cJSON_Print(root);	cJSON_Delete(root);

	cJSON_Minify(out);
	
	//printf("%s\n",out);
	//printCitation(p);
	freeCitationNode(&pHead);
	return out;
}




void cpsd(const char * port)
{
	int listenfd;
	int connfd;
	
	socklen_t len;
	//char buff[MAXLINE];
	//time_t ticks;
	struct sockaddr_storage cliaddr;
	
	// parser service init
	initCitationInfoPredict();
	// parser service inited
	
	if(daemon_proc)
	{
		syslog(LOG_NOTICE,"starting service");
	}else
	{
		fprintf(stdout,"starting service\n");
	}
	listenfd = tcp_listen(NULL,port,NULL);
	if(!listenfd)
	{
		if(daemon_proc)
		{
			syslog(LOG_ERR,"listen failed");
		}else
		{
			fprintf(stderr,"listen failed\n");
		}
		return ;
	}
	
	for(;;)
	{
		len = sizeof(cliaddr);
		if(daemon_proc)
		{
			syslog(LOG_NOTICE,"listening ... %s ",port);
		}else
		{
			fprintf(stdout,"listening ... %s \n",port);
		}
		connfd = accept(listenfd,(SA*)&cliaddr,&len);
		signal(SIGCHLD,sig_chld); // signal
		//signal(SIGTERM,sig_term); // signal
		if(fork() == 0)
		{
			if(daemon_proc)
			{
				syslog(LOG_NOTICE,"new connection ...");
			}else
			{
				fprintf(stdout,"new connection ...\n");
			}
			int n ;
			char recvline[MAXLINE+1];
			string query("");
			size_t query_offset = 0;
			while((n = read(connfd,recvline,MAXLINE)) > 0 )
			{
				recvline[n] = '\0';
				//end_flag
				query.append(recvline,n);
				if((query_offset = query.find(end_flag)) != std::string::npos) // to end
				    break;
				if(query.size() > Q_SIZE_LIMIT)
					break;
			}
			if(query_offset > 0 && query_offset != std::string::npos )
			{
				query = query.substr(0,query_offset);
		        if(daemon_proc)
				{
					if(query_offset < 50 )
					{
						syslog(LOG_NOTICE,"query : [%s(%lu)]",query.c_str(),query_offset);
					}else
					{
						syslog(LOG_NOTICE,"query : [%s...(%lu)]",query.substr(0,50).c_str(),query_offset);
					}
				}else
				{
					if(query_offset < 50 )
					{
						fprintf(stdout,"query : [%s(%lu)]\n",query.c_str(),query_offset);
					}else
					{
						fprintf(stdout,"query : [%s...(%lu)]\n",query.substr(0,50).c_str(),query_offset);
					}
				}
				//if(page < 0 ) page = 0;
				//if(size < 0 ) size = 0;
				//else if(size > 20 ) size = 20;
				char * mbuff = rpc_query(query.c_str(),query_offset);
				write(connfd,mbuff,strlen(mbuff));	
			
				free(mbuff);
			}
			
			close(connfd);
			_exit(0);
		}
		close(connfd);
	}
	
	// service clean
	cleanCitationInfoPredict();
	// 
}

int main(int argc, char * argv[])
{
	//char path[1024];
	//getcwd(path,1024);// /home/.local/bin/xxx
	
	//printf("base query : [%s] \n",query_base.c_str());
	
	//daemon_init("namegend");
	//int port = 9527;
	const char * d_port = "9529";
	if(argc >= 2)
	{
		int port = atoi(argv[1]);
		if(port != 0 ){
			cpsd(argv[1]);
		}else
		{
			cpsd(d_port);
		}
	}else
	{
		cpsd(d_port);
	}
	
	//daemon_stop();
	return 0;
}


