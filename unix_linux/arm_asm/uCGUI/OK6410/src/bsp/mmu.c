/*******************************************************************************
* File: mmu.c
*
* By weiqiangdragonite@gmail.com
* Date: 2013-8-31
*******************************************************************************/

#include "mmu.h"

void create_page_table(void)
{
    // 本程序只使用一级页表，以段的方式进行地址映射。
    // 一级页表中使用4096个描述符来表示这4GB空间，
    // 每个描述符对应1MB的虚拟地址，每个描述符占用4字节，
    // 所以一级页表占用16KB的空间。
    // 本程序使用SDRAM的物理地址0x5090_0000来存放一级页表
    
    //unsigned int va, pa;
    unsigned int *ttb = (unsigned int *) TTB_BASE;
    
    // 1.将虚拟地址0 ~ 0x000F_FFFF，即1M大小，映射到同样的物理地址
    // 因为系统一上电是从0地址处开始执行的，而且异常向量表也在0处
    // 将0 ~ 1M 的虚拟地址映射到同样的物理地址
    //va = 0;
    //pa = 0;
    ttb[0] = (0 | MMU_SECDESC_CB);
    
    // 2.SDRAM物理地址范围0x5000_0000 ~ 0x5FFF_FFFF，
    // 将虚拟地址0x5000_0000 ~ 0x5FFF_FFFF映射到同样的物理地址
    // 页表项的位置 = 0x5000_0000 / 0x0010_0000 = 500，即ttb[500] = ...
    //va = 0x50000000;
    //pa = 0x50000000;
    //*(ttb + (va >> 20)) = (pa & 0xFFF00000) | MMU_SECDESC_WB;
    //
    // when we get a MVA address, such as 0x50000000, we need to get
    // where it is in the ttb, so 0x50000000 / 0x00100000 = 500,
    // it is in the ttb[500], and we go to ttb[500] and get the PA address in it.
    
    for (unsigned int pa = 0x50000000; pa < 0x60000000; pa += 0x00100000) {
        *(ttb + (pa >> 20)) = (pa & 0xFFF00000) | MMU_SECDESC_CB;
    }
    
    // 3.外设寄存器的地址为0x7000_0000 ~ 0x7F00_FFFF
    // 将虚拟地址0x7000_0000 ~ 0x7F00_FFFF映射到同样的物理地址
    //va = 0xA0000000;
    //pa = 0x7F008000;
    //*(ttb + (va >> 20)) = (pa & 0xFFF00000) | MMU_SECDESC_NCNB;
    
    for (unsigned int pa = 0x70000000; pa < 0x80000000; pa += 0x00100000) {
        *(ttb + (pa >> 20)) = (pa & 0xFFF00000) | MMU_SECDESC_NCNB;
    }
    
    // 分析:
    //
    // MVA:
    // 31           20|19              0
    // +--------------+-----------------+
    // | 页表项索引   |  段地址索引     |
    // +--------------+-----------------+
    //
    // 段描述符:
    // 31                   20|19     12|  10|9|8      5 4 3 2 1 0 
    // +----------------------+---------+----+-+--------+-+-+-+-+-+
    // | Section base address |         | AP | | Domain |1|C|B|1|0| Section
    // +----------------------+---------+----+-+--------+-+-+-+-+-+
    // 
    // MVA的[31:20]用于索引一级页表，找到它所对应的描述符(表项)，对应于va >> 20
    // 段描述符的[31:20]保存段的物理首地址，对应于pa & 0xFFF00000
    // 位[11:0]中用来设置段的访问权限，包括所属的域、AP位、C位(是否可Cache)、
    // B位(是否使用Write buffer)，对应于MMU_SECDESC_WB或MMU_SECDESC_NCNB
    
    
    // 下面开始时进程空间的虚拟地址分配
    // 进程空间的物理地址为 0x50A00000 ~ 0x5FA00000，共240M
    // 每个进程的分配虚拟地址空间为32MB，但实际的物理地址空间只有2MB，
    // 所以只有120个进程（ARM最大支持128个进程）
    /*
    
    MVA                            PA
    
    0xF000_0000 +----------------+ 0x5F9F_FFFF
                |   process 120  |
                +----------------+
    
                |   process 2    |
    0x0400_0000 +----------------+ 0x50C0_0000
                |   process 1    | 2M
    0x0200_0000 +----------------+ 0x50A0_0000
    
    
    */
    /*
    
    每个进程的虚拟地址空间为PID * 0x02000000 ~ PID * 0x02000000 + 0x01FFFFFF
    进程1: 物理地址空间 0x50A00000 ~ 0x50BFFFFF，对应MVA（修正虚拟地址，进程PID<<25形成）
           MVA地址空间  0x02000000 ~ 0x03FFFFFF
           
    进程2: 物理地址空间 0x50C00000 ~ 0x50DFFFFF
           MVA地址空间  0x04000000 ~ 0x05FFFFFF
           
           ...
           (n - 1) * 0x00200000 + 0x50A00000 ~ + 0x001FFFFF
           
    进程39: 物理地址空间 0x55600000 ~ 0x557FFFFF
            MVA地址空间  0x4E000000 ~ 0x4FFFFFFF
            
    进程40: 物理地址空间 0x55800000 ~ 0x559FFFFF
            MVA地址空间  0x50000000 ~ 0x51FFFFFF
            
        因为0x50000000 ~ 0x60000000 对应于真实的物理地址空间，而且前面已经做了
        映射，所以从进程40 ~ 进程47不能再做映射。
    */
    // MVA address such as 0x02000000, we need to get where it is in the ttb,
    // so 0x02000000 / 0x00100000 = 20, is in the ttb[20], and we go to
    // ttb[20] to get the PA address 0x50A00000
    for (int pid = 1; pid < 40; ++pid) {
        *(ttb + ((pid * 0x02000000) >> 20)) =
            (((pid - 1) * 0x00200000 + 0x50A00000) & 0xFFF00000) | MMU_SECDESC_CB;
    }
    
    /*
    
    进程48: 物理地址空间 0x56800000 ~ 0x569FFFFF
            MVA地址空间  0x60000000 ~ 0x61FFFFFF
            
            ...
            
    进程55: 物理地址空间  0x57000000 ~ 0x571FFFFF
            MVA地址空间   0x6E000000 ~ 0x6FFFFFFF
            
    进程56: 物理地址空间  0x57200000 ~ 0x573FFFFF
            MVA地址空间   0x70000000 ~ 0x71FFFFFF
            
        因为0x70000000 ~ 0x80000000 对应于真实的物理地址空间，而且前面已经做了
        映射，所以从进程56 ~ 进程63不能再做映射。
    
    */
    for (int pid = 48; pid < 56; ++pid) {
        *(ttb + ((pid * 0x02000000) >> 20)) =
            (((pid - 1) * 0x00200000 + 0x50A00000) & 0xFFF00000) | MMU_SECDESC_CB;
    }
    
    /*
    
    进程64: 物理地址空间 0x58800000 ~ 0x589FFFFF
            MVA地址空间  0x80000000 ~ 0x81FFFFFF
            
    进程65: 物理地址空间 0x58A00000 ~ 0x58BFFFFF
            MVA地址空间  0x82000000 ~ 0x83FFFFFF
            
            ...
            
    进程120:  物理地址空间 0x5F800000 ~ 0x5F9FFFFF
              MVA地址空间  0xF0000000 ~ 0xF1FFFFFF
    
    */
    for (int pid = 64; pid <= 120; ++pid) {
        *(ttb + ((pid * 0x02000000) >> 20)) =
            (((pid - 1) * 0x00200000 + 0x50A00000) & 0xFFF00000) | MMU_SECDESC_CB;
    }
    
    
    
    
    return;
}

