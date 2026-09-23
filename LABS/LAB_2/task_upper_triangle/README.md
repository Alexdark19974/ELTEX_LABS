# Upper-triangle fill

**Lab:** `LAB_2` · **Task:** `task_upper_triangle`

## Goal

Fills the upper triangle with 1s and the lower with 0s, then prints the matrix.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_2
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_2/task_upper_triangle build
```

You can also `cd` into `LABS/LAB_2/task_upper_triangle` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build && make run`

Expected binaries (under `bin/` of the build tree): `upper_triangle`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_2/task_upper_triangle clean
# or from root:
make clean_LAB_2
```
