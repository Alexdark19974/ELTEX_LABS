# Thread attributes

**Lab:** `LAB_14` · **Task:** `task_thread_opts`

## Goal

Experiments with pthread attributes / options (stack, detach state, etc. as coded).

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_14
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_14/task_thread_opts build
```

You can also `cd` into `LABS/LAB_14/task_thread_opts` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build && make run`

Expected binaries (under `bin/` of the build tree): `thread_opts`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_14/task_thread_opts clean
# or from root:
make clean_LAB_14
```
