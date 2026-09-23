# TCP AF_INET echo

**Lab:** `LAB_18` · **Task:** `task_tcp_inet`

## Goal

Stream sockets over IPv4 (`AF_INET`, `SOCK_STREAM`): server listens, client connects and exchanges messages.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_18
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_18/task_tcp_inet build
```

You can also `cd` into `LABS/LAB_18/task_tcp_inet` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

Terminal A: `./bin/server`; Terminal B: `./bin/client`.

Expected binaries (under `bin/` of the build tree): `server`, `client`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_18/task_tcp_inet clean
# or from root:
make clean_LAB_18
```
