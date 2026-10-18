# Mini nano editor

**Lab:** `LAB_11` · **Task:** `task_nano`

## Goal

Simplified ncurses text editor with init/deinit, redraw, and SIGWINCH handling.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_11
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_11/task_nano build
```

You can also `cd` into `LABS/LAB_11/task_nano` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build && make run`

Expected binaries (under `bin/` of the build tree): `nano`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_11/task_nano clean
# or from root:
make clean_LAB_11
```
