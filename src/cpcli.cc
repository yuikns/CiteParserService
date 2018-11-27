#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h> // for read

#include "argcvnw.h"
#include <syslog.h>
#include <time.h>
#include "argcvhttp.h"

#include <string>

#define host "127.0.0.1"
#define port "9529"

std::string file_read()
{
    FILE *fp = fopen("../test/f.txt","rb");
    size_t sz;
    char *buff;
    fseek(fp, 0, SEEK_END);  
    sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    buff = (char *)malloc(sizeof(char)*(sz+1));
    buff[sz] = '\0';
    fread(buff,sz,1,fp); 
    std::string rtstr(buff,sz);
    free(buff);
    fclose(fp);
    return rtstr;
}

char * cpsd_cli()
{
	int sockfd,n;
	const size_t MAXLINE = 1024;
	char recvline[MAXLINE + 1];
	socklen_t len;
	struct sockaddr_storage ss;
	
	//char host[255];
	//char port[10];
	
	sockfd = tcp_connect(host ,port);
	if(sockfd == 0)
	{
		return NULL;
	}

	len = sizeof(ss);
	getpeername(sockfd,(SA *)&ss , &len);

	fprintf(stdout,"connected to %s : %s\n",host ,port);
	
	std::string request_buff(file_read());
	request_buff.append("#MTFBWU#");
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
	char * content = cpsd_cli();
	printf("Result\n%s\n",content == NULL ? "ERROR" : content);
	free(content);
	return 0;
}

