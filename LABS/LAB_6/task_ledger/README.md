# Heap ledger with ctor/dtor

**Lab:** `LAB_6` · **Task:** `task_ledger`

## Goal

OOP-flavored ledger: construct/destroy objects, persist/load via filename prompts, manage entries dynamically.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_6
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_6/task_ledger build
```

You can also `cd` into `LABS/LAB_6/task_ledger` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build && make run`

Expected binary: `bin/ledger`.

## Clean

```bash
make -C build/LABS/LAB_6/task_ledger clean
# or from root:
make clean_LAB_6
```
