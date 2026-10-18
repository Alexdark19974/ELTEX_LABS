# System V MQ: combined mode

**Lab:** `LAB_15` · **Task:** `task_sys_v_queues_3`

## Goal

One program covering send/receive roles for System V queues.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_15
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_15/task_sys_v_queues_3 build
```

You can also `cd` into `LABS/LAB_15/task_sys_v_queues_3` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build && ./bin/sender_receiver` with args from `print_usage`.

Expected binaries (under `bin/` of the build tree): `sender_receiver`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_15/task_sys_v_queues_3 clean
# or from root:
make clean_LAB_15
```
