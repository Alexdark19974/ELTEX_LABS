# Display bytes of an unsigned int

**Lab:** `LAB_1` · **Task:** `task_bitwise_1`

## Goal

Prints each byte of an `unsigned int` using bitwise operations.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_1
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_1/task_bitwise_1 build
```

You can also `cd` into `LABS/LAB_1/task_bitwise_1` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build && make run`

Expected binaries (under `bin/` of the build tree): `bitwise_ex1`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_1/task_bitwise_1 clean
# or from root:
make clean_LAB_1
```
