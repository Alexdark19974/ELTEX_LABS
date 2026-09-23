# System V MQ: sender & receiver

**Lab:** `LAB_16` · **Task:** `task_sys_v_queues_1`

## Goal

Uses `ftok`/`msgget`/`msgsnd`/`msgrcv` with a structured message type.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_16
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_16/task_sys_v_queues_1 build
```

You can also `cd` into `LABS/LAB_16/task_sys_v_queues_1` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

Start `receiver`, then `sender`.

Expected binaries (under `bin/` of the build tree): `sender`, `receiver`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_16/task_sys_v_queues_1 clean
# or from root:
make clean_LAB_16
```
