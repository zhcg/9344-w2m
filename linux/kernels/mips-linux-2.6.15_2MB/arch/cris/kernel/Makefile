# $Id: //depot/sw/releases/Aquila_9.2.0_U11/linux/kernels/mips-linux-2.6.15_2MB/arch/cris/kernel/Makefile#1 $
#
# Makefile for the linux kernel.
#

extra-y	:= vmlinux.lds

obj-y   := process.o traps.o irq.o ptrace.o setup.o \
	   time.o sys_cris.o semaphore.o

obj-$(CONFIG_MODULES)    += crisksyms.o
obj-$(CONFIG_MODULES)	 += module.o
obj-$(CONFIG_SYSTEM_PROFILER)   += profile.o

clean:

