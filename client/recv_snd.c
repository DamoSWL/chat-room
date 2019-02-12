#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <client.h>
#include <string.h>

int mysend(int rws_fd,char *buf,int mylen)
{
	int i;
	int len;
	char buf_head[4]={0};
	char buf_snd[BUFMAX]={0};
	fgets(buf,mylen+1,stdin);
	len=strlen(buf);
	if('\n'==buf[len-1]){
		buf[len-1]='\0';
	}
	len=strlen(buf);
	snprintf(buf_head,4,"%d",len);
	for(i=0;i<4;i++){
		if('\0'==buf_head[i]){
			buf_head[i]=' ';
		}
	}
	snprintf(buf_snd,BUFMAX,"%s%s",buf_head,buf);
	len+=4;
	if(len!=send(rws_fd,buf_snd,len,MSG_NOSIGNAL)){
		perror("send:");
		return -1;
	}
	return 0;
}
int myrecv(int rws_fd,char *buf)
{
	int len;
	if(0>=recv(rws_fd,buf,4,0)){
		perror("recv:1");
		return -1;
	}
	len=atoi(buf);
	if(0>=recv(rws_fd,buf,len, 0)){
		perror("recv:2");
		return -1;
	}		
	return 0;
}
	
int recv_msg(int rws, My *my_info)
{
	char log_in[]="log in success";
	char link[]="link success";
	char buf[BUFMAX]={0};
	char *buf_name;
	int len;
	if(0>myrecv(rws,buf)){
		return -1;
	}
	printf("=============================\n");
	printf("%s\n",buf);
	if(0==strncmp(buf,log_in,strlen(buf))){
		my_info->state=ONLINE;

	}
	if(0==strncmp(buf,link,strlen(link))){
		buf_name=buf+13;
		len=strlen(buf_name);
		strncpy(my_info->counterpart,buf_name,len);
		printf("to %s\n",my_info->counterpart);
	}
	return 0;
}

int send_msg(int rws,My *my_info)
{
	char buf[BUFMAX]={0};
	int len;
	char buf_print[BUFMAX]="to ";
	char quit[]="quit";
	if(OFFLINE==my_info->state){
		if(0>mysend(rws,buf,6)){
			return -1;
		}
	}else if(ONLINE==my_info->state){	
		len=strlen(my_info->counterpart);
		if(0==len){
			if(0>mysend(rws,buf,99)){
				return -1;
			}
			if(0==strncmp(buf,quit,strlen(buf))){
				my_info->state=OFFLINE;
				return -1;
			}	
		}else{
			if(0>mysend(rws,buf,99)){
				return -1;
			}
			if(0==strncmp(buf,quit,strlen(buf))){
				memset(my_info->counterpart,0,BUFMAX);
				return 0;
			}	
			
			strncat(buf_print,my_info->counterpart,len);
			printf("%s",buf_print);
		}
	}	

	return 0;
}



