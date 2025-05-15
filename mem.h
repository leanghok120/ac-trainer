#ifndef MEM_H
#define MEM_H

#include <stdint.h>
#include <sys/types.h>
#include <stddef.h>

int mem_read(pid_t pid, uintptr_t address, void *buffer, size_t size);
int mem_write(pid_t pid, uintptr_t address, void *buffer, size_t size);
pid_t find_pid_by_name(const char *name);
uintptr_t get_nth_module_base(pid_t pid, const char *mod_name, int index);

#endif