void enable_mmu(void)
{
    unsigned int *ttb = (unsigned int *) TTB_BASE;
    
    // 开启MMU
    __asm__ (
        "mov r0, #0\n"
        "mcr p15, 0, r0, c7, c7, 0\n"       // 使无效ICaches和DCaches
        
        "mcr p15, 0, r0, c7, c10, 4\n"      // 使能写入缓冲器
        "mcr p15, 0, r0, c8, c7, 0\n"       // 使无效指令、数据TLB
        
        "mcr p15, 0, %0, c2, c0, 0\n"       // 设置页表基址寄存器
        
        // 域访问控制寄存器设为0xFFFFFFFD，Domain0为01用户模式，其它为管理员权限
        // 0x2取反等于0xFFFFFFFD
        "mvn r0, #0x2\n"
		"mcr p15, 0, r0, c3, c0, 0\n"       // 写入域控制信息
        
        
        "mrc p15, 0, r0, c1, c0, 0\n"       // 读出控制寄存器的值
        // C1控制寄存器的位含义为
        // RR[14] - Round-robin置换， 0为随机置换，1为Round Robin置换
        // V[13] - 异常向量表地址，0为低地址0x0，1为高地址0xFFFF0000
        // I[12] - ICaches控制，0为禁用，1为使能
        // S[8], R[9] - 和页表中描述符一起确定内存访问权限
        // B[7] - 大小端设置，0为小端模式，1为大端模式
        // C[2] - DCaches使能，0为禁用，1为使能
        // A[1] - 地址对齐检查，0为禁用，1为使能
        // M[0] - MMU使能，0为禁用，1为使能
        "bic r0, r0, #0x3000\n"             // 清除V、I位
        "bic r0, r0, #0x0300\n"             // 清除R、S位
        "bic r0, r0, #0x0087\n"             // 清除B、C、A、M位
        
        "orr r0, r0, #0x0002\n"             // 开启地址对齐检查
        "orr r0, r0, #0x0004\n"             // 开启DCaches
        "orr r0, r0, #0x1000\n"             // 开启ICaches
        "orr r0, r0, #0x0001\n"             // 使能MMU
        
        "mcr p15, 0, r0, c1, c0, 0\n"       // 将修改的值写入控制器中
        : // 无输出
        : "r" (ttb)
    
    );

    return;
}