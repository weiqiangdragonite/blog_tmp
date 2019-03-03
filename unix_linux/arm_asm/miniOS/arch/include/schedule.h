

#ifndef SCHEDULE_H
#define SCHEDULE_H


#define TASK_SZ                 121     // 支持进程最大个数(0 ~ 120)
#define VALID_TASK_PID(x)       (((x) >= 1 && (x) <= 39) || \
                                 ((x) >= 48 && (x) <= 55) || \
                                 ((x) >= 64 && (x) < TASK_SZ))

// 进程状态
#define TASK_UNALLOCATE         -1      // 进程PCB未分配
#define TASK_RUNNING            0       // 进程正在运行或已经准备就绪
#define TASK_INTERRUPTIBLE      1       // 进程处于可中断等待状态
#define TASK_UNINTERRUPTIBLE    2       // 进程处于不可中断等待状态
#define TASK_ZOMBIE             3       // 进程处于僵死状态
#define TASK_STOPPED            4       // 进程已经停止
#define TASK_SLEEPING           5       // 进程进入睡眠状态

#define PID_OFFSET              0       // PID相对偏移位
#define STATE_OFFSET            4       // 状态相对偏移位
#define COUNT_OFFSET            8       // 时间片数相对偏移位
#define PRIORITY_OFFSET         16      // 优先级别相对偏移位
#define CONTENT_OFFSET          20      // 上下文保存数据区相对偏移位
#define NULL                    0


#define STACK_BASE          0x5FFFFFF0
#define SVC_STACK_START     STACK_BASE
#define SVC_STACK_SIZE      (32 * 1024)
#define ABT_STACK_START     (SVC_STACK_START - SVC_STACK_SIZE)
#define ABT_STACK_SIZE      (4 * 1024)
#define UND_STACK_START     (ABT_STACK_START - ABT_STACK_SIZE)
#define UND_STACK_SIZE      (4 * 1024)
#define IRQ_STACK_START     (UND_STACK_START - UND_STACK_SIZE)
#define IRQ_STACK_SIZE      (32 * 1024)
#define FIQ_STACK_START     (IRQ_STACK_START - IRQ_STACK_SIZE)
#define FIQ_STACK_SIZE      (16 * 1024)
#define SYS_STACK_START     (FIQ_STACK_START - FIQ_STACK_SIZE)
#define USR_STACK_START     SYS_STACK_START


// PCB struct
typedef struct task_struct {
    unsigned int    pid;                // 进程ID
    unsigned int    state;              // 进程状态
    unsigned int    count;              // 进程时间片数
    unsigned int    timer;              // 进程休眠时间
    unsigned int    priority;           // 进程默认优先级
    unsigned int    content[20];        // 进程执行现场保存区
    /*
	 * content[0]:    保存用户进程状态寄存器CPSR的值
     * content[1]:    保存用户进程SP栈指针寄存器的值
	 * content[2]:    保存用户进程LR返回地址寄存器的值
	 * content[3~15]: 保存R0~R12寄存器的值
	 * content[16]:   保存PC程序计数器寄存器的值
	 */
} PCB;


// 进程队列数组
extern PCB task[TASK_SZ];

// 当前进程结构体指针
extern PCB *current;

// 检查当前CPU工作模式，如果是用户模式返回0
int check_cpu_mode(void);

// 定时器处理
//void do_timer(void);
void round_robin(void);

// 进程调度
void schedule(void);

// 杀死进程
//void kill_task(int pid);

// 进程调度初始化
void init_schedule(void);

// 创建进程
int create_process(char *parameters);

extern void copy_program(unsigned int *dest);
extern void switch_process(PCB *pcur, PCB *pnext);


#endif  // SCHEDULE_H