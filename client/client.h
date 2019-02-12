#define BUFMAX 256
#define OFFLINE -1
#define ONLINE 0

typedef struct {
	int state;
	char counterpart[BUFMAX];
}My;

int recv_msg(int rws, My *my_info);
int send_msg(int rws,My *my_info);
