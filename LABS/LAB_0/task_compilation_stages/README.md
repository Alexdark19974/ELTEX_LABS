# Show compilation stages

**Lab:** `LAB_0` · **Task:** `task_compilation_stages`

## Goal

Compiles a trivial C program while explicitly emitting preprocessed (`.i`) and assembly (`.s`) intermediates alongside the object file.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_0
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_0/task_compilation_stages build
```

You can also `cd` into `LABS/LAB_0/task_compilation_stages` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

From the task dir (or the mirrored `build/LABS/LAB_0/task_compilation_stages`): `make build && make run`. Inspect `build/*.i` and `build/*.s`.

Expected binaries (under `bin/` of the build tree): `compilation_stages`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_0/task_compilation_stages clean
# or from root:
make clean_LAB_0
```
