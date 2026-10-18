# LAB_8: Shared library calculator

## Topic

Dynamic shared objects (`.so`)

## Summary

Builds `libstack.so` with `-shared -fPIC` and links the calculator dynamically (`-L… -lstack`).

## Tasks

| Task | Title |
|------|-------|
| `task_dynamic_calc` | Calculator + libstack.so |

## Build this lab

From repository root:

```bash
make build_LAB_8
```

Artifacts are produced under `build/LABS/LAB_8/…`.

## Clean

```bash
make clean_LAB_8
```

## See also

- Root [README.md](../../README.md) — global build help
- [MAKEFILE_IMPROVEMENTS.md](../../MAKEFILE_IMPROVEMENTS.md) — known Makefile issues and suggestions
