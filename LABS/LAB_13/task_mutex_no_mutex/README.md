# Races vs mutex strategies

**Lab:** `LAB_13` · **Task:** `task_mutex_no_mutex`

## Goal

Multiple programs increment a shared counter with/without mutexes and with different locking granularities to show lost updates.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_13
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_13/task_mutex_no_mutex build
```

You can also `cd` into `LABS/LAB_13/task_mutex_no_mutex` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build`, then `PROG=<binary> make run` for each program in `bin/`.

Expected binaries (under `bin/` of the build tree): `thread_total_overwrite`, `thread_partial_overwrite`, `thread_mutex_coarse_grained`, `thread_mutex_fine_grained`, `thread_mutex_local_accum`, `thread_force_context_switch`, `thread_no_printf_as_sync_point`, `thread_overwrite_tmp_ptr`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_13/task_mutex_no_mutex clean
# or from root:
make clean_LAB_13
```
