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
#include <sys/epoll.h>
#include <server.h>

int main()
{
	signal(SIGPIPE,SIG_IGN);
	myuser usr[USRMAX];
	memset(usr,0,sizeof(usr));
	if(0>load_usr(usr)){
		return -1;
	}
	int listen_soc;
	struct sockaddr_in ser_addr={
		.sin_family=AF_INET,
		.sin_port=htons(8000),
		.sin_addr.s_addr=INADDR_ANY,
	};
	memset(ser_addr.sin_zero,0,8);
	listen_soc=init_soc(&ser_addr,10);
	if(0>listen_soc){
		printf("init sock fail\n");
		return -1;
	}
	usr[0].myfd=listen_soc;
	
	int ep_fd=epoll_create(1);
	if(0>ep_fd){
		perror("epoll create:");
		return -1;
	}
	
	struct epoll_event listen_event;
	memset(&listen_event,0,sizeof(listen_event));
	listen_event.events=EPOLLIN;
	listen_event.data.ptr=usr;
	epoll_ctl(ep_fd,EPOLL_CTL_ADD,listen_soc,&listen_event);
	
	struct epoll_event usr_event[USRMAX];
	memset(&usr_event,0,sizeof(usr_event));
	char buf[BUFMAX]="please input 1:new user 2:registered user";
	int len=strlen(buf);
	int rws;
	
	while(1)
	{
		int i;
		int e_num=epoll_wait(ep_fd,usr_event,USRMAX,-1);
		if(0>e_num){
			perror("epoll_wati:");
			break;
		}
		for(i=0;i<e_num;i++){
			myuser *pusr=usr_event[i].data.ptr;
			if(listen_soc==pusr->myfd){
				rws=accept(listen_soc,NULL,NULL);
				if(0>rws){
					perror("accept:");
					break;
				}
				if(0>mysend(rws,buf,len)){
					close(rws);	
				}
				if(0>add(ep_fd,rws,usr)){
					close(rws);
				}
			}
				
			else{
				if(-1==pusr->temp){
					
					init_log(ep_fd,&usr_event[i]);
				}
				if(1==pusr->temp){
					
					new_log(ep_fd,&usr_event[i]);
				}
				if(2==pusr->temp){
					
					reg_log(ep_fd,&usr_event[i],usr);
				}
				if(0==pusr->temp){
					if(0>chat(ep_fd,&usr_event[i],usr)){
						quit(ep_fd,&usr_event[i]);
					}
				}
			}
		}	
	
	}
	return 0;
}
