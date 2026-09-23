# LAB_5: Stack buffer overflow demo

## Topic

Unsafe input and stack smashing (educational)

## Summary

Intentionally vulnerable password check (`gets`) used to illustrate how overflow can overwrite control data. For learning only.

## Tasks

| Task | Title |
|------|-------|
| `task_hack` | Password overflow demo |

## Build this lab

From repository root:

```bash
make build_LAB_5
```

Artifacts are produced under `build/LABS/LAB_5/…`.

## Clean

```bash
make clean_LAB_5
```

## See also

- Root [README.md](../../README.md) — global build help
- [MAKEFILE_IMPROVEMENTS.md](../../MAKEFILE_IMPROVEMENTS.md) — known Makefile issues and suggestions
