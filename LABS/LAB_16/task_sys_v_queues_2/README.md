# System V MQ: extended

**Lab:** `LAB_16` · **Task:** `task_sys_v_queues_2`

## Goal

Second System V queue exercise building on typed messages.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_16
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_16/task_sys_v_queues_2 build
```

You can also `cd` into `LABS/LAB_16/task_sys_v_queues_2` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

Start `receiver`, then `sender`.

Expected binaries (under `bin/` of the build tree): `sender`, `receiver`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_16/task_sys_v_queues_2 clean
# or from root:
make clean_LAB_16
```
