# LAB_18: Sockets

## Topic

TCP/UDP over AF_INET and AF_LOCAL

## Summary

Echo-style client/server pairs for Internet and UNIX-domain sockets, stream (TCP) and datagram (UDP).

## Tasks

| Task | Title |
|------|-------|
| `task_tcp_inet` | TCP AF_INET echo |
| `task_tcp_local` | TCP AF_LOCAL (UNIX stream) |
| `task_udp_inet` | UDP AF_INET |
| `task_udp_local` | UDP AF_LOCAL (UNIX datagram) |

## Build this lab

From repository root:

```bash
make build_LAB_18
```

Artifacts are produced under `build/LABS/LAB_18/…`.

## Clean

```bash
make clean_LAB_18
```

## See also

- Root [README.md](../../README.md) — global build help
- [MAKEFILE_IMPROVEMENTS.md](../../MAKEFILE_IMPROVEMENTS.md) — known Makefile issues and suggestions
