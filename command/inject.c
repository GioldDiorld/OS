#include "stdio.h"
#include "string.h"
#include "elf.h"


int main(int argc, char * argv[]) {
    int fd;
    int i;
    char *symbol_start = "_start";
    unsigned long start_offset;
    char *symbol_print = "printf";
    unsigned long print_offset;
    char * filepath = argv[1];

    fd = open(filepath, O_RDWR);
    assert(fd != -1);

    
    // 准备代码
    // 填充到0x0fd0     0x1000前0x30字节
    // unsigned char code_inject[] = {
    //     0x50,                                       // push eax
    //     0x51,                                       // push ecx
    //     0xc7, 0x04, 0x24, 0x00, 0x00, 0x00, 0x00,   // movl   $0x00(后面填充),(%esp)
    //     0xe8, 0x00, 0x00, 0x00, 0x00,               // call printf(后面填充)
    //     0x59,                                       // pop ecx
    //     0x58,                                       // pop eax
    //     0xe9, 0x00, 0x00, 0x00, 0x00,               // jmp _start(后面填充)
    //     0xf4,                                       // hlt
    //     0x90, 0x90                                  // nop
    // };
    unsigned char code_inject[] = {
        0x50,                                       // push eax
        0x51,                                       // push ecx
        0x55,                                       // push ebp
        0x89, 0xe5,                                 // mov esp, ebp
        0x83, 0xe4, 0xf0,                           // and esp, 0xfffffff0
        0x83, 0xec, 0x10,                           // sub esp, 0x10
        0xc7, 0x04, 0x24, 0x00, 0x00, 0x00, 0x00,   // movl   $0x00(后面填充),(%esp)
        0xe8, 0x00, 0x00, 0x00, 0x00,               // call printf(后面填充)
        0xc9,                                       // leave
        0x59,                                       // pop ecx
        0x58,                                       // pop eax
        0xe9, 0x00, 0x00, 0x00, 0x00,               // jmp _start(后面填充)
        0xf4,                                       // hlt
        0x90, 0x90                                  // nop
    };
    unsigned char *data_inject = "This elf file is injected!!!\n";
    int code_length = 34;
    int data_length = 30;
    unsigned int code_base = 0xf00;
    unsigned int call_base = code_base + 23;
    unsigned int jmp_base  = code_base + 31; 
    unsigned int added_length = 0x100;

    // read ELF_HEADER
    lseek(fd, 0, SEEK_SET);
    Elf32_Ehdr elf_header;
    read(fd, &elf_header, sizeof(elf_header));

    // 定位到节头表的位置
    lseek(fd, elf_header.e_shoff, SEEK_SET);

    // 寻找符号表节和字符串表节
    Elf32_Shdr symtab_section_header;
    Elf32_Shdr strtab_section_header;
    Elf32_Shdr code_section_header;
    Elf32_Shdr section_header;
    for (i = 0; i < elf_header.e_shnum; ++i) {
        read(fd, &section_header, sizeof(section_header));

        // 判断是否为符号表节
        if (section_header.sh_type == SHT_SYMTAB) {
            symtab_section_header = section_header;
        }

        // 判断是否为字符串表节(有多个，据观察，最后一个有我们所需要的)
        if (section_header.sh_type == SHT_STRTAB) {
            strtab_section_header = section_header;
        }
    }

    // 读取代码节
    lseek(fd, elf_header.e_shoff + sizeof(section_header), SEEK_SET);
    read(fd, &code_section_header, sizeof(code_section_header));


    // 读取符号表
    Elf32_Sym symbol;
    for (i = 0; i < symtab_section_header.sh_size / sizeof(symbol); i++) {
        lseek(fd, symtab_section_header.sh_offset + i * sizeof(Elf32_Sym), SEEK_SET);
        read(fd, &symbol, sizeof(symbol));

        // 读取符号名
        lseek(fd, strtab_section_header.sh_offset + symbol.st_name, SEEK_SET);
        char symbol_name_buffer[64];    // 假设符号长度不超过64
        read(fd, symbol_name_buffer, sizeof(symbol_name_buffer));

        // 检查符号名是否匹配
        if (strcmp(symbol_name_buffer, symbol_start) == 0) {
            start_offset = symbol.st_value;
        }
        if (strcmp(symbol_name_buffer, symbol_print) == 0) {
            print_offset = symbol.st_value;
        }
    }


    // 填充code
    unsigned int data_offset = code_base + code_length;
    int call_offset = print_offset - call_base;
    int jmp_offset = start_offset - jmp_base;
    // 填充movl   $0x00,(%esp)
    code_inject[14] = data_offset & 0xff;
    code_inject[15] = (data_offset >> 8) & 0xff;
    code_inject[16] = (data_offset >> 16) & 0xff;
    code_inject[17] = data_offset >> 24;

    // 填充call printf
    code_inject[19] = call_offset & 0xff;
    code_inject[20] = (call_offset >> 8) & 0xff;
    code_inject[21] = (call_offset >> 16) & 0xff;
    code_inject[22] = call_offset >> 24;

    // 填充jmp _start
    code_inject[27] = jmp_offset & 0xff;
    code_inject[28] = (jmp_offset >> 8) & 0xff;
    code_inject[29] = (jmp_offset >> 16) & 0xff;
    code_inject[30] = jmp_offset >> 24;

    // 写入code
    lseek(fd, code_base, SEEK_SET);
    write(fd, code_inject, code_length);
    // 写入data
    lseek(fd, code_base + code_length, SEEK_SET);
    write(fd, data_inject, data_length);

    // 重写e_entry
    elf_header.e_entry = code_base;
    lseek(fd, 0, SEEK_SET);
    write(fd, &elf_header, sizeof(elf_header));

    // 重写代码节sh_offset和sh_size
    code_section_header.sh_offset = code_base;
    code_section_header.sh_size = code_section_header.sh_size + added_length;
    lseek(fd, elf_header.e_shoff + sizeof(section_header), SEEK_SET);
    write(fd, &code_section_header, sizeof(code_section_header));

    // 重写程序段p_offset, p_filesz和p_memsz
    Elf32_Phdr program_header;
    lseek(fd, elf_header.e_phoff, SEEK_SET);
    read(fd, &program_header, sizeof(program_header));
    program_header.p_offset = code_base;
    program_header.p_vaddr = code_base;
    program_header.p_paddr = code_base;
    program_header.p_filesz = program_header.p_filesz + added_length;
    program_header.p_memsz = program_header.p_memsz + added_length;
    program_header.p_align = 0x0100;
    lseek(fd, elf_header.e_phoff, SEEK_SET);
    write(fd, &program_header, sizeof(program_header));
    

    close(fd);
    printf("Inject complete.\n");
}

