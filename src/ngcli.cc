#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h> // for read

#include "argcvnw.h"
#include <syslog.h>
#include <time.h>
#include "argcvhttp.h"

#include <string>


char * cnchk_read(int argc,char * argv[])
{
	int sockfd,n;
	const size_t MAXLINE = 1024;
	char recvline[MAXLINE + 1];
	socklen_t len;
	struct sockaddr_storage ss;
	
	//char host[255];
	//char port[10];
	
	sockfd = tcp_connect(argv[1] ,argv[2]);
	if(sockfd == 0)
	{
		return NULL;
	}

	len = sizeof(ss);
	getpeername(sockfd,(SA *)&ss , &len);

	fprintf(stdout,"connected to %s : %s\n",argv[1] ,argv[2]);
	
	std::string request_buff;
	for(int i = 3 ; i < argc ; i ++ )
	{
		request_buff.append(argv[i]).append("\n");
	}
	request_buff.append("\n");
	fprintf(stdout,"send query : \n%s",request_buff.c_str());

	size_t send_len  = write(sockfd,request_buff.c_str(),request_buff.length());
	

	printf("length : %d\n",(int)send_len);

	std::string rtstr;
	while((n = read(sockfd,recvline,MAXLINE)) > 0 )
	{
		recvline[n] = '\0';
		rtstr.append(recvline);
		printf(".");
		//printf("%s",recvline);
		//printf("\n######################################################################\n");
	}
	printf("received\n");
	close(sockfd);
	
	size_t str_size = rtstr.size();
	char * rt_str = (char * ) malloc((str_size + 1)* sizeof(char));
	rt_str[str_size] = '\0';
	memcpy(rt_str,rtstr.c_str(),str_size);
	return rt_str;
}


int main(int argc,char * argv[])
{
	if(argc < 5)
	{
		fprintf(stderr,"\tusage : %s <HOST> <PORT> <1 or 0 means full or not full>[name1 , name2 ....]\n",argv[0]);
		return -1;
	}
	char * content = cnchk_read(argc,argv);
	printf("Result\n%s\n",content == NULL ? "ERROR" : content);
	free(content);
	return 0;
}
