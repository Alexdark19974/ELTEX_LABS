# Condition variables

**Lab:** `LAB_13` · **Task:** `task_cond_wait`

## Goal

Variants of `pthread_cond_wait` / `pthread_cond_signal` producer–consumer style sync.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_13
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_13/task_cond_wait build
```

You can also `cd` into `LABS/LAB_13/task_cond_wait` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build`; run `cond_wait_signal_*` from `bin/` (`PROG=... make run`).

Expected binaries (under `bin/` of the build tree): `cond_wait_signal_1`, `cond_wait_signal_2`, `cond_wait_signal_3`, `cond_wait_signal_4`, `cond_wait_signal_5`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_13/task_cond_wait clean
# or from root:
make clean_LAB_13
```
