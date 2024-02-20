#include "page.h"
#include "stdio.h"
#include "const.h"
#include "protect.h"
#include "proc.h"
#include "hd.h"

unsigned int visual_page_map[MAX_VISUAL_PAGE_NUM / 32] = {0};
unsigned int phys_page_map[MAX_PAGE_NUM / 32] = {0};
unsigned int swap_map[MAX_VISUAL_PAGE_NUM/32] = {0};

unsigned int visual_page_queue[MAX_VISUAL_PAGE_NUM] = {0};
unsigned int visual_page_queue_head = 0, visual_page_queue_tail = 0;

void set_bit(unsigned int bitmap[], unsigned int page_num)
{
    bitmap[page_num / 32] |= (1 << (page_num % 32));
}

void clear_bit(unsigned int bitmap[], unsigned int page_num)
{
    bitmap[page_num / 32] &= ~(1 << (page_num % 32));
}

unsigned int get_bit(unsigned int bitmap[], unsigned int page_num)
{
    unsigned int i = page_num / 32;
    unsigned int j = page_num % 32;
    return bitmap[i] & (1 << j);
}