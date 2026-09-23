############# directories #################
SRC_DIR := $(CURDIR)/src
BIN_DIR := $(CURDIR)/bin
BUILD_DIR := $(CURDIR)/build
INCLUDE_DIR := $(CURDIR)/include
LIB_DIR := $(CURDIR)/lib
###########################################

############# sources #####################
SRCS := $(SRC_DIR)/main.c
GET_LINE_SRCS := $(SRC_DIR)/get_line.c
IO_SRCS := $(SRC_DIR)/fopen.c $(SRC_DIR)/open.c
###########################################

###########################################

############# object files ################
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
STACK_OBJS := $(STACK_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
GET_LINE_OBJS := $(GET_LINE_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
IO_OBJS := $(IO_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
###########################################

############# utilities ###################
include ../../../../vars.make
PROG := print_reversed_string
LIB_GET_LINE := libget_line
GET_LINE := get_line
LIB_IO := libio
IO := io
###########################################

############## flags & options #############
VLGRND_FLAGS := -s --show-leak-kinds=all --track-origins=yes --leak-check=full
CPPCHECK_FLAGS := --inconclusive --language=c --std=c99 --enable=all --suppress=missingIncludeSystem .
CFLAGS := -Wall -Wextra -Werror -std=c99 -pedantic -Wformat -Wformat-security
CFLAGS += -I$(INCLUDE_DIR)
ifeq ($(DEBUG),y)
CFLAGS += -g3 -O0 -DDEBUG=1
else
CFLAGS += -O2 -D_FORTIFY_SOURCE=2 -DNDEBUG=1 -fstack-protector-strong -fstack-clash-protection
endif
LDFLAGS += -fPIC
###########################################

.PHONY: build build_lib run run_vlgrnd cppcheck clean
build: $(OBJS) $(LIB_DIR)/$(LIB_GET_LINE).$(SO_EXT) $(LIB_DIR)/$(LIB_IO).$(SO_EXT)
	$(Q)echo ""
	$(Q)echo "GOAL: $(BIN_DIR)/$(PROG)"
	$(Q)echo "PREREQ: $^"
	$(MKDIR) -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJS) -o $(BIN_DIR)/$(PROG) $(LDFLAGS) -L$(LIB_DIR) -l$(IO) -l$(GET_LINE) -Wl,-z,relro,-z,now

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(Q)echo ""
	$(Q)echo "GOAL: $@"
	$(Q)echo "PREREQ: $^"
	$(MKDIR) -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

$(LIB_DIR)/$(LIB_GET_LINE).$(SO_EXT): $(GET_LINE_OBJS)
	$(Q)echo ""
	$(Q)echo "GOAL: $@"
	$(Q)echo "PREREQ: $^"
	$(MKDIR) -p $(LIB_DIR)
	$(CC) -shared $^ -o $@ $(LDFLAGS)

$(LIB_DIR)/$(LIB_IO).$(SO_EXT): $(IO_OBJS)
	$(Q)echo ""
	$(Q)echo "GOAL: $@"
	$(Q)echo "PREREQ: $^"
	$(MKDIR) -p $(LIB_DIR)
	$(CC) -shared $^ -o $@ $(LDFLAGS)

run: $(BIN_DIR)/$(PROG)
	$(BIN_DIR)/$(PROG)

run_vlgrnd: $(BIN_DIR)/$(PROG)
	$(VLGRND) $(VLGRND_FLAGS) $(BIN_DIR)/$(PROG)

cppcheck:
	$(Q)echo ""
	$(CPPCHECK) $(CPPCHECK_FLAGS) $(SRC_DIR)/*.c

clean:
	$(Q)echo ""
	rm -rf $(BUILD_DIR) $(BIN_DIR) $(LIB_DIR)
