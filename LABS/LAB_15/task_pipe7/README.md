# Mini bash with pipes

**Lab:** `LAB_15` · **Task:** `task_pipe7`

## Goal

Interactive shell prompt that can parse and run piped command lines.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_15
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_15/task_pipe7 build
```

You can also `cd` into `LABS/LAB_15/task_pipe7` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build && make run`

Expected binaries (under `bin/` of the build tree): `mini_bash`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_15/task_pipe7 clean
# or from root:
make clean_LAB_15
```
