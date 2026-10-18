# System V sem: consumer/supplier

**Lab:** `LAB_16` · **Task:** `task_system_v_semaphores_2`

## Goal

Consumer/supplier with System V semaphore sets.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_16
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_16/task_system_v_semaphores_2 build
```

You can also `cd` into `LABS/LAB_16/task_system_v_semaphores_2` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build && ./bin/consumer_supplier` — see usage.

Expected binaries (under `bin/` of the build tree): `consumer_supplier`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_16/task_system_v_semaphores_2 clean
# or from root:
make clean_LAB_16
```
