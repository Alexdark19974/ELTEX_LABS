# LAB_16: Semaphores and shared memory

## Topic

POSIX/System V semaphores and shared memory; chatroom

## Summary

Sync with semaphores, exchange data via shared memory, plus an ncurses shared-memory chatroom.

## Tasks

| Task | Title |
|------|-------|
| `task_posix_semaphores_1` | POSIX sem: increment race |
| `task_posix_semaphores_2` | POSIX sem: consumer/supplier |
| `task_posix_shmem_1` | POSIX shm: reader/writer |
| `task_posix_shmem_2` | POSIX shm: client/server chatty exchange |
| `task_shmem_chatroom` | Shared-memory chatroom (ncurses) |
| `task_system_v_semaphores_1` | System V sem: increment |
| `task_system_v_semaphores_2` | System V sem: consumer/supplier |
| `task_system_v_shmem_1` | System V shm: reader/writer |
| `task_system_v_shmem_2` | System V shm: client/server |

## Build this lab

From repository root:

```bash
make build_LAB_16
```

Artifacts are produced under `build/LABS/LAB_16/…`.

## Clean

```bash
make clean_LAB_16
```

## See also

- Root [README.md](../../README.md) — global build help
- [MAKEFILE_IMPROVEMENTS.md](../../MAKEFILE_IMPROVEMENTS.md) — known Makefile issues and suggestions
