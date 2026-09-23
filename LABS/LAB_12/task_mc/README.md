# Mini Midnight Commander

**Lab:** `LAB_12` · **Task:** `task_mc`

## Goal

Dual-pane file-manager style TUI built with ncurses.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_12
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_12/task_mc build
```

You can also `cd` into `LABS/LAB_12/task_mc` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build && make run`

Expected binaries (under `bin/` of the build tree): `mc`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_12/task_mc clean
# or from root:
make clean_LAB_12
```
