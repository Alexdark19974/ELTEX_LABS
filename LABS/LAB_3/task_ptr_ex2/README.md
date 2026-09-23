# Change a byte via pointer

**Lab:** `LAB_3` · **Task:** `task_ptr_ex2`

## Goal

Interactively pick a byte index and new value [0–255] and mutate the integer through a pointer.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_3
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_3/task_ptr_ex2 build
```

You can also `cd` into `LABS/LAB_3/task_ptr_ex2` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build && make run`

Expected binaries (under `bin/` of the build tree): `ex2`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_3/task_ptr_ex2 clean
# or from root:
make clean_LAB_3
```
