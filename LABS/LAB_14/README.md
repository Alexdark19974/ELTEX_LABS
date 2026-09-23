# LAB_14: POSIX threads

## Topic

pthreads, mutexes, condition variables

## Summary

Race conditions vs mutex strategies, `pthread_once`, condition variables, producer/consumer-style store, and threaded matrix multiply.

## Tasks

| Task | Title |
|------|-------|
| `task_cond_wait` | Condition variables |
| `task_customer` | Customer / store threads |
| `task_matrix` | Threaded matrix multiply |
| `task_mutex_no_mutex` | Races vs mutex strategies |
| `task_pthread_once` | pthread_once |
| `task_thread_opts` | Thread attributes |

## Build this lab

From repository root:

```bash
make build_LAB_14
```

Artifacts are produced under `build/LABS/LAB_14/…`.

## Clean

```bash
make clean_LAB_14
```

## See also

- Root [README.md](../../README.md) — global build help
- [MAKEFILE_IMPROVEMENTS.md](../../MAKEFILE_IMPROVEMENTS.md) — known Makefile issues and suggestions
