/*******************************************************************************
* File: mmu.h
*
* By weiqiangdragonite@gmail.com
* Date: 2013-8-31
*******************************************************************************/

#ifndef MMU_H
#define MMU_H


#define MMU_FULL_ACCESS         (3 << 10)   // 访问权限，管理员权限
#define MMU_USER_ACCESS         (1 << 10)   // 访问权限，用户类型

#define MMU_DOMAIN_1              (1 << 5)    // 属于域domain 1，用于管理员模式
#define MMU_DOMAIN_0              (0 << 5)    // 属于域domain 0，用于用户模式

#define MMU_SPECIAL             (1 << 4)    // bit 4必须是1
#define MMU_CACHEABLE           (1 << 3)    // 可以使用cache
#define MMU_BUFFERABLE          (1 << 2)    // 可以使用write buffer
#define MMU_SECDESC             (2)         // 表示这是段描述符

#define MMU_SECDESC_CB          (MMU_FULL_ACCESS | MMU_DOMAIN_1 | \
                                 MMU_SPECIAL | MMU_CACHEABLE | \
                                 MMU_BUFFERABLE | MMU_SECDESC)
                                 
#define MMU_SECDESC_NCNB        (MMU_FULL_ACCESS | MMU_DOMAIN_1 | \
                                 MMU_SPECIAL | MMU_SECDESC)


#define TTB_BASE 0x50900000


/* function prototype */
void create_page_table(void);
void enable_mmu(void);


#endif  // MMU_H