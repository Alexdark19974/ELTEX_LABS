# POSIX MQ: combined mode

**Lab:** `LAB_16` · **Task:** `task_posix_queues_3`

## Goal

Single binary that can act as sender and/or receiver (`print_usage` for flags).

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_16
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_16/task_posix_queues_3 build
```

You can also `cd` into `LABS/LAB_16/task_posix_queues_3` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build && ./bin/sender_receiver --help` (or as usage prints).

Expected binaries (under `bin/` of the build tree): `sender_receiver`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_16/task_posix_queues_3 clean
# or from root:
make clean_LAB_16
```
