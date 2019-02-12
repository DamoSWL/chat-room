#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>       
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <poll.h>
#include <client.h>

int main()
{
	signal(SIGPIPE,SIG_IGN);
	int rws=socket(AF_INET,SOCK_STREAM,0);
	if(0>rws){
		perror("socket:");
		return -1;
	}
	struct sockaddr_in ser_addr;
	memset(&ser_addr,0,sizeof(ser_addr));
	ser_addr.sin_family=AF_INET;
	ser_addr.sin_port=htons(8000);
	ser_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	if(0>connect(rws,(struct sockaddr *)&ser_addr,sizeof(ser_addr))){
		perror("connect:");
		return -1;
	}
	My my_info;
	memset(&my_info,0,sizeof(my_info));
	my_info.state=OFFLINE;
	
	struct pollfd mypoll[2];
	mypoll[0].fd=rws;
	mypoll[0].events=POLLIN;
	mypoll[0].revents=0;
	mypoll[1].fd=STDIN_FILENO;
	mypoll[1].events=POLLIN;
	mypoll[1].revents=0;
	
	while(1){
		int ret=poll(mypoll,2,-1);
		if(0>ret){
			perror("poll:");
			break;
		}
		if(POLLIN&mypoll[0].revents){
			if(0>recv_msg(rws,&my_info)){
				break;
			}
		}
		if(POLLIN&mypoll[1].revents){
			if(0>send_msg(rws,&my_info)){
				break;
			}
		}
	}
	close(rws);

	return 0;
}
