# Makefile – makefile of our memory driver

# if KERNELRELEASE is not defined, we've been called directly from the command line.
# Invoke the kernel build system.
ifeq (${KERNELRELEASE},)
	KERNEL_SOURCE := /usr/src/linux-headers-5.8.0-44-generic
	PWD := $(shell pwd)
default:
	${MAKE} -C ${KERNEL_SOURCE} M=${shell pwd} ${CMFLAGS} modules

clean:
	rm -f *.o *.ko *.mod* \.*cmd* *order *symvers \.*d

# Otherwise KERNELRELEASE is defined; we've been invoked from the
# kernel build system and can use its language.
else
	obj-m := memory.o
endif

