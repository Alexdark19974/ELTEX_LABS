# Calculator + static stack (variant)

**Lab:** `LAB_8` · **Task:** `task_static_calc`

## Goal

Same static-library calculator pattern as LAB_7 with a different link line in the Makefile.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_8
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_8/task_static_calc build
```

You can also `cd` into `LABS/LAB_8/task_static_calc` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build && make run`

Expected binaries (under `bin/` of the build tree): `static_calculator`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_8/task_static_calc clean
# or from root:
make clean_LAB_8
```
