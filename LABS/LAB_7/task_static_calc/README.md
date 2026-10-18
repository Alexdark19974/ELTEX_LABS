# Calculator + libstack.a

**Lab:** `LAB_7` · **Task:** `task_static_calc`

## Goal

Stack ADT compiled into `lib/libstack.a`; calculator links statically against it.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_7
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_7/task_static_calc build
```

You can also `cd` into `LABS/LAB_7/task_static_calc` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build && make run` (RPN/stack calculator UI as implemented).

Expected binaries (under `bin/` of the build tree): `static_calculator`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_7/task_static_calc clean
# or from root:
make clean_LAB_7
```
