#define MSG_TIMEOUT     2
#define MAX_MSG         1024//最大消息数

typedef struct {
    int mid;
    int pid;
//    struct message *adt;
} message;
message msg_list[MAX_MSG];

unsigned int msg_head;
unsigned int msg_tail;

typedef struct {
    long jiffies;
    int type;
    long init_jiffies;
    int pid;
    struct user_timer *next;
} user_timer;

user_timer *timer_head;

// extern int sys_timer_create(long milliseconds, int type);
extern void post_message(int type);
// extern void sys_get_message(message *msg);