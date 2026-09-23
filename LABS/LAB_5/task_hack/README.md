# Password overflow demo

**Lab:** `LAB_5` · **Task:** `task_hack`

## Goal

Educational stack-smash demo around unsafe `gets()`. Do not reuse this pattern in real code.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_5
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_5/task_hack build
```

You can also `cd` into `LABS/LAB_5/task_hack` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build && make run`

Expected binaries (under `bin/` of the build tree): `hack_pass`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Notes

Lab only: demonstrates why bounded input APIs are required.

## Clean

```bash
make -C build/LABS/LAB_5/task_hack clean
# or from root:
make clean_LAB_5
```
