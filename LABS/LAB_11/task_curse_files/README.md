# ncurses demos

**Lab:** `LAB_11` · **Task:** `task_curse_files`

## Goal

Collection of small ncurses programs (windows, input, mouse, colors, password field, etc.).

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_11
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_11/task_curse_files build
```

You can also `cd` into `LABS/LAB_11/task_curse_files` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build` then run a chosen binary from `bin/`, e.g. `PROG=cursedwindows make run`. Requires `libncurses`.

Expected binaries (under `bin/` of the build tree): `cursed`, `cursed2`, `cursedcolors`, `cursedinput`, `cursedmouse`, `cursedpassword`, `cursedwindows`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_11/task_curse_files clean
# or from root:
make clean_LAB_11
```
