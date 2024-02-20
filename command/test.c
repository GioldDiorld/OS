#include "stdio.h"

int main() {
    int pid = getpid();

    while(1) {
        printf("pid is : %d\n", pid);
    }
    return 0;
}
