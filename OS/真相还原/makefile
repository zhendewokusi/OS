OBJS_BIN=$(patsubst ./boot/%.S,out/boot/%.bin,$(shell find ./boot -name '*.S'))
include_paths =  ./boot/include

I_flags = $(include_paths:%=-I%)

#image: ${OBJS_BIN} 定义了一个名为image的目标，依赖于OBJS_BIN，
#表示要生成一个名为MBR.img的磁盘映像文件，并将MBR.bin写入该磁盘映像中。
image: ${OBJS_BIN} out/kernel/kernel.bin
#创建一个大小为30MB的全0磁盘映像文件。
	dd if=/dev/zero of=MBR.img bs=512 count=61440
#MBR.bin文件写入MBR.img磁盘映像文件的第一个扇区。
	dd if=out/boot/mbr.bin of=MBR.img bs=512 count=1 conv=notrunc
	dd if=out/boot/loader.bin of=MBR.img bs=512 seek=1 count=5 conv=notrunc
	dd if=./out/kernel/kernel.bin of=./MBR.img bs=512 count=200 seek=10 conv=notrunc
#@-表示忽略删除过程中出现的错误。
	@-rm -rf *.bin	

#表示将以.S为扩展名的汇编源文件编译为以.bin为扩展名的二进制文件。
out/boot/%.bin:boot/%.S
	mkdir -p $(dir $@)
	nasm $^ -o $@  $(I_flags)

out/kernel/print.o:lib/kernel/print.S
	mkdir -p $(dir $@)
	nasm -f elf -o out/kernel/print.o lib/kernel/print.S -I ./lib/kernel/include -g

out/kernel/kernel.o:kernel/kernel.S
	mkdir -p $(dir $@)
	nasm -f elf -o out/kernel/kernel.o kernel/kernel.S -g

kernel/kernel.i:kernel/kernel.S
	mkdir -p $(dir $@)
	nasm -E -o kernel/kernel.i kernel/kernel.S -g

out/kernel/traps.o:kernel/traps.c
	cc -m32 -I include -I include/asm -I include/kernel -I include/linux -c -fno-builtin -o out/kernel/traps.o kernel/traps.c -fno-stack-protector -g3 -O0

out/kernel/memory.o:kernel/memory.c
	cc -m32 -I include -I include/asm -I include/kernel -I include/linux -c -fno-builtin -o out/kernel/memory.o kernel/memory.c -fno-stack-protector -g3 -O0

out/kernel/thread.o:kernel/thread.c
	cc -m32 -I include -I include/asm -I include/kernel -I include/linux -c -fno-builtin -o out/kernel/thread.o kernel/thread.c -fno-stack-protector -g3 -O0

out/kernel/sched.o:kernel/sched.c
	cc -m32 -I include -I include/asm -I include/kernel -I include/linux -c -fno-builtin -o out/kernel/sched.o kernel/sched.c -fno-stack-protector -g3 -O0

out/lib/string.o:lib/string.c
	mkdir -p $(dir $@)
	cc -m32 -I include -I include/asm -I include/kernel -I include/linux -c -fno-builtin -o out/lib/string.o lib/string.c -fno-stack-protector -g3 -O0

out/lib/kernel/bitmap.o:lib/kernel/bitmap.c
	mkdir -p $(dir $@)
	cc -m32 -I include -I include/asm -I include/kernel -I include/linux -c -fno-builtin -o out/lib/kernel/bitmap.o lib/kernel/bitmap.c -fno-stack-protector -g3 -O0

out/interrupt.o:kernel/interrupt.c
	cc -m32 -I include -I include/asm -I include/kernel -I include/linux -c -fno-builtin -o out/interrupt.o kernel/interrupt.c -fno-stack-protector -g3 -O0

out/init.o:kernel/init.c
	cc -m32 -I include -I include/asm -I include/kernel -I include/linux -I device/ -c -fno-builtin -o out/init.o kernel/init.c -fno-stack-protector -g3 -O0

out/timer.o:device/timer.c
	cc -m32 -I include -I include/asm -I include/kernel -I include/linux -I device -c -fno-builtin -o out/timer.o device/timer.c -fno-stack-protector

out/kernel/main.o:kernel/main.c
	mkdir -p $(dir $@)
	cc -m32 -I include -I include/asm -I include/kernel -I include/linux -c -fno-builtin -o out/kernel/main.o kernel/main.c -fno-stack-protector -g3 -O0 

out/kernel/kernel.bin:out/kernel/main.o out/kernel/print.o out/kernel/kernel.o out/init.o out/interrupt.o out/timer.o out/kernel/traps.o out/lib/string.o out/lib/kernel/bitmap.o out/kernel/memory.o out/kernel/thread.o out/kernel/sched.o
	ld -melf_i386  -T kernel.ld -o ./out/kernel/kernel.bin \
    out/kernel/main.o out/kernel/print.o out/kernel/kernel.o out/init.o out/interrupt.o out/timer.o out/kernel/traps.o out/lib/string.o out/lib/kernel/bitmap.o out/kernel/memory.o out/kernel/thread.o out/kernel/sched.o

build:$(OBJS_BIN) out/kernel/kernel.bin
	make image
#定义了一个名为run的目标，依赖于OBJS_BIN，表示要运行程序。
run: ${OBJS_BIN}
	bochs -f bochsrc.txt -q
debug:
	objcopy --only-keep-debug out/kernel/kernel.bin kernel.sym
	~/Downloads/bochs-gdb/bochs -f gdbbochs.txt -q

clean:
	rm -rf *.img  *.out *.lock *.bin out *.sym

.PHONY:image build run clean