# Double pointer rebinding

**Lab:** `LAB_3` · **Task:** `task_ptr_ex3`

## Goal

Uses a pointer-to-pointer to change which object a pointer refers to.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_3
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_3/task_ptr_ex3 build
```

You can also `cd` into `LABS/LAB_3/task_ptr_ex3` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build && make run`

Expected binaries (under `bin/` of the build tree): `ex3`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_3/task_ptr_ex3 clean
# or from root:
make clean_LAB_3
```
