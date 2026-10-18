# Parent/child pipe exchange

**Lab:** `LAB_14` · **Task:** `task_pipe1`

## Goal

Creates a pipe; child/parent exchange argv payload over the pipe.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_14
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_14/task_pipe1 build
```

You can also `cd` into `LABS/LAB_14/task_pipe1` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build && ./bin/pipe Hello` (requires an argument).

Expected binaries (under `bin/` of the build tree): `pipe`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_14/task_pipe1 clean
# or from root:
make clean_LAB_14
```
