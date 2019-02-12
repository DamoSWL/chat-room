#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <server.h>
#include <sys/epoll.h>
#include <string.h>
#include <unistd.h>

int init_log(int ep_fd,struct epoll_event * usr_event)
{
	char recv_buf[BUFMAX]={0};
	char buf[BUFMAX]="invalid input\nplease input 1:new user 2:registered user";
	char buf_name[]="please enter your usrname";
	int len;
	myuser *pusr=usr_event->data.ptr;
	int rws=pusr->myfd;
	if(0>myrecv(rws,recv_buf)){
		memset(pusr,0,sizeof(myuser));
		epoll_ctl(ep_fd,EPOLL_CTL_DEL,rws,usr_event);
		close(rws);
		return -1;
	}
	int opt=atoi(recv_buf);
	if(1==opt){
		pusr->temp=1;	
	}else if(2==opt){
		pusr->temp=2;
	}else{
		len=strlen(buf);
		if(0>mysend(rws,buf,len)){
			memset(pusr,0,sizeof(myuser));
			epoll_ctl(ep_fd,EPOLL_CTL_DEL,rws,usr_event);
			close(rws);	
			return -1;
		}
		return -1;
	}
	len=strlen(buf_name);
	if(0>mysend(rws,buf_name,len)){
		memset(pusr,0,sizeof(myuser));
		epoll_ctl(ep_fd,EPOLL_CTL_DEL,rws,usr_event);
		close(rws);	
		return -1;
	}
	
	return 0;
}

int new_log(int ep_fd,struct epoll_event * usr_event)
{
	char recv_buf[BUFMAX]={0};
	char log_in[]="log in success";
	myuser *pusr=usr_event->data.ptr;
	int rws=pusr->myfd;
	char buf_passwd[]="please enter your password(max 6 digits)";
	int len=strlen(pusr->usrname);
	if(0==len){
		if(0>myrecv(rws,recv_buf)){
			memset(pusr,0,sizeof(myuser));
			epoll_ctl(ep_fd,EPOLL_CTL_DEL,rws,usr_event);
			close(rws);
			return -1;
		}
		len=strlen(recv_buf);
		strncpy(pusr->usrname,recv_buf,len);
		len=strlen(buf_passwd);
		if(0>mysend(rws,buf_passwd,len)){
			memset(pusr,0,sizeof(myuser));
			epoll_ctl(ep_fd,EPOLL_CTL_DEL,rws,usr_event);
			close(rws);
			return -1;
		}
	}else{
		if(0>myrecv(rws,recv_buf)){
			epoll_ctl(ep_fd,EPOLL_CTL_DEL,rws,usr_event);
			close(rws);
			return -1;
		}
		pusr->password=atoi(recv_buf);
		pusr->state=ONLINE;
		pusr->counterfd=NONE;
		pusr->temp=0;
		
		FILE *fp;
		fp=fopen("usr_info","a");
		fputs(pusr->usrname,fp);
		fputc('\n',fp);
		fputs(recv_buf,fp);
		fputc('\n',fp);
		fclose(fp);
		
		len=strlen(log_in);
		if(0>mysend(rws,log_in,len)){
			memset(pusr,0,sizeof(myuser));
			epoll_ctl(ep_fd,EPOLL_CTL_DEL,rws,usr_event);
			close(rws);	
			return -1;
		}	
	}
	return 0;	
}

int reg_log(int ep_fd,struct epoll_event * usr_event,myuser *usr)
{
	int i;
	char recv_buf[BUFMAX]={0};
	char log_in[]="log in success";
	myuser *pusr=usr_event->data.ptr;
	int rws=pusr->myfd;
	char buf_name[]="please enter your usrname:";
	char buf_err_name[]="invalid usrname";
	char buf_passwd[]="please enter your password(max 6 digits):";
	char buf_err_passwd[]="wrong password";
	int len=strlen(pusr->usrname);
	if(0==len){
		if(0>=myrecv(rws,recv_buf)){
			epoll_ctl(ep_fd,EPOLL_CTL_DEL,rws,usr_event);
			close(rws);	
			return -1;
		}
		len=strlen(recv_buf);
		for(i=1;i<USRMAX;i++){
			if(0==strncmp(recv_buf,usr[i].usrname,len)){
				strncpy(pusr->usrname,recv_buf,len);
				len=strlen(buf_passwd);
				if(0>mysend(rws,buf_passwd,len)){
					memset(pusr,0,sizeof(myuser));
					epoll_ctl(ep_fd,EPOLL_CTL_DEL,rws,usr_event);
					close(rws);	
					return -1;
				}
				return 0;
			}
		}
		len=strlen(buf_err_name);
		if(0>mysend(rws,buf_err_name,len)){
			memset(pusr,0,sizeof(myuser));
			epoll_ctl(ep_fd,EPOLL_CTL_DEL,rws,usr_event);
			close(rws);	
			return -1;
		}
		len=strlen(buf_name);
		if(0>mysend(rws,buf_name,len)){
			memset(pusr,0,sizeof(myuser));
			epoll_ctl(ep_fd,EPOLL_CTL_DEL,rws,usr_event);
			close(rws);	
			return -1;
		}	
	}else{
		int passwd;
		if(0>=myrecv(rws,recv_buf)){
			epoll_ctl(ep_fd,EPOLL_CTL_DEL,rws,usr_event);
			close(rws);	
			return -1;
		}
		passwd=atoi(recv_buf);
		for(i=1;i<USRMAX;i++){
			if(0==strncmp(pusr->usrname,usr[i].usrname,len)){
				break;
			}
		}
		if(passwd==usr[i].password){
			usr[i].state=ONLINE;
			usr[i].myfd=rws;
			usr[i].counterfd=NONE;
			usr[i].temp=0;
			memset(pusr,0,sizeof(myuser));
			usr_event->data.ptr=usr+i;
			epoll_ctl(ep_fd,EPOLL_CTL_MOD,rws,usr_event);
			len=strlen(log_in);
			if(0>mysend(rws,log_in,len)){
				epoll_ctl(ep_fd,EPOLL_CTL_DEL,rws,usr_event);
				close(rws);	
				return -1;
			}	
		}else{
			len=strlen(buf_err_passwd);
			if(0>mysend(rws,buf_err_passwd,len)){
				memset(pusr,0,sizeof(myuser));
				epoll_ctl(ep_fd,EPOLL_CTL_DEL,rws,usr_event);
				close(rws);	
				return -1;
			}
		}
	}
	return 0;
}












