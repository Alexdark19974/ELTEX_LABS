############# directories #################
SRC_DIR := $(CURDIR)/src
BIN_DIR := $(CURDIR)/bin
BUILD_DIR := $(CURDIR)/build
INCLUDE_DIR := $(CURDIR)/include
LIB_DIR := $(CURDIR)/lib
###########################################

############# sources #####################
SRCS := $(wildcard $(SRC_DIR)/*.c)
###########################################

############# object files ################
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
###########################################

############# utilities ###################
include ../../../../vars.make
PROG := $(OBJS:$(BUILD_DIR)/%.o=%)
PROG ?= $(firstword $(PROG))
###########################################

############## flags & options #############
CPPCHECK_FLAGS := --inconclusive --language=c --std=c99 --enable=all --suppress=missingIncludeSystem .
CFLAGS := -std=c89 -Wno-deprecated-declarations -no-pie -fno-stack-protector
ifeq ($(DEBUG),y)
CFLAGS += -g3 -O0 -DDEBUG=1 -DNDEBUG=0
endif
###########################################

.PHONY: build run cppcheck clean
build: $(OBJS)
	$(Q)echo ""
	$(Q)echo "GOAL: $(BIN_DIR)/$(PROG)"
	$(Q)echo "PREREQ: $^"
	$(MKDIR) -p $(BIN_DIR) || exit $?
	$(CC) $(CFLAGS) $^ -o $(BIN_DIR)/$(PROG)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(Q)echo ""
	$(Q)echo "GOAL: $@"
	$(Q)echo "PREREQ: $^"
	$(MKDIR) -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $(BUILD_DIR)/$*.o

run: $(BIN_DIR)/$(PROG)
	$(Q)echo "Don't launch it via make. Copy & paste:"
	$(Q)echo "printf "AAAABBBBCCCCDDDDEEEE\xb5\x11\x40\x00\x00\x00\x00\x00" | $(BIN_DIR)/$(PROG)"	

cppcheck:
	$(Q)echo ""
	$(CPPCHECK) $(CPPCHECK_FLAGS) $(SRC_DIR)/*.c

clean:
	$(Q)echo ""
	rm -rf $(BUILD_DIR) $(BIN_DIR)
