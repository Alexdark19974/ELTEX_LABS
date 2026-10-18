# Makefile analysis and suggested improvements

Backup of the tree prior to documentation changes: `_backup_pre_readme/`.

## How the build works today

1. Root `Makefile` copies `LABS/` into `build/LABS/` (initial full copy, then per-lab `cp -ru`).
2. It recurses into `build/LABS/LAB_N`; that lab Makefile is `include $(ROOT_DIR)/lab.mk`, which fans out to each `task_*`.
3. Task Makefiles include `../../../../vars.make`, compile `src/*.c` → `build/*.o` → `bin/<name>`.

This “mirror then build” model keeps sources in `LABS/` clean but duplicates the tree and makes path assumptions easy to break.

## Done

1. **Fortify flag** — all tasks use `-D_FORTIFY_SOURCE=2` (was `-FORTIFY_SOURCE=2` in some socket Makefiles).
2. **`DEBUG` unified** — tasks honor `DEBUG=y` from `vars.make` / `helper.sh` (no remaining `USE_DEBUG`).
3. **`-DDNDEBUG` typo** — no longer present in calculator Makefiles.
4. **Link libraries** — LAB_13 uses `CFLAGS += -pthread`; POSIX MQ / shm / sem tasks (LAB_15–16) pass `-lrt` on the link line only; LAB_8 / LAB_9 link with `-Wl,-rpath,'$$ORIGIN/../lib'` and set `LD_LIBRARY_PATH=$(LIB_DIR)` in `run` / `run_vlgrnd`.
5. **`clean` removes `lib/`** — LAB_7–10 library tasks use `rm -rf $(BUILD_DIR) $(BIN_DIR) $(LIB_DIR)`.
6. **Toolchain from `PATH`** — `vars.make` uses plain `cc`, `ar`, `valgrind`, etc. (not `/bin/cc`).
7. **Root convenience targets** — `make build` / `clean` / `cppcheck` with `LAB=` (default `all`) exist and match `helper.sh`.
8. **`clean_LAB_*` guard** — root clean skips missing `build/LABS/LAB_*` directories.
9. **`PROG=` documented** — root README and `helper.sh` mention `PROG=<name> make run` for multi-binary tasks.
10. **Shared `lab.mk`** — all `LABS/LAB_N/Makefile` files are `include $(ROOT_DIR)/lab.mk`.
11. **No build outputs under `LABS/`** — removed tracked `libstack.a`; `.gitignore` ignores `LABS/**/{bin,lib,build}/`, `*.a`, `*.so`.
12. **Shell exit status in recipes** — `|| exit $?` corrected to `|| exit $$?` (Make’s `$?` is not the shell status).
13. **`make list`** — root target prints each lab and its `task_*` directories.
14. **Safer recipes** — `vars.make` sets `.DELETE_ON_ERROR:`, `.ONESHELL:`, and `.SHELLFLAGS := -ec` (lab.mk includes `vars.make` so lab-level makes get the same).
15. **LAB merge + renumber** — removed duplicate static-lib lab; remaining labs are contiguous `LAB_0` … `LAB_17` (shared lib is LAB_8, sockets are LAB_17).

## Still open

### Design / DRY

1. **Most task Makefiles are copy-pasted** — extract `task-simple.mk`, `task-multi-bin.mk`, `task-lib.mk` templates; tasks only set `PROG`, `LDLIBS`, extras.
2. **No header dependencies** — changing an `.h` does not rebuild `.o` files. Use `-MMD -MP` and `-include *.d`.
3. **`PROG ?= $(firstword $(PROGS))` is order-dependent** — `wildcard` order is filesystem-dependent, so `make run` may start `client` or `server` unpredictably.
4. **Parallel make (`-j`) races** on `mkdir -p` are mostly OK, but linking multi-step libs without order-only deps can still flake; add explicit ordering / `.WAIT` where needed.
5. **`.EXPORT_ALL_VARIABLES`** at root exports everything into recipes — surprising and can leak make metadata into tools; prefer exporting an explicit list.

## Suggested incremental refactor

```make
# tasks/common-flags.mk
CFLAGS += -Wall -Wextra -std=c99 -pedantic -Wformat -Wformat-security
ifeq ($(DEBUG),y)
  CFLAGS += -g3 -O0 -DDEBUG=1
else
  CFLAGS += -O2 -D_FORTIFY_SOURCE=2 -fstack-protector-strong
endif
```

## Priority order (remaining)

1. Deduplicate task Makefiles via shared `*.mk` includes
2. Auto dependency generation (`-MMD`)
