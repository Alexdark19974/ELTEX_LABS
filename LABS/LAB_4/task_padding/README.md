# Struct padding

**Lab:** `LAB_4` · **Task:** `task_padding`

## Goal

Illustrates how compiler padding/alignment affects struct size and member visibility in memory dumps.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_4
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_4/task_padding build
```

You can also `cd` into `LABS/LAB_4/task_padding` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build && make run`

Expected binaries (under `bin/` of the build tree): `struct_padding`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_4/task_padding clean
# or from root:
make clean_LAB_4
```
