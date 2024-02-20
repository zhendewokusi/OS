就离谱，改了两天，实在不知道什么问题，跑了下作者仓库源码，和我一样的疑问，麻了，学艺不精的我想哭，这玩意到底咋回事呀。
```c
    __asm__ __volatile__("sti");	     // 演示中断处理
    enum intr_status tmp1 = get_intr_status();
    __asm__ __volatile__("cli");	     // 演示中断处理
```

```c
    __asm__ __volatile__("sti");	     // 演示中断处理
    __asm__ __volatile__("cli");	     // 演示中断处理
    enum intr_status tmp1 = get_intr_status();
```

--------------------------------

难过了，整了半天忘记了位图是全局的，不支持变长数组，一整个蚌埠住了。

```c
        // 什么狗屎代码，直接操作内存拿loader获取的mem_size，头疼，之前写的loader没考虑到位，哎
        put_int(mem_size);
        put_char('\n');
        asm volatile (
        "movl $0xc009efec, %%eax\n\t"
        "movl (%%eax), %0"
        : "=r" (mem_size)
        :
        : "eax"
        );
        put_int(mem_size);
```