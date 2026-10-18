# Modular dynamic calculator

**Lab:** `LAB_9` · **Task:** `task_mod_dynamic_lib`

## Goal

Dynamic-library calculator with the stack split as a shared module; reinforces PIC and `-shared`.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_9
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_9/task_mod_dynamic_lib build
```

You can also `cd` into `LABS/LAB_9/task_mod_dynamic_lib` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build`, then `LD_LIBRARY_PATH=./lib ./bin/dynamic_calculator`.

Expected binaries (under `bin/` of the build tree): `dynamic_calculator`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_9/task_mod_dynamic_lib clean
# or from root:
make clean_LAB_9
```
