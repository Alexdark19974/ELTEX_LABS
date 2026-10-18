# POSIX MQ: extended

**Lab:** `LAB_15` · **Task:** `task_posix_queues_2`

## Goal

Second POSIX queue exercise (priorities/attributes or flow as implemented).

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_15
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_15/task_posix_queues_2 build
```

You can also `cd` into `LABS/LAB_15/task_posix_queues_2` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

Run `receiver` then `sender` from `bin/` after `make build`.

Expected binaries (under `bin/` of the build tree): `sender`, `receiver`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_15/task_posix_queues_2 clean
# or from root:
make clean_LAB_15
```
