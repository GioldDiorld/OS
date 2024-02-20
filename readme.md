启动

```shell
start.sh
```



安全的指令都可以正常运行

1. pwd
2. echo
3. 任务二 calculate计算器
4. 任务二 cat打开



在clock.c中的clock_handler中注释以下代码

```c
if (distance >= NR_TASKS + NR_NATIVE_PROCS + 2)
    checkstack();
```

关闭checkstack之后可以进行有威胁的指令

在exec.c中的do_exec中注释以下代码

```
if (!check_validdata(pathname, mmbuf, real_length))
{
    printl("Invalid Checksum for file %s\n", pathname);
    return -1;
}
```

关闭check之后可以看到威胁指令的效果

1. POC1 inject
2. POC2 overflow 后执行test_overflow 
3. 任务二 inject后command无法运行，tty0内显示checkfile fails，关闭checkfile后可正常inject
4. 任务四 开启-fstack-protector后（开启canary）编译canary.c，可在溢出后检测，并将程序正常退出