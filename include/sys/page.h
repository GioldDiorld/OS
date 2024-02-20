#ifndef _ORANGES_PAGE_H_
#define _ORANGES_PAGE_H_

#define PG_P 1
#define PG_RWW 2
#define PG_USU 4
#define PAGE_SIZE 4096
#define SECTOR_SIZE 512
#define MB 0x100000
#define MAX_VISUAL_MEMORY (128 * MB)
#define MAX_MEMORY (32 * MB)
#define USER_MEM_BEGIN (10 * MB)

#define MAX_VISUAL_PAGE_NUM ((MAX_VISUAL_MEMORY - USER_MEM_BEGIN) / PAGE_SIZE)
#define MAX_PAGE_NUM ((MAX_MEMORY - USER_MEM_BEGIN) / PAGE_SIZE)
#define USER_PAGE_TABLE_BEGIN (0x101000 + USER_MEM_BEGIN / PAGE_SIZE * 4)

#define ADDR2PAGE(addr) ((((unsigned long)addr) - USER_MEM_BEGIN) / PAGE_SIZE)
#define PAGE2ADDR(n) (((unsigned long)(n)) * PAGE_SIZE) + USER_MEM_BEGIN
#define PAGE2PAGE_TABLE(n) *(unsigned int *)((((unsigned int)(n)) * 4) + USER_PAGE_TABLE_BEGIN)

extern unsigned int visual_page_map[MAX_VISUAL_PAGE_NUM / 32];
extern unsigned int phys_page_map[MAX_PAGE_NUM / 32];
extern unsigned int swap_map[MAX_VISUAL_PAGE_NUM/32];
extern unsigned int visual_page_queue[MAX_VISUAL_PAGE_NUM];
extern unsigned int visual_page_queue_head, visual_page_queue_tail;

void *alloc_page(unsigned int page_num);
void free_pages(void *address, unsigned int page_num);
void set_bit(unsigned int bitmap[], unsigned int page_num);
void clear_bit(unsigned int bitmap[], unsigned int page_num);
unsigned int get_bit(unsigned int bitmap[], unsigned int page_num);

#endif
