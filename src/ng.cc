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


#define MAXLINE 1024


using std::vector;
using std::string;

static string query_base;


void ng_service(const char * port);
void ng_popen(const vector<string> & name_vect,string & result);

void ng_popen(const vector<string> & name_vect,string & result)
{
	string query(query_base);
	
	for (std::vector<string>::const_iterator it = name_vect.begin() ; it != name_vect.end(); it ++ )
	{
		//printf("add : %s\n",(*it).c_str());
		query.append("\"").append(*it).append("\" ");
	}
	//printf("final query : %s \n",query.c_str());
	FILE * stream = popen(query.c_str(),"r");
	//FILE * stream = popen("echo \"hello, world !\"","r");
	if(stream != NULL)
	{
		char buff[MAXLINE+1];
		result.assign("");
		size_t prsz;
		while((prsz = fread( buff, sizeof(char),MAXLINE, stream))> 0 )
		{
			//printf("well , add : %s \n",buff);
			result.append(buff,prsz);
			//printf("rst : %s \n",result.c_str());
		}
		pclose(stream);
	}else 
	{
		if(!daemon_proc) fprintf(stderr,"open pipe failed ... \n");
		else syslog(LOG_NOTICE,"open pipe failed ... \n");
	}
}




void ng_service(const char * port)
{
	int listenfd;
	int connfd;
	
	socklen_t len;
	//char buff[MAXLINE];
	//time_t ticks;
	struct sockaddr_storage cliaddr;
	
	if(!daemon_proc) fprintf(stdout,"starting service , listening : %s \n",port);
	else syslog(LOG_NOTICE,"starting service , listening : %s \n",port);
	listenfd = tcp_listen(NULL,port,NULL);
	if(!listenfd)
	{
		if(!daemon_proc) fprintf(stderr,"connect error ... ");
		else syslog(LOG_NOTICE,"connect error ... ");
		return ;
	}
	
	for(;;)
	{
		len = sizeof(cliaddr);
		if(!daemon_proc) fprintf(stdout,"listening ... \n");
		else syslog(LOG_NOTICE,"listening ... ");
		connfd = accept(listenfd,(SA*)&cliaddr,&len);
		signal(SIGCHLD,sig_chld); // signal
		//signal(SIGTERM,sig_term); // signal
		if(fork() == 0)
		{
			//if(!daemon_proc) fprintf(stdout,"new connection ...\n");
			//else syslog(LOG_NOTICE,"new connection ...");
			int n ;
			char recvline[MAXLINE];
			int i;
			vector<string> name_vect;
			string name;
			int cnt_size = 0;
			bool end_flag = false;
			while((n = read(connfd,recvline,MAXLINE)) > 0 )
			{
				recvline[n] = '\0';
				for(i = 0 ; i < n; i ++)
				{
					if(recvline[i] == '\0') continue;
					switch(recvline[i])
					{
					case '\r':
					case '\n':
						if(name.length() > 0 )
						{
							name_vect.push_back(name);
							name.assign("");
						}
						cnt_size ++ ;
						if(cnt_size >= 2)
							end_flag = true;
						break;
					case '\'':
						cnt_size = 0;
						name.append("\\\'");
						break;
					case '\"':
						cnt_size = 0;
						name.append("\\\"");
						break;
					default :
						cnt_size = 0;
						name.append(&recvline[i],1);
					}
				}
				if(end_flag) break;
			}
			//fprintf(stdout,"jump out \n");
			if(name.length() > 0 )
			{
				name_vect.push_back(name);
			}
			//printf("REQUEST:\n%s\n",recvline);
			//char buff[1024];
			// prepare return text
			
			//
			string mbuff ;
			ng_popen(name_vect,mbuff);
			//mbuff.append("\n");
			//printf("result : %s \n",mbuff.c_str());
			write(connfd,mbuff.c_str(),mbuff.length());	
			//write(connfd,mbuff,strlen(mbuff));
			//write(connfd,buff,strlen(buff));
			
			close(connfd);
			_exit(0);
		}
		close(connfd);
	}
}

int main(int argc, char * argv[])
{
	char path[1024];
	getcwd(path,1024);
	query_base.assign("java -jar ");
	query_base.append(path).append("/").append("ng.jar ");
	
	//printf("base query : [%s] \n",query_base.c_str());
	
	//daemon_init("namegend");
	//int port = 9527;
	const char * d_port = "9528";
	if(argc >= 2)
	{
		int port = atoi(argv[1]);
		if(port != 0 ){
			ng_service(argv[1]);
		}else
		{
			ng_service(d_port);
		}
	}else
	{
		ng_service(d_port);
	}
	
	//daemon_stop();
	return 0;
}


