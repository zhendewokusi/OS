#!/bin/bash

# 获取上一级目录下的bochsrc.txt文件的路径
bochsrc_path="../bochsrc.txt"

# 获取bochs配置中的分配内存大小
meqs=$(grep "megs" "$bochsrc_path" | awk '{print $3}')

# 设置文件路径
file_path="../include/kernel/bitmap.h"


# 将第5行的宏定义更改为新值
sed -i "5s/.*/#define MEMO_SIZE $meqs/" $file_path