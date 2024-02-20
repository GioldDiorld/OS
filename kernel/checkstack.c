#include "type.h"
#include "stdio.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "fs.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"

PUBLIC	int sys_checkstack(int _unused1, int _unused2, int _unused3, struct proc * p_proc)
{
    const u32 retaddr_offset = p_proc->regs.ebp + 4;
    const u32 ss = p_proc->regs.ss;
    const u32 base = reassembly(p_proc->ldts[ss>>3].base_high,24,p_proc->ldts[ss>>3].base_mid,16,p_proc->ldts[ss>>3].base_low);
    const u32 retaddr = base + retaddr_offset;
    if (retaddr > PROC_IMAGE_SIZE_DEFAULT - PROC_ORIGIN_STACK || retaddr - 0x1000 > 0x2000 || retaddr <= 0x1000){
        panic("checkstack: proc %s retaddr (%d) invalid.", p_proc->name, retaddr);
        spin("checkstack");
    }
    return 0;
}