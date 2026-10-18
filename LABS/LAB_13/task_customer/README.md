# Customer / store threads

**Lab:** `LAB_13` · **Task:** `task_customer`

## Goal

Threaded shopper/store simulations with synchronization between customers and stock.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_13
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_13/task_customer build
```

You can also `cd` into `LABS/LAB_13/task_customer` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build`; run `customer_store*` binaries.

Expected binaries (under `bin/` of the build tree): `customer_store`, `customer_store_2`, `customer_store_3`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_13/task_customer clean
# or from root:
make clean_LAB_13
```
