# UDP AF_INET

**Lab:** `LAB_18` · **Task:** `task_udp_inet`

## Goal

Datagram sockets over IPv4 on port 7777.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_18
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_18/task_udp_inet build
```

You can also `cd` into `LABS/LAB_18/task_udp_inet` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

Start `./bin/server`, then `./bin/client`.

Expected binaries (under `bin/` of the build tree): `server`, `client`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_18/task_udp_inet clean
# or from root:
make clean_LAB_18
```
