#include <stdio.h>
#include <string.h>

void __stack_chk_fail_local()
{
    printf("Mia!!!! __Stack smashing detected__\n");
//    __asm__("hlt");
    exit(1);
}

void bad_function()
{
    char buffer[10];
    strcpy(buffer, "1234567890000000000000000");
    printf("bad: %s\n", buffer);
}

void good_function()
{
    // just a good function, nothing broken
    char buffer[10];
    strcpy(buffer, "123456789");
    printf("good: %s\n", buffer);
}

int main()
{
    good_function();
    bad_function();

    return 0;
}