# POSIX shm: reader/writer

**Lab:** `LAB_17` · **Task:** `task_posix_shmem_1`

## Goal

`shm_open`/`mmap` writer and reader pair.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_17
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_17/task_posix_shmem_1 build
```

You can also `cd` into `LABS/LAB_17/task_posix_shmem_1` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

Run `shm_writer` and `shm_reader` (order per usage).

Expected binaries (under `bin/` of the build tree): `shm_writer`, `shm_reader`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_17/task_posix_shmem_1 clean
# or from root:
make clean_LAB_17
```
