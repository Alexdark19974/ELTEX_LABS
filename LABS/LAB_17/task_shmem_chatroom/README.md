# Shared-memory chatroom (ncurses)

**Lab:** `LAB_17` · **Task:** `task_shmem_chatroom`

## Goal

Multi-user chatroom using shared memory + ncurses UI (client/server roles).

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_17
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_17/task_shmem_chatroom build
```

You can also `cd` into `LABS/LAB_17/task_shmem_chatroom` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

```bash
make build
./bin/server    # first
PROG=client make run   # or ./bin/client
```

May prompt for a password. Needs `libncurses`. Binaries: `bin/server`, `bin/client`.

## Clean

```bash
make -C build/LABS/LAB_17/task_shmem_chatroom clean
# or from root:
make clean_LAB_17
```
