# Bytes via pointer

**Lab:** `LAB_3` · **Task:** `task_ptr_ex1`

## Goal

Displays each byte of an `unsigned int` by casting its address to `unsigned char *`, plus endianness.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_3
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_3/task_ptr_ex1 build
```

You can also `cd` into `LABS/LAB_3/task_ptr_ex1` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build && make run`

Expected binaries (under `bin/` of the build tree): `ex1`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_3/task_ptr_ex1 clean
# or from root:
make clean_LAB_3
```
