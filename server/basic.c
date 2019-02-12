#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <server.h>
#include <string.h>
 #include <sys/epoll.h>

int mysend(int rws_fd,char *buf,int len)
{
	int i;
	char buf_head[4]={0};
	char buf_snd[BUFMAX]={0};
	snprintf(buf_head,4,"%d",len);
	for(i=0;i<4;i++){
		if('\0'==buf_head[i]){
			buf_head[i]=' ';
		}
	}
	snprintf(buf_snd,BUFMAX,"%s%s",buf_head,buf);
	len+=4;
	if(len!=send(rws_fd,buf_snd,len, MSG_NOSIGNAL)){
		perror("send:");
		return -1;
	}
	return 0;
}

int myrecv(int rws_fd,char *buf)
{
	int len;
	if(0>=recv(rws_fd,buf,4,0)){
		perror("recv:3");
		return -1;
	}
	len=atoi(buf);
	if(0>recv(rws_fd,buf,len,MSG_DONTWAIT)){
		perror("recv:4");
		return -1;
	}		
	return 0;
}

int init_soc(const struct sockaddr_in *addr,const int backlog)
{
	int fd=socket(AF_INET,SOCK_STREAM,0);
	if(0>bind(fd,(struct sockaddr *)addr,sizeof(struct sockaddr_in))){
		perror("bind:");
		return -1;
	}
	if(0>listen(fd,backlog)){
		perror("listen:");
		return -1;
	}
	return fd;
}

int add(int epoll_fd,int rws,myuser *usr)
{
	int ret;
	int i;
	struct epoll_event event;
	for(i=1;i<USRMAX;i++){
		if(0==usr[i].state){
			usr[i].myfd=rws;
			usr[i].temp=-1;
			break;
		}
	}
	event.events=EPOLLIN;
	event.data.ptr=usr+i;
	
	ret=epoll_ctl(epoll_fd,EPOLL_CTL_ADD,rws,&event);
	if(0>ret){
		perror("epoll add:");
		return -1;
	}
	return 0;
}

int load_usr(myuser *usr)
{
	char buf[BUFMAX];
	int i=1;
	int num=1;
	int len;
	FILE * fp;
	fp=fopen("usr_info","r");
	if(NULL==fp){
		perror("fopen:");
		return -1;
	}

	while(1){
		if(1==(i%2)){
			if(NULL==fgets(buf,BUFMAX,fp)){
				return 1;
			}
			len=strlen(buf);
			if('\n'==buf[len-1]){
				buf[len-1]='\0';
			}
			len=strlen(buf);
			strncpy(usr[num].usrname,buf,len+1);
		}else{
			if(NULL==fgets(buf,BUFMAX,fp)){
				perror("fgets:");
				return 1;
			}
			usr[num].password=atoi(buf);
			usr[num].state=OFFLINE;
			usr[num].myfd=NONE;
			usr[num].counterfd=NONE;
			usr[num].temp=0;
			
		}
		if(0==i%2){
			num++;
		}
		i++;
	}
	fclose(fp);		
	return 0;
}












