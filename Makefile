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

include vars.make

.PHONY: $(BUILD_LABS) build_all $(CLEAN_LABS) clean_all clean_build help
build_all: $(BUILD_LABS)
	$(Q)echo "BUILDING $(LABS)...DONE"
$(BUILD_LABS): build_LAB_%:
	$(Q)echo "GOAL: $@"
	$(Q)if [ ! -d "$(BUILD_DIR)" ]; then \
			$(MKDIR) -p $(BUILD_DIR) || exit $?; \
			$(COPY) -rf $(LAB_DIR) $(BUILD_DIR) || exit $?; \
	fi
	$(COPY) -ru $(LAB_DIR)/LAB_$*/* $(BUILD_LAB_DIR)/LAB_$*/ || exit $?
	$(MAKE) -C $(BUILD_LAB_DIR)/LAB_$* build TARGET_LAB=LAB_$* || exit $?

clean_all: $(CLEAN_LABS)
$(CLEAN_LABS): clean_LAB_%:
	$(Q)echo "GOAL: $@"
	$(MAKE) -C $(BUILD_LAB_DIR)/LAB_$* clean TARGET_LAB=LAB_$* || exit $?

clean_build:
	$(Q)echo "GOAL: $@"
	$(REMOVE) -rf $(BUILD_DIR) || exit $?

.PHONY: help
help:
	$(Q)./helper.sh

