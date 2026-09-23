# Two-command pipeline

**Lab:** `LAB_15` · **Task:** `task_pipe2`

## Goal

Implements `cmd1 | cmd2` style execution (usage expects `|` and enough argv tokens).

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_15
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_15/task_pipe2 build
```

You can also `cd` into `LABS/LAB_15/task_pipe2` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build && ./bin/pipe2 ls -la | head -n4` (as printed by the program usage).

Expected binaries (under `bin/` of the build tree): `pipe2`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_15/task_pipe2 clean
# or from root:
make clean_LAB_15
```
