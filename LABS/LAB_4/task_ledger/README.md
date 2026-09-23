# Address book (ledger)

**Lab:** `LAB_4` · **Task:** `task_ledger`

## Goal

Interactive contact book: add/remove/find/display entries using structs and function pointers.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_4
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_4/task_ledger build
```

You can also `cd` into `LABS/LAB_4/task_ledger` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build && make run` then follow on-screen commands.

Expected binary: `bin/address_book`.

## Clean

```bash
make -C build/LABS/LAB_4/task_ledger clean
# or from root:
make clean_LAB_4
```
