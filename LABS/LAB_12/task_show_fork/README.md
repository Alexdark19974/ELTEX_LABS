# Show fork

**Lab:** `LAB_12` · **Task:** `task_show_fork`

## Goal

Demonstrates `fork()` and how parent/child execute after the split.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_12
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_12/task_show_fork build
```

You can also `cd` into `LABS/LAB_12/task_show_fork` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build && make run`

Expected binaries (under `bin/` of the build tree): `fork_show`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_12/task_show_fork clean
# or from root:
make clean_LAB_12
```
