# Calculator + libstack.so

**Lab:** `LAB_8` · **Task:** `task_dynamic_calc`

## Goal

Builds a shared stack library and links `dynamic_calculator` with `-lstack` and RELRO flags.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_8
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_8/task_dynamic_calc build
```

You can also `cd` into `LABS/LAB_8/task_dynamic_calc` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build` then run with library path, e.g. `LD_LIBRARY_PATH=./lib ./bin/dynamic_calculator` (or `make run` if rpath/env is set).

Expected binaries (under `bin/` of the build tree): `dynamic_calculator`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Notes

Without `LD_LIBRARY_PATH` or `-Wl,-rpath`, the loader may fail to find `libstack.so`.

## Clean

```bash
make -C build/LABS/LAB_8/task_dynamic_calc clean
# or from root:
make clean_LAB_8
```
