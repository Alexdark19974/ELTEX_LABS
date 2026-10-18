.EXPORT_ALL_VARIABLES:

ROOT_DIR := $(CURDIR)
BUILD_DIR := $(ROOT_DIR)/build
LAB_DIR := $(ROOT_DIR)/LABS
BUILD_LAB_DIR := $(BUILD_DIR)/LABS
# old solution
# LABS := $(shell find $(LAB_DIR)/* -maxdepth 0 -type d -printf "%f\n")
LABS := $(notdir $(patsubst %/,%,$(wildcard $(LAB_DIR)/*/)))
# $(wildcard $(LAB_DIR)/*/ produces only directories: /home/user/LABS/LAB_1/
# $(patsubst %/,%, ...) anything that ends in a slash is substituted without one: /home/user/LABS/LAB_1
# $(notdir ...) removes the directory path, leaving the file/folder name at the very end: LAB_1
CLEAN_LABS := $(addprefix clean_, $(LABS))
BUILD_LABS := $(addprefix build_, $(LABS))
CPPCHECK_LABS := $(addprefix cppcheck_, $(LABS))

include vars.make

.PHONY: build clean cppcheck list $(BUILD_LABS) build_all $(CLEAN_LABS) clean_all $(CPPCHECK_LABS) cppcheck_all clean_build help

# LAB comes from vars.make (default: all). Command line overrides it:
#   make build LAB=LAB_10
#   make clean
#   make cppcheck LAB=LAB_10 DEBUG=y
build: build_$(LAB)
clean: clean_$(LAB)
cppcheck: cppcheck_$(LAB)

# Print every lab and its task_* directories (sources under LABS/).
list:
	$(Q)for lab in $(LABS); do \
		echo "$$lab"; \
		for task in "$(LAB_DIR)/$$lab"/task_*/; do \
			[ -d "$$task" ] || continue; \
			echo "  $$(basename "$$task")"; \
		done; \
	done

build_all: $(BUILD_LABS)
	$(Q)echo "BUILDING $(LABS)...DONE"
$(BUILD_LABS): build_LAB_%:
	$(Q)echo "GOAL: $@"
	$(Q)$(MKDIR) -p $(BUILD_LAB_DIR)/LAB_$* || exit $$?
	$(Q)$(COPY) -ru $(LAB_DIR)/LAB_$*/* $(BUILD_LAB_DIR)/LAB_$*/ || exit $$?
	$(Q)$(MAKE) -C $(BUILD_LAB_DIR)/LAB_$* build TARGET_LAB=LAB_$* || exit $$?

clean_all: $(CLEAN_LABS)
$(CLEAN_LABS): clean_LAB_%:
	$(Q)echo "GOAL: $@"
	$(Q)if [ ! -d "$(BUILD_LAB_DIR)/LAB_$*" ]; then \
			echo "nothing to clean: $(BUILD_LAB_DIR)/LAB_$*"; \
		else \
			$(MAKE) -C $(BUILD_LAB_DIR)/LAB_$* clean TARGET_LAB=LAB_$* || exit $$?; \
		fi

cppcheck_all: $(CPPCHECK_LABS)
	$(Q)echo "CPPCHECK $(LABS)...DONE"
$(CPPCHECK_LABS): cppcheck_LAB_%:
	$(Q)echo "GOAL: $@"
	$(Q)$(MKDIR) -p $(BUILD_LAB_DIR)/LAB_$* || exit $$?
	$(Q)$(COPY) -ru $(LAB_DIR)/LAB_$*/* $(BUILD_LAB_DIR)/LAB_$*/ || exit $$?
	$(Q)$(MAKE) -C $(BUILD_LAB_DIR)/LAB_$* cppcheck TARGET_LAB=LAB_$* || exit $$?

clean_build:
	$(Q)echo "GOAL: $@"
	$(REMOVE) -rf $(BUILD_DIR) || exit $$?

.PHONY: help
help:
	$(Q)./helper.sh

