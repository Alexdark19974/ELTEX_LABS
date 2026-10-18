# TCP AF_LOCAL (UNIX stream)

**Lab:** `LAB_17` · **Task:** `task_tcp_local`

## Goal

Stream sockets on UNIX domain path `/tmp/unix_server_sock`.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_17
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_17/task_tcp_local build
```

You can also `cd` into `LABS/LAB_17/task_tcp_local` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

Start `./bin/server`, then `./bin/client`. Remove stale socket file if bind fails.

Expected binaries (under `bin/` of the build tree): `server`, `client`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_17/task_tcp_local clean
# or from root:
make clean_LAB_17
```
