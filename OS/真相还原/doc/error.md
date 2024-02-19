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