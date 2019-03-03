

#include "schedule.h"
#include "uart.h"

// 进程队列数组
PCB task[TASK_SZ];

// 当前进程结构体指针
PCB *current = NULL;

void init_schedule(void)
{
    // ptr指向task首地址
    PCB *ptr = &task[0];
    
    // 循环为每个进程PCB初始化
    for (int i = 0; i < TASK_SZ; ++i, ++ptr) {
        // pid = -1，表示未分配pid
        ptr->pid = -1;
        // 设置初始进程状态为未分配状态
        ptr->state = TASK_UNALLOCATE;
        // 设置时间片计数为0，表示没有时间片
        ptr->count = 0;
        // 初始进程优先级为0
        ptr->priority = 0;
    }
    
    // 初始化0号进程，0号进程为内核进程
    ptr = &task[0];
    ptr->pid = 0;
    ptr->state = TASK_RUNNING;
    // 设置其时间片为5
    ptr->count = 5;
    // 设置优先级为5
    ptr->priority = 5;
    // 保存状态寄存器cpsr值，设为系统模式，开启IRQ/FIQ中断
    ptr->content[0] = 0x1F;
    // 设置当前进程栈指针
    ptr->content[1] = SYS_STACK_START;
    // 
    ptr->content[2] = 0;
    ptr->content[16] = 0;
    
    // 当前运行进程为0号进程
    current = &task[0];
    
    return;
}

void kill_task(int pid)
{
    if (pid == 0) return;
    
    for (int i = 0; i < TASK_SZ; ++i) {
        if (task[i].pid == pid) {
            task[i].pid = -1;
            task[i].state = TASK_UNALLOCATE;
            task[i].count = 0;
            task[i].priority = 0;
            break;
        }
    }
    
    // 杀死进程后，重新调度
	schedule();
    
    return;
}

void schedule(void)
{
    // max用来保存当前进程队列里最高优先级进程count
    // p_tsk用来保存当前进程PID副本
    int max = -1;                       // max初始值为-1，后面作判断
    int next = 0;                       // next保存最高优先级PID
    PCB *p_tsk = NULL;                  // 临时进程结构体指针
    
    // 进程调度循环
    while (1) {
        // 循环找出进程队列里，就绪状态最高优先级进程，也就是count值最大进程，
        // count越大说明其被执行时间越短，CPU需求越高，
        // 同时保存其PID(进程队列数组下标)到next里
        for (int i = 0; i < TASK_SZ; ++i) {
            if ((task[i].state == TASK_RUNNING) && (max < (int) task[i].count)) {
                max = (int) task[i].count;
                next = i;
            }
        }
        
        // 如果max为非0，跳出循环，说名选出了调度进程
        // 如果max为0，说明count值最大进程为0，说明全部进程分配时间片已执行完，
        // 需要重新分配，执行break后面for语句
        // 如果max为-1，说明没有就绪进程可被调度，退出循环，继续执行0进程
        if (max) break;                 // 选出新进程，跳出循环
        
        // max=0，进程队列中count值最大为0，全部进程时间片用尽，需重新分配
        for (int i = 0; i < TASK_SZ; ++i) {
            if (task[i].state == TASK_RUNNING) {
                // 时间片数为其默认优先级
                task[i].count = task[i].priority;
            }
        }
    }
    
    // 当前进程为选出进程，说明当前进程优先级还是最高，返回继续执行
    if (current == &task[next]) return;
    
    // 无效PID
    if (task[next].pid < 0) return;
    
    // 保存当前进程副本到p_tsk，将选出进程设置为当前运行进程
    p_tsk = current;
    current = &task[next];
    
    
    // 调用上下文切换函数
    /*
    uart_printf("\ncpsr = ");
    unsigned int mode;
    __asm__ (
        "mrs %0, cpsr\n"
        : "=r"(mode)
        :
    );
    uart_print_hex(mode);
    uart_puts("");
    
    uart_puts("current process:");
    uart_printf("pid = ");
    uart_print_int(p_tsk->pid);
    uart_printf(" state = ");
    uart_print_int(p_tsk->state);
    uart_puts("");
    
    for (int i = 0; i < 17; ++i) {
        uart_print_int(i);
        uart_printf(" - ");
        uart_print_hex(task[p_tsk->pid].content[i]);
        uart_puts("");
    }
    
    
    
    uart_puts("\nnext process:");
    uart_printf("pid = ");
    uart_print_int(current->pid);
    uart_printf(" state = ");
    uart_print_int(current->state);
    uart_puts("");
    
    for (int i = 0; i < 17; ++i) {
        uart_print_int(i);
        uart_printf(" - ");
        uart_print_hex(task[current->pid].content[i]);
        uart_puts("");
    }
    
    */
    uart_puts("switch process");
    switch_process(p_tsk, current);
    
    return;
}


