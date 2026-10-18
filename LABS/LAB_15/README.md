# LAB_15: Message queues

## Topic

POSIX and System V IPC message queues

## Summary

Sender/receiver pairs and combined modes for POSIX `mq_*` and System V `msgsnd`/`msgrcv` queues.

## Tasks

| Task | Title |
|------|-------|
| `task_posix_queues_1` | POSIX MQ: sender & receiver |
| `task_posix_queues_2` | POSIX MQ: extended |
| `task_posix_queues_3` | POSIX MQ: combined mode |
| `task_sys_v_queues_1` | System V MQ: sender & receiver |
| `task_sys_v_queues_2` | System V MQ: extended |
| `task_sys_v_queues_3` | System V MQ: combined mode |

## Build this lab

From repository root:

```bash
make build_LAB_15
```

Artifacts are produced under `build/LABS/LAB_15/…`.

## Clean

```bash
make clean_LAB_15
```

## See also

- Root [README.md](../../README.md) — global build help
- [MAKEFILE_IMPROVEMENTS.md](../../MAKEFILE_IMPROVEMENTS.md) — known Makefile issues and suggestions
