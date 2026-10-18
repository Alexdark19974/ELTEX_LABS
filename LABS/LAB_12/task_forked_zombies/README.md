# Zombie processes

**Lab:** `LAB_12` · **Task:** `task_forked_zombies`

## Goal

Creates zombie children to illustrate unreaped exit statuses (`ps` / `wait`).

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_12
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_12/task_forked_zombies build
```

You can also `cd` into `LABS/LAB_12/task_forked_zombies` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

`make build && make run`; observe with `ps` in another terminal.

Expected binaries (under `bin/` of the build tree): `zombies`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_12/task_forked_zombies clean
# or from root:
make clean_LAB_12
```
