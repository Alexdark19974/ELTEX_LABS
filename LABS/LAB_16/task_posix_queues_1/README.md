# POSIX MQ: sender & receiver

**Lab:** `LAB_16` · **Task:** `task_posix_queues_1`

## Goal

Basic POSIX message queue: separate `sender` and `receiver` processes (`mq_open`/`mq_send`/`mq_receive`).

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_16
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_16/task_posix_queues_1 build
```

You can also `cd` into `LABS/LAB_16/task_posix_queues_1` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

Terminal A: `./bin/receiver`; Terminal B: `./bin/sender`. Build with `make build`.

Expected binaries (under `bin/` of the build tree): `sender`, `receiver`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Notes

May need `-lrt` on older glibc; ensure queue names match between programs.

## Clean

```bash
make -C build/LABS/LAB_16/task_posix_queues_1 clean
# or from root:
make clean_LAB_16
```
