#define MAXSIZE 256
#define ONLINE 1
#define OFFLINE -1
#define USRMAX 100
#define BUFMAX 256	
#define NONE -1

typedef int passwd_t;
typedef int stat_t;
typedef struct {
	char usrname[MAXSIZE];
	passwd_t password;
	stat_t state;
	int myfd;
	int counterfd;
	int temp;
}myuser;
typedef struct epoll_event * epoll;


int mysend(int rws_fd,char *buf,int len);
int myrecv(int rws_fd,char *buf);
int init_soc(const struct sockaddr_in *addr,const int backlog);
int add(int epoll_fd,int rws,myuser *usr);
int load_usr(myuser *usr);
int init_log(int ep_fd,struct epoll_event *usr_event);
int new_log(int ep_fd,struct epoll_event *usr_event);
int reg_log(int ep_fd,struct epoll_event *usr_event,myuser *usr);
int chat(int epoll_fd,struct epoll_event *usr_event,myuser *usr);
int quit(int epoll_fd,struct epoll_event *usr_event);
	
	
	
