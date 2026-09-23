# System V sem: increment

**Lab:** `LAB_17` · **Task:** `task_system_v_semaphores_1`

## Goal

Increment demo synchronized with System V semaphores (`semget`/`semop`).

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_17
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_17/task_system_v_semaphores_1 build
```

You can also `cd` into `LABS/LAB_17/task_system_v_semaphores_1` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build && ./bin/incr_prog` — see usage.

Expected binaries (under `bin/` of the build tree): `incr_prog`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_17/task_system_v_semaphores_1 clean
# or from root:
make clean_LAB_17
```