void show_content(unsigned int r0, unsigned int r1)
{
    /*
    --r0;
    uart_puts("\nR0:");
    for (int i = 16; i >= 0; --i) {
        uart_print_int(i);
        uart_printf(" - ");
        uart_print_hex(*r0--);
        uart_puts("");
    }
    
    --r1;
    uart_puts("\nR1:");
    for (int i = 16; i >= 0; --i) {
        uart_print_int(i);
        uart_printf(" - ");
        uart_print_hex(*r1--);
        uart_puts("");
    }
    */
    
    uart_puts("\nswi caller:");
    uart_printf("r0 = ");
    uart_print_hex(r0);
    uart_printf(", r1 = ");
    uart_print_hex(r1);
    uart_puts("");
    
    
    return;
}


int check_cpu_mode(void)
{
    int mode;
    
    __asm__ (
        "mrs r0, cpsr\n"
        "and r0, #0xF\n"
        "mov %0, r0\n"
        : "=r" (mode)   // output
        : // input
    );
    
    return mode;
}

void round_robin(void)
{
    // 没有当前进程，说明进程还未创建，返回
    if (!current) return;
    
    // 递减睡眠进程，睡眠时间到了，将其状态改为就绪态
    for (int i = 0; i < TASK_SZ; ++i) {
        if (task[i].state == TASK_SLEEPING) {
            //uart_puts("has task sleeping");
            
            //uart_printf("timer = ");
            //uart_print_int(task[i].timer);
            //uart_puts("");
            
            if (--task[i].timer == 0) {
                task[i].state = TASK_RUNNING;
                uart_puts("sleep task start to run");
            }
        }
    }
    
    //if (task[1].state == TASK_SLEEPING) {
    //    uart_puts("task 1 sleeping");
    //} //else if (task[0].state == TASK_RUNNING){
      //  uart_puts("process 0 running");
    //}
    
    // 对当前执行进程时间片递减，每10ms递减一次
    if (current->count) --current->count;
    
    // 如果当前进程时间片已经用完，或当前进程状态为非就绪态，则尝试调度新进程
    if ((current->state != TASK_RUNNING) || (current->count <= 0)) {
        if (check_cpu_mode()) {
            //uart_puts("round_robin and begin schedule");
            schedule();
        }
    }
    
    
    return;
}

int create_process(char *parameters)
{

    int pid = -1;
    
    // 为新进程挑选可用pid
    for (int i = 0; i < TASK_SZ; ++i) {
        if (task[i].state == TASK_UNALLOCATE && VALID_TASK_PID(i)) {
            pid = i;
            break;
        }
    }
    
    // 如果没有可用Pid，出错，退出
    if (pid == -1) return -1;
    
    // 创建进程，关闭中断
    __asm__ (
        "mrs r0, cpsr\n"
        "orr r0, #0xC0\n"
        "msr cpsr_c, r0\n"
    );
    
    
    uart_printf("pid = ");
    uart_print_int(pid);
    uart_puts("");
    
    // 进程执行空间首地址为其对应PID号左移25位，
    unsigned int addr = (pid - 1) * 0x00200000 + 0x50A00000;
    unsigned int *ptr = (unsigned int *) addr;
    
    // 将进程执行空间清零
    for (unsigned int i = 0, n = addr + 0x00200000; i < n; ++i) {
        ptr[i] = 0;
    }
    
    // 将程序代码复制到进程执行空间
    copy_program(ptr);
    
    
    
    
    
    
    
    // 程序参数保存在新进程空间最高1KB空间内，以下简称参数空间
    ptr = (unsigned int *) (addr + 0x00200000 - 1024);
    
    // 根据parameters，计算出程序参数个数，以空格分开
    // ./xxx xxx xxx
    char *argv;
    int argc = 0;
    for (int i = 0; parameters[i] != '\0'; ++i) {
        if (parameters[i] == ' ') ++argc;
    }
    ++argc;
    
    // 将进程pid号保存到参数空间第一个位置
    *ptr++ = pid;
    // 将参数个数保存到参数空间第二个位置
    *ptr++ = argc;
    
    // argv用来存放每个参数字符串，首先跳过argc个位置
    // 每个位置用来放参数字符串指针
    argv = (char *) (ptr + argc);
    
    // 拷贝每个参数字符串
    for (int i = 0, j = 0; i < argc; ++i, ++j) {
        // 将每一个参数字符串地址保存到ptr指向的参数空间
        *ptr++ = (unsigned int) argv;
        
        for (; parameters[j] != '\0' && parameters[j] != ' '; ++j) {
            *argv++ = parameters[j];
        }
        // 最后加上字符串结束符
        *argv++ = '\0';
        // 保证每个参数开始位置是4字节对齐
        argv = (char *) (((unsigned int) argv + 4) & (~0x3));
    }
    
    
    
    
    // 新进程PID
    task[pid].pid = pid;
    // 新进程执行状态
    task[pid].state = TASK_RUNNING;
    // 新进程时间片
    task[pid].count = 5;
    // 新进程优先级
    task[pid].priority = 5;	
    // CPSR	
    task[pid].content[0] = 0x1F;
    // SP栈指针
    task[pid].content[1] = addr + 0x00200000 - 1024;
    // LR返回地址
    task[pid].content[2] = 0;
    // PC
    task[pid].content[16]= addr;
    
    
    
    // 打开中断
    __asm__ (
        "mrs r0, cpsr\n"
        "bic r0, #0xC0\n"
        "msr cpsr_c, r0\n"
        :
        :
    );
    
    
    return pid;
}













