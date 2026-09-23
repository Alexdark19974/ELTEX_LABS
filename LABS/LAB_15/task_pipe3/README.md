# N-command pipeline

**Lab:** `LAB_15` · **Task:** `task_pipe3`

## Goal

Generalizes piping across an arbitrary number of `|`-separated commands.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_15
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_15/task_pipe3 build
```

You can also `cd` into `LABS/LAB_15/task_pipe3` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build` then pass a pipeline on the command line.

Expected binaries (under `bin/` of the build tree): `pipe3`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_15/task_pipe3 clean
# or from root:
make clean_LAB_15
```
