#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include "mem.h"

#define LIB_BASE      0x78e34a2ab000UL
#define RELATIVE_BASE 0x32A8
#define OFFSET1       0x78
#define OFFSET2       0x30
#define OFFSET3       0x154
#define DESIRED_VALUE 1000

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("usage: sudo ./ac-trainer <pid>\n");
    return 1;
  }

  pid_t pid = atoi(argv[1]);
  if (open_mem(pid) != 0) return 1;

  printf("Trainer started. Using base: 0x%lx\n", LIB_BASE);

  while (1) {
    unsigned long ptr1 = read_ptr(LIB_BASE + RELATIVE_BASE);
    if (!ptr1) break;

    unsigned long ptr2 = read_ptr(ptr1 + OFFSET1);
    if (!ptr2) break;

    unsigned long ptr3 = read_ptr(ptr2 + OFFSET2);
    if (!ptr3) break;

    unsigned long final_addr = ptr3 + OFFSET3;

    write_int(final_addr, DESIRED_VALUE);

    usleep(50000);
  }

  close_mem();
  return 0;
}
