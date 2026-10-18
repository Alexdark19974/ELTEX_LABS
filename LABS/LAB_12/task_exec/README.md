# exec family

**Lab:** `LAB_12` · **Task:** `task_exec`

## Goal

Replaces the process image using `exec*` after fork (or directly as coded).

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_12
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_12/task_exec build
```

You can also `cd` into `LABS/LAB_12/task_exec` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build && make run`

Expected binaries (under `bin/` of the build tree): `exec`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_12/task_exec clean
# or from root:
make clean_LAB_12
```
