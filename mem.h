#pragma once

#include <stdint.h>
#include <sys/types.h>

int open_mem(pid_t pid);
void close_mem();
unsigned long read_ptr(unsigned long addr);
int write_int(unsigned long addr, int value);
