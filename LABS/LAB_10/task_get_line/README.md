# get_line + fopen/open modes

**Lab:** `LAB_10` · **Task:** `task_get_line`

## Goal

Builds shared libraries `libget_line.so` and `libio.so`, then links `print_reversed_string`, which reads a line and exercises `--fopen` / `--open` I/O modes.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_10
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_10/task_get_line build
```

You can also `cd` into `LABS/LAB_10/task_get_line` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

```bash
make build
LD_LIBRARY_PATH=./lib ./bin/print_reversed_string --fopen
LD_LIBRARY_PATH=./lib ./bin/print_reversed_string --open
```

Expected binary: `bin/print_reversed_string` (plus `lib/libget_line.so`, `lib/libio.so`).

## Clean

```bash
make -C build/LABS/LAB_10/task_get_line clean
# or from root:
make clean_LAB_10
```
