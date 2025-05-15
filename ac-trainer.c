#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include "mem.h"

#define MODULE_NAME "libX11.so"
#define PROCESS_NAME "linux_64_client"
#define AMMO_VALUE 1000

uintptr_t base_offset = 0x32A8;
uintptr_t offsets[] = { 0x78, 0x30, 0x154 };
size_t offsets_len = sizeof(offsets) / sizeof(offsets[0]);

int main() {
  pid_t pid = find_pid_by_name(PROCESS_NAME);
  if (pid == -1) {
    fprintf(stderr, "Could not find process: %s\n", PROCESS_NAME);
    return 1;
  }

  uintptr_t base = get_nth_module_base(pid, MODULE_NAME, 4);
  if (!base) {
    fprintf(stderr, "Could not find module base: %s\n", MODULE_NAME);
    return 1;
  }
  printf("Module base: 0x%lx\n", base);

  uintptr_t addr = base + base_offset;
  uintptr_t ptr = 0;
  printf("base + offset: 0x%lx\n", addr);

  if (mem_read(pid, addr, &ptr, sizeof(ptr)) != 0) {
    fprintf(stderr, "Failed to read base pointer\n");
    return 1;
  }
  printf("Pointer read value: 0x%lx\n", ptr);

  for (size_t i = 0; i < offsets_len - 1; ++i) {
    if (mem_read(pid, ptr + offsets[i], &ptr, sizeof(ptr)) != 0) {
      fprintf(stderr, "Failed to follow pointer chain at level %zu\n", i);
      return 1;
    }
  }

  uintptr_t final_addr = ptr + offsets[offsets_len - 1];
  printf("Freezing ammo at 0x%lx\n", final_addr);

  int ammo = AMMO_VALUE;
  while (1) {
    mem_write(pid, final_addr, &ammo, sizeof(ammo));
    usleep(30000);
  }

  return 0;
}
