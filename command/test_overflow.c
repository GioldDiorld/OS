#include "stdio.h"

void test() {
    printf("Stack Overflow!!!\n");
}

void fun(unsigned char* s, int len) {
    unsigned char buf[4];
    int i = 0;
    for (i = 0; i < len; i++) {
        buf[i] = s[i];
    }

    for (;i < 1000000000;i++){
        // 避免程序在一个时钟中断内运行完毕
    }

    return;
}

int main() {
    unsigned int arr[4];
    arr[1] = 4;         // 维持fun里的i
    arr[3] = 0;         // test地址，需修改
    fun(arr, 4);        // 需修改为16
    return 0;
}
