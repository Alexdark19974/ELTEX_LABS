# pthread_once

**Lab:** `LAB_14` · **Task:** `task_pthread_once`

## Goal

Ensures one-time initialization across threads via `pthread_once`.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_14
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_14/task_pthread_once build
```

You can also `cd` into `LABS/LAB_14/task_pthread_once` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build` and run each `pthread_once*` binary from `bin/`.

Expected binaries (under `bin/` of the build tree): `pthread_once`, `pthread_once_2`, `pthread_once_3`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_14/task_pthread_once clean
# or from root:
make clean_LAB_14
```
