# Pipeline variant 4

**Lab:** `LAB_15` · **Task:** `task_pipe4`

## Goal

Further pipe/`dup2` practice connecting multiple processes.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_15
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_15/task_pipe4 build
```

You can also `cd` into `LABS/LAB_15/task_pipe4` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build` and follow program usage on stderr/stdout.

Expected binaries (under `bin/` of the build tree): `pipe4`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_15/task_pipe4 clean
# or from root:
make clean_LAB_15
```
