/*************************************************************************//**
 *****************************************************************************
 * @file   mm/main.c
 * @brief  Orange'S Memory Management.
 * @author Forrest Y. Yu
 * @date   Tue May  6 00:33:39 2008
 *****************************************************************************
 *****************************************************************************/

#include "type.h"
#include "config.h"
#include "stdio.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "fs.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "keyboard.h"
#include "proto.h"
#include "page.h"

unsigned int FIFO = 0;

PUBLIC void do_fork_test();

PRIVATE void init_mm();

/*****************************************************************************
 *                                task_mm
 *****************************************************************************/
/**
 * <Ring 1> The main loop of TASK MM.
 * 
 *****************************************************************************/
PUBLIC void task_mm()
{
	init_mm();

	while (1) {
		send_recv(RECEIVE, ANY, &mm_msg);
		int src = mm_msg.source;
		int reply = 1;

		int msgtype = mm_msg.type;

		switch (msgtype) {
		case FORK:
			mm_msg.RETVAL = do_fork();
			break;
		case EXIT:
			do_exit(mm_msg.STATUS);
			reply = 0;
			break;
		case EXEC:
			mm_msg.RETVAL = do_exec();
			break;
		case WAIT:
			do_wait();
			reply = 0;
			break;
		default:
			dump_msg("MM::unknown msg", &mm_msg);
			assert(0);
			break;
		}

		if (reply) {
			mm_msg.type = SYSCALL_RET;
			send_recv(SEND, src, &mm_msg);
		}
	}
}

/*****************************************************************************
 *                                init_mm
 *****************************************************************************/
/**
 * Do some initialization work.
 * 
 *****************************************************************************/
PRIVATE void init_mm()
{
	int i;
    for (i = 0; i < ADDR2PAGE(MAX_MEMORY); ++i) {
        PAGE2PAGE_TABLE(i) = 0;
    }

	struct boot_params bp;
	get_boot_params(&bp);

	memory_size = bp.mem_size;

	/* print memory size */
	printl("{MM} memsize:%dMB\n", memory_size / (1024 * 1024));
}


/*****************************************************************************
 *                                init_mm
 *****************************************************************************/
/**
 * Do some initialization work.
 *
 *****************************************************************************/
PUBLIC void *alloc_page(unsigned int page_num) {
	int i;
    for (i = 0; i < MAX_PAGE_NUM; i++) {
        if (!get_bit(visual_page_map, i)) {
            char find = 1;
			int j;
            for (j = 0; j < page_num; j++) {
                if (get_bit(visual_page_map, i+j)) {
                    find = 0;
                    break;
                }
            }
            if (find) {
				int j;
                for (j = 0; j < page_num; j++) {
                    set_bit(visual_page_map, i+j);
                }
                return (void *)PAGE2ADDR(i);
            }
        }
    }
    return 0;
}

PUBLIC void free_pages(void *address, unsigned int page_num)
{
	int i;
    for (i = 0; i < page_num; i++) {
        clear_bit(visual_page_map, ADDR2PAGE(address)+i);
        clear_bit(phys_page_map, ADDR2PAGE(PAGE2PAGE_TABLE(ADDR2PAGE(address)+i) & 0xFFFFF000));
		// printl("%d ", ADDR2PAGE(address)+i);
		// PAGE2PAGE_TABLE(ADDR2PAGE(address)+i) = 0;

		// printl("%d ", ADDR2PAGE(PAGE2PAGE_TABLE(ADDR2PAGE(address)+i) & 0xFFFFF000));

    }
    return;
}
