# UDP AF_LOCAL (UNIX datagram)

**Lab:** `LAB_17` · **Task:** `task_udp_local`

## Goal

UNIX-domain datagram sockets with client/server pathnames under `/tmp`.

## Build

From the repository root (copies sources into `build/` then builds):

```bash
make build_LAB_17
```

Or build only this task after the lab tree exists under `build/`:

```bash
make -C build/LABS/LAB_17/task_udp_local build
```

You can also `cd` into `LABS/LAB_17/task_udp_local` and run `make build` if `vars.make` paths resolve (root build flow is the supported path).

## Run

Start `./bin/server`, then `./bin/client`. Clean up `/tmp/unix_*` socket files if needed.

Expected binaries (under `bin/` of the build tree): `server`, `client`.

For multi-binary tasks:

```bash
PROG=<name> make run
```

## Clean

```bash
make -C build/LABS/LAB_17/task_udp_local clean
# or from root:
make clean_LAB_17
```
