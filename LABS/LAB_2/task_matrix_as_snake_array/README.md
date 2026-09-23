# Snake-fill matrix (array variant)

**Lab:** `LAB_2` · **Task:** `task_matrix_as_snake_array`

## Goal

Same snake-fill idea with an alternate array-oriented implementation.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_2
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_2/task_matrix_as_snake_array build
```

You can also `cd` into `LABS/LAB_2/task_matrix_as_snake_array` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build && make run`

Expected binaries (under `bin/` of the build tree): `matrix_as_snake`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_2/task_matrix_as_snake_array clean
# or from root:
make clean_LAB_2
```
