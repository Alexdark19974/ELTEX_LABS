# Reverse matrix

**Lab:** `LAB_2` · **Task:** `task_reverse_matrix`

## Goal

Fills a matrix, reverses it (by rows/layout as implemented), and prints before/after.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_2
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_2/task_reverse_matrix build
```

You can also `cd` into `LABS/LAB_2/task_reverse_matrix` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build && make run`

Expected binaries (under `bin/` of the build tree): `reverse_matrix`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_2/task_reverse_matrix clean
# or from root:
make clean_LAB_2
```
