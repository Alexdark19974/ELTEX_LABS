# Makefile analysis and suggested improvements

Backup of the tree prior to documentation changes: `_backup_pre_readme/`.

## How the build works today

1. Root `Makefile` copies `LABS/` into `build/LABS/` (initial full copy, then per-lab `cp -ru`).
2. It recurses into `build/LABS/LAB_N` which fans out to each `task_*`.
3. Task Makefiles include `../../../../vars.make`, compile `src/*.c` → `build/*.o` → `bin/<name>`.

This “mirror then build” model keeps sources in `LABS/` clean but duplicates the tree and makes path assumptions easy to break.

## Issues found

### Correctness / flags

1. **`-FORTIFY_SOURCE=2` is wrong** — should be `-D_FORTIFY_SOURCE=2`. As written it is a no-op filename-style flag and does not enable Fortify.
2. **`DEBUG` vs `USE_DEBUG` mismatch** — `vars.make` / most tasks use `DEBUG`; LAB_6–10 calculators use `USE_DEBUG`. `helper.sh` documents `USE_DEBUG`, so `DEBUG=y make …` silently does nothing on those labs.
3. **Typo `-DDNDEBUG=0`** in several calculator Makefiles (should be `-DNDEBUG=0` or simply omit NDEBUG in debug builds).
4. **Missing link libraries** in many task Makefiles:
   - LAB_14 (and other pthread code) often omit `-pthread` / `-lpthread`
   - POSIX MQ / shm / sem tasks often omit `-lrt` (needed on older glibc)
   - Dynamic calculators need runtime `LD_LIBRARY_PATH` or `-Wl,-rpath,$ORIGIN/../lib`
5. **`clean` does not remove `lib/`** for static/shared library tasks (stale `libstack.a` / `.so` remain).
6. **Root `clean_LAB_*` fails** if `build/LABS/LAB_*` was never created (no guard).
7. **Hardcoded `/bin/cc`, `/bin/valgrind`, …** in `vars.make` — prefer `$(shell command -v cc)` or plain `cc` from `PATH` (Nix, Homebrew, custom prefixes).

### Design / DRY

8. **All lab-level Makefiles are identical** — replace with one shared `lab.mk` included from each lab.
9. **Most task Makefiles are copy-pasted** — extract `task-simple.mk`, `task-multi-bin.mk`, `task-lib.mk` templates; tasks only set `PROG`, `LDLIBS`, extras.
10. **No header dependencies** — changing an `.h` does not rebuild `.o` files. Use `-MMD -MP` and `-include *.d`.
11. **`PROG ?= $(firstword $(PROGS))` is order-dependent** — `wildcard` order is filesystem-dependent, so `make run` may start `client` or `server` unpredictably.
12. **Parallel make (`-j`) races** on `mkdir -p` are mostly OK, but linking multi-step libs without order-only deps can still flake; add explicit ordering / `.WAIT` where needed.
13. **Root targets vs help text** — `helper.sh` describes `make build LAB=…` / `make clean` / `make cppcheck`, but root only defines `build_all`, `build_LAB_%`, `clean_all`, `clean_LAB_%`, `clean_build`, `help`. Either add the documented targets or fix the helper.
14. **`.EXPORT_ALL_VARIABLES`** at root exports everything into recipes — surprising and can leak make metadata into tools; prefer exporting an explicit list.

### UX

15. Document `PROG=server make run` for multi-binary tasks (client/server, sender/receiver).
16. Add a root `make list` printing labs/tasks.
17. Prefer `CFLAGS += -pthread` over bare `-lpthread` (sets compile and link semantics).
18. Add `.DELETE_ON_ERROR:` and `.ONESHELL:` carefully for safer recipes.
19. Stop committing/keeping build outputs under `LABS/**/bin` and `LABS/**/lib` if present; keep them only under `build/` or gitignore them.
20. LAB_7 vs LAB_8 are nearly duplicates — clarify in READMEs which Makefile difference is intentional or merge.

## Suggested incremental refactor

```make
# labs/common-lab.mk
TASKS := $(notdir $(patsubst %/,%,$(wildcard task_*/)))
build: $(addprefix build_,$(TASKS))
build_%:
	$(MAKE) -C $(BUILD_LAB_DIR)/$(TARGET_LAB)/$* build
```

```make
# tasks/common-flags.mk
CFLAGS += -Wall -Wextra -std=c99 -pedantic -Wformat -Wformat-security
ifeq ($(DEBUG),y)
  CFLAGS += -g3 -O0 -DDEBUG=1
else
  CFLAGS += -O2 -D_FORTIFY_SOURCE=2 -fstack-protector-strong
endif
```

Root convenience wrappers:

```make
build:
ifeq ($(LAB),all)
	$(MAKE) build_all
else
	$(MAKE) build_$(LAB)
endif
```

## Priority order

1. Fix `-D_FORTIFY_SOURCE=2` and unify `DEBUG`/`USE_DEBUG`
2. Align `helper.sh` with real root targets (or add aliases)
3. Add `-pthread` / `-lrt` / rpath where required
4. Deduplicate via shared `*.mk` includes
5. Auto dependency generation (`-MMD`)
