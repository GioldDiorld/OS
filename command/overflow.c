#include "stdio.h"
#include "elf.h"

int main() {
    int fd;
    int i;
    char * symbol_test = "test";
    unsigned long test_offset;
    char * symbol_main = "main";
    unsigned long main_offset;

    char * filename = "test_overflow";

    fd = open(filename, O_RDWR);
    assert(fd != -1);

    // read ELF_HEADER
    lseek(fd, 0, SEEK_SET);
    Elf32_Ehdr elf_header;
    read(fd, &elf_header, sizeof(elf_header));

    // 定位到节头表的位置
    lseek(fd, elf_header.e_shoff, SEEK_SET);

    // 寻找符号表节和字符串表节
    Elf32_Shdr symtab_section_header;
    Elf32_Shdr strtab_section_header;
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

    // 读取数据节
    Elf32_Shdr data_section_header;
    lseek(fd, elf_header.e_shoff + sizeof(section_header) * 2, SEEK_SET);
    read(fd, &data_section_header, sizeof(data_section_header));

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
        if (strcmp(symbol_name_buffer, symbol_test) == 0) {
            test_offset = symbol.st_value;
        }

        if (strcmp(symbol_name_buffer, symbol_main) == 0) {
            main_offset = symbol.st_value;
        }
    }


    unsigned int arr3 = test_offset;
    unsigned int len = 16;


    // 赋值arr[3]
    lseek(fd, main_offset + 0x10, SEEK_SET);
    write(fd, &arr3, sizeof(unsigned int));

    // 修改fun参数len
    lseek(fd, main_offset + 0x18, SEEK_SET);
    write(fd, &len, sizeof(unsigned int));

    close(fd);
    return 0;
}
