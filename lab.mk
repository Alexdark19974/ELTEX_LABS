# Safety flags / toolchain (each make -C lab is a new process).
include $(ROOT_DIR)/vars.make

######### tasks ##########
TASKS := $(notdir $(patsubst %/,%,$(wildcard task_*/)))
BUILD_TASKS := $(addprefix build_,$(TASKS))
CLEAN_TASKS := $(addprefix clean_,$(TASKS))
CPPCHECK_TASKS := $(addprefix cppcheck_,$(TASKS))
##########################

.PHONY: build $(BUILD_TASKS) clean $(CLEAN_TASKS) cppcheck $(CPPCHECK_TASKS)
build: $(BUILD_TASKS)
$(BUILD_TASKS): build_%:
	$(MAKE) -C $(BUILD_LAB_DIR)/$(TARGET_LAB)/$* build

clean: $(CLEAN_TASKS)
$(CLEAN_TASKS): clean_%:
	$(MAKE) -C $(BUILD_LAB_DIR)/$(TARGET_LAB)/$* clean

cppcheck: $(CPPCHECK_TASKS)
$(CPPCHECK_TASKS): cppcheck_%:
	$(MAKE) -C $(BUILD_LAB_DIR)/$(TARGET_LAB)/$* cppcheck
