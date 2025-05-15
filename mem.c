#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/uio.h>
#include "mem.h"
#include <dirent.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef PROCESS_VM_READV_SYSCALL
#define PROCESS_VM_READV_SYSCALL 310
#endif

#ifndef PROCESS_VM_WRITEV_SYSCALL
#define PROCESS_VM_WRITEV_SYSCALL 311
#endif

ssize_t process_vm_readv(pid_t pid,
                         const struct iovec *local_iov,
                         unsigned long liovcnt,
                         const struct iovec *remote_iov,
                         unsigned long riovcnt,
                         unsigned long flags) {
    return syscall(PROCESS_VM_READV_SYSCALL, pid, local_iov, liovcnt, remote_iov, riovcnt, flags);
}

ssize_t process_vm_writev(pid_t pid,
                          const struct iovec *local_iov,
                          unsigned long liovcnt,
                          const struct iovec *remote_iov,
                          unsigned long riovcnt,
                          unsigned long flags) {
    return syscall(PROCESS_VM_WRITEV_SYSCALL, pid, local_iov, liovcnt, remote_iov, riovcnt, flags);
}

#include "mem.h"
#include <sys/uio.h>
#include <dirent.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int mem_read(pid_t pid, uintptr_t address, void *buffer, size_t size) {
    struct iovec local = {
        .iov_base = buffer,
        .iov_len = size
    };
    struct iovec remote = {
        .iov_base = (void *)address,
        .iov_len = size
    };
    return process_vm_readv(pid, &local, 1, &remote, 1, 0) == (ssize_t)size ? 0 : -1;
}

int mem_write(pid_t pid, uintptr_t address, void *buffer, size_t size) {
    struct iovec local = {
        .iov_base = buffer,
        .iov_len = size
    };
    struct iovec remote = {
        .iov_base = (void *)address,
        .iov_len = size
    };
    return process_vm_writev(pid, &local, 1, &remote, 1, 0) == (ssize_t)size ? 0 : -1;
}

pid_t find_pid_by_name(const char *name) {
    DIR *dir = opendir("/proc");
    if (!dir) return -1;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (!isdigit(entry->d_name[0])) continue;

        char path[256];
        snprintf(path, sizeof(path), "/proc/%s/comm", entry->d_name);

        FILE *fp = fopen(path, "r");
        if (!fp) continue;

        char comm[256];
        if (fgets(comm, sizeof(comm), fp)) {
            comm[strcspn(comm, "\n")] = 0;
            if (strcmp(comm, name) == 0) {
                fclose(fp);
                closedir(dir);
                return atoi(entry->d_name);
            }
        }
        fclose(fp);
    }
    closedir(dir);
    return -1;
}

uintptr_t get_nth_module_base(pid_t pid, const char *mod_name, int index) {
    char maps_path[256];
    snprintf(maps_path, sizeof(maps_path), "/proc/%d/maps", pid);
    FILE *fp = fopen(maps_path, "r");
    if (!fp) return 0;

    char line[512];
    int count = 0;
    uintptr_t base = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, mod_name)) {
            if (count == index) {
                sscanf(line, "%lx-%*lx", &base);
                break;
            }
            count++;
        }
    }

    fclose(fp);
    return base;
}
