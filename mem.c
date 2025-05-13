#include "mem.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

static int mem_fd = -1;

int open_mem(pid_t pid) {
  char mem_path[64];
  snprintf(mem_path, sizeof(mem_path), "/proc/%d/mem", pid);
  mem_fd = open(mem_path, O_RDWR);
  if (mem_fd == -1) {
    perror("open /proc/pid/mem");
    return -1;
  }
  return 0;
}

void close_mem() {
  if (mem_fd != -1)
    close(mem_fd);
}

unsigned long read_ptr(unsigned long addr) {
  unsigned long val = 0;
  if (lseek(mem_fd, addr, SEEK_SET) == -1) {
    perror("lseek");
    return 0;
  }
  if (read(mem_fd, &val, sizeof(val)) != sizeof(val)) {
    perror("read");
    return 0;
  }
  return val;
}

int write_int(unsigned long addr, int value) {
  if (lseek(mem_fd, addr, SEEK_SET) == -1) {
    perror("lseek (write)");
    return -1;
  }
  if (write(mem_fd, &value, sizeof(value)) != sizeof(value)) {
    perror("write");
    return -1;
  }
  return 0;
}
