# ac-trainer

a simple trainer for assault cube built for linux built in C

## installation

```sh
gcc -o ac-trainer ac-trainer.c mem.h mem.c
```

## usage

```sh
sudo ./ac-trainer <pid>
```

## notes

if you're facing some issues with input/output, try running the following command:

```sh
cat /proc/sys/kernel/yama/ptrace_scope
```

if the above command prints 1 or anything above then run:

```sh
sudo sysctl -w kernel.yama.ptrace_scope=0
```
