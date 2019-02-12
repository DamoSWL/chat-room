#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <server.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>


int chat(int epoll_fd,struct epoll_event * usr_event,myuser *usr)
{
	int i=1;
	char buf[BUFMAX]={0};
	char from[BUFMAX]="from ";
	char online[BUFMAX]={0};
	char buf_err[]="invalid usrname\n";
	char link[BUFMAX]="link success ";
	char * buf_name;
	int len;
	myuser *pusr=usr_event->data.ptr;
	int myfd=pusr->myfd;
	len=strlen(pusr->usrname);
	strncat(from,pusr->usrname,len);
	strncat(from,": ",2);
	if(0>myrecv(myfd,buf)){
		quit(epoll_fd,usr_event);
		return -1;
	}

	int counterfd=pusr->counterfd;
	if(NONE!=counterfd){
		if(0==strncmp(buf,"quit",4)){
			pusr->counterfd=NONE;
			return 0;
		}else{
			len=strlen(buf);
			strncat(from,buf,len);
			len=strlen(from);
			if(0>mysend(counterfd,from,len)){
				quit(epoll_fd,usr_event);
				return -1;
			}
		}
	}else{
		if(0==strncmp(buf,"quit",4)){
			quit(epoll_fd,usr_event);
			return 0;
		}else if(0==strncmp(buf,"list",4)){
			for(i=1;i<USRMAX;i++){
				if(ONLINE==usr[i].state){
					len=strlen(usr[i].usrname);
					memset(online,0,sizeof(online));
					strncpy(online,usr[i].usrname,len);
					strncat(online," online",8);
					len=strlen(online);
					if(0>mysend(myfd,online,len)){
						quit(epoll_fd,usr_event);
						return -1;
					}
				}
			}
		}else if(0==strncmp(buf,"link",4)){
			buf_name=buf+5;
			len=strlen(buf_name);
			for(i=1;i<USRMAX;i++){
				if(ONLINE==usr[i].state){
					if(0==strncmp(buf_name,usr[i].usrname,len)){
						pusr->counterfd=usr[i].myfd;
						len=strlen(usr[i].usrname);
						strncat(link,usr[i].usrname,len);
						len=strlen(link);
						
						if(0>mysend(myfd,link,len)){
							quit(epoll_fd,usr_event);
							return -1;
						}						
						return 0;
					}
				}				
			}
			len=strlen(buf_err);
			if(0>mysend(myfd,buf_err,len)){
				quit(epoll_fd,usr_event);
				return -1;
			}
		}else{}		
	}

	return 0;
}


int quit(int epoll_fd,struct epoll_event *usr_event)
{
	int myfd;
	myuser *pusr=usr_event->data.ptr;
	myfd=pusr->myfd;
	pusr->state=OFFLINE;
	pusr->myfd=NONE;
	pusr->counterfd=NONE;
	pusr->temp=0;
	epoll_ctl(epoll_fd,EPOLL_CTL_DEL,myfd,usr_event);
	close(myfd);
	return 0;
}
