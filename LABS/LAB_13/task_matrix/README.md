# Threaded matrix multiply

**Lab:** `LAB_13` · **Task:** `task_matrix`

## Goal

Parallel matrix multiplication using pthreads.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_13
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_13/task_matrix build
```

You can also `cd` into `LABS/LAB_13/task_matrix` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build && make run`

Expected binaries (under `bin/` of the build tree): `matrix_multiplication`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_13/task_matrix clean
# or from root:
make clean_LAB_13
```
