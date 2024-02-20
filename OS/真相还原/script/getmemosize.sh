#!/bin/bash

# 获取上一级目录下的bochsrc.txt文件的路径
bochsrc_path="../bochsrc.txt"

# 获取bochs配置中的分配内存大小
meqs=$(grep "megs" "$bochsrc_path" | awk '{print $3}')

# 将其
