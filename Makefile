CC=gcc

# fill in all your make rules

all: objdump_x2017 vm_x2017

objdump_x2017: objdump_x2017.c
vm_x2017: vm_x2017.c

test: objdump_x2017
	bash test.sh
