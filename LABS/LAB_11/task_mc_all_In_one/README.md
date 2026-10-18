# MC all-in-one

**Lab:** `LAB_11` · **Task:** `task_mc_all_In_one`

## Goal

Single-translation-unit variant of the Midnight Commander–style UI.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_11
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_11/task_mc_all_In_one build
```

You can also `cd` into `LABS/LAB_11/task_mc_all_In_one` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build && make run`

Expected binary: `bin/mc_all_in_one`.

## Clean

```bash
make -C build/LABS/LAB_11/task_mc_all_In_one clean
# or from root:
make clean_LAB_11
```
