# ELTEX Labs

Collection of C labs covering compilation, language fundamentals, libraries, processes, threads, IPC, and sockets.

## Layout

```
ELTEX_LABS/
├── Makefile          # top-level: build_LAB_*, clean_LAB_*, clean_build, help
├── vars.make         # shared toolchain variables
├── helper.sh         # text shown by `make help`
├── LABS/LAB_N/       # sources (authoritative)
│   ├── Makefile      # fans out to task_* directories
│   └── task_*/       # per-task sources + Makefile
└── build/LABS/       # working copy used by the root build
```

## Quick start

```bash
make help
make build_all                 # build every lab
make build_LAB_18              # build one lab
make clean_LAB_18
make clean_build               # remove the entire build/ tree
```

Per-task, after a lab build:

```bash
make -C build/LABS/LAB_18/task_tcp_inet build
PROG=server make -C build/LABS/LAB_18/task_tcp_inet run
PROG=client make -C build/LABS/LAB_18/task_tcp_inet run
```

### Useful variables (`vars.make`)

| Variable | Values | Meaning |
|----------|--------|---------|
| `DEBUG` / `USE_DEBUG` | `y`/`n` | Debug symbols / `-O0` (name differs across task Makefiles) |
| `PROG` | binary name | Which program `make run` executes when several exist |
| `USE_VALGRIND` | `y`/`n` | Documented in helper (prefer `make run_vlgrnd` in tasks) |

## Labs overview

| Lab | Topic |
|-----|-------|
| [LAB_0](LABS/LAB_0/README.md) | Compilation stages — C compilation pipeline (preprocess → compile → assemble → link) |
| [LAB_1](LABS/LAB_1/README.md) | Bitwise operations — Byte-level inspection and modification of integers |
| [LAB_2](LABS/LAB_2/README.md) | Matrices and arrays — 2D arrays, indexing, and matrix transforms |
| [LAB_3](LABS/LAB_3/README.md) | Pointers — Pointers, endianness, and pointer-to-pointer |
| [LAB_4](LABS/LAB_4/README.md) | Structs and an address book — Structure layout/padding and a simple contact ledger |
| [LAB_5](LABS/LAB_5/README.md) | Stack buffer overflow demo — Unsafe input and stack smashing (educational) |
| [LAB_6](LABS/LAB_6/README.md) | OOP-style ledger — Dynamic ledger with constructor/destructor pattern |
| [LAB_7](LABS/LAB_7/README.md) | Static library calculator — Static archives (`.a`) and a stack-based calculator |
| [LAB_8](LABS/LAB_8/README.md) | Static library calculator (variant) — Static linking of a stack calculator |
| [LAB_9](LABS/LAB_9/README.md) | Shared library calculator — Dynamic shared objects (`.so`) |
| [LAB_10](LABS/LAB_10/README.md) | Modular dynamic library — Shared libraries and modular calculator design |
| [LAB_11](LABS/LAB_11/README.md) | File I/O and get_line — stdio vs POSIX I/O, reading lines safely |
| [LAB_12](LABS/LAB_12/README.md) | ncurses TUI — Terminal UI with ncurses (nano/mc-like demos) |
| [LAB_13](LABS/LAB_13/README.md) | Processes: fork/exec/wait — UNIX process model |
| [LAB_14](LABS/LAB_14/README.md) | POSIX threads — pthreads, mutexes, condition variables |
| [LAB_15](LABS/LAB_15/README.md) | Pipes and shell pipelines — Anonymous pipes and command pipelines |
| [LAB_16](LABS/LAB_16/README.md) | Message queues — POSIX and System V IPC message queues |
| [LAB_17](LABS/LAB_17/README.md) | Semaphores and shared memory — POSIX/System V semaphores and shared memory; chatroom |
| [LAB_18](LABS/LAB_18/README.md) | Sockets — TCP/UDP over AF_INET and AF_LOCAL |

## Requirements

- C toolchain (`cc`), GNU Make
- Optional: `valgrind`, `cppcheck`, `gdb`
- ncurses for LAB_12 and `LAB_17/task_shmem_chatroom` (`libncurses-dev`)
- Realtime extensions may be needed for POSIX MQ/sem/shm on some systems (`-lrt`)

## Documentation

- Each lab and task has its own `README.md`
- Makefile analysis and suggested fixes: [MAKEFILE_IMPROVEMENTS.md](MAKEFILE_IMPROVEMENTS.md)

## Backup

A pre-documentation snapshot lives in `_backup_pre_readme/` (created before README generation).
