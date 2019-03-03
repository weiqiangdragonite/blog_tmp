

#define MMU_FULL_ACCESS         (3 << 10)   // 访问权限
#define MMU_DOMAIN              (0 << 5)    // 属于哪个域
#define MMU_SPECIAL             (1 << 4)    // bit 4必须是1
#define MMU_CACHEABLE           (1 << 3)    // 可以使用cache
#define MMU_BUFFERABLE          (1 << 2)    // 可以使用write buffer
#define MMU_SECDESC             (2)         // 表示这是段描述符

#define MMU_SECDESC_WB          (MMU_FULL_ACCESS | MMU_DOMAIN | \
                                 MMU_SPECIAL | MMU_CACHEABLE | \
                                 MMU_BUFFERABLE | MMU_SECDESC)
                                 
#define MMU_SECDESC_NCNB        (MMU_FULL_ACCESS | MMU_DOMAIN | \
                                 MMU_SPECIAL | MMU_SECDESC)

void createPageTable(void);
void enableMMU(void);

void initMMU(void)
{
    // 创建页表
    createPageTable();
    
    // 开启MMU
    enableMMU();
    
    return;
}


void createPageTable(void)
{

    // 本程序只使用一级页表，以段的方式进行地址映射。
    // 一级页表中使用4096个描述符来表示这4GB空间，
    // 每个描述符对应1MB的虚拟地址，每个描述符占用4字节，
    // 所以一级页表占用16KB的空间。
    // 本程序使用SDRAM的开始16KB来存放一级页表，所以剩下的内存
    // 开始物理地址为0x5000_4000。
    
    
    unsigned int va, pa;
    unsigned int *ttb = (unsigned int *) 0x50000000;
    
    // 1.将虚拟地址0 ~ 0x000F_FFFF，即1M大小，映射到同样的物理地址
    // 因为系统一上电是从0地址处开始执行的
    // 为了在开启MMU后仍能运行第一部分的程序，
    // 将0 ~ 1M 的虚拟地址映射到同样的物理地址
    va = 0;
    pa = 0;
    ttb[0] = (0 | MMU_SECDESC_WB);
    
    // 2.SDRAM物理地址范围0x5000_0000 ~ 0x5FFF_FFFF，
    // 将虚拟地址0xB000_0000 ~ 0xB00F_FFFF映射到
    // 物理地址范围0x5000_0000 ~ 0x500F_FFFF
    // 页表项的位置 = 0xB000_0000 / 0x0010_0000 = 2816，即ttb[2816] = ...
    va = 0xB0000000;
    pa = 0x50000000;
    *(ttb + (va >> 20)) = (pa & 0xFFF00000) | MMU_SECDESC_WB;
    
    // 3.GPIO寄存器的起始地址为0x7F00_8000，
    // 将虚拟地址0xA000_0000 ~ 0xA00F_FFFF映射到
    // 物理地址范围0x7F00_0000 ~ 0x7F0F_FFFF
    va = 0xA0000000;
    pa = 0x7F008000;
    *(ttb + (va >> 20)) = (pa & 0xFFF00000) | MMU_SECDESC_NCNB;
    
    // 分析:
    // 虚拟地址的[31:20]用于索引一级页表，找到它所对应的描述符(表项)，
    // 对应于va >> 20
    // 段描述符的[31:20]保存段的物理地址，对应于pa & 0xFFF00000
    // 位[11:0]中用来设置段的访问权限，包括所属的域、AP位、C位(是否可Cache)、
    // B位(是否使用Write buffer)，对应于MMU_SECDESC_WB或MMU_SECDESC_NCNB
    
    return;
}

void enableMMU(void)
{
    unsigned int *ttb = (unsigned int *) 0x50000000;
    
    // 开启MMU
    // from 韦东山
    __asm__ (
        "mov r0, #0\n"
        "mcr p15, 0, r0, c7, c7, 0\n"       // 使无效ICaches和DCaches
        
        "mcr p15, 0, r0, c7, c10, 4\n"      // drain write buffer on v4
        "mcr p15, 0, r0, c8, c7, 0\n"       // 使无效指令、数据TLB
        
        "mcr p15, 0, %0, c2, c0, 0\n"       // 设置页表基址寄存器
        
        "mvn r0, #0\n"
		"mcr p15, 0, r0, c3, c0, 0\n"       // 域访问控制寄存器设为0xFFFFFFFF，不进行权限检查
        
        
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