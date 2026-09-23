# Modify a selected byte

**Lab:** `LAB_1` · **Task:** `task_bitwise_2`

## Goal

Shows endianness, then lets you replace one of the four bytes of an `unsigned int`.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_1
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_1/task_bitwise_2 build
```

You can also `cd` into `LABS/LAB_1/task_bitwise_2` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build && make run` (interactive prompts)

Expected binaries (under `bin/` of the build tree): `bitwise_ex2`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_1/task_bitwise_2 clean
# or from root:
make clean_LAB_1
```
