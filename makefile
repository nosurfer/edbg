# make       - build
# make run   - build and run
# make clean - remove build setup

CC       := g++
CFLAGS   := -Wall -Wextra -O3 -Iinclude
LDFLAGS  :=

SRC_DIR  := src
OBJ_DIR  := obj
BIN_DIR  := $(CURDIR)
TARGET   := $(BIN_DIR)/edbg

# explicity wildcard keyword used for formating works
SRCS     := $(wildcard $(SRC_DIR)/*.cc)
# syntax for convert SRCS list with .cc extensions to .o extensions 
OBJS     := $(SRCS:$(SRC_DIR)/%.cc=$(OBJ_DIR)/%.o)

all: $(TARGET)
# $^ expression means taking entirely list to command
$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@
# $< expression means taking only first element
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@
# create OBJ_DIR only if it doesn't exist
$(OBJ_DIR):
	mkdir -p $@
# MAKECMDGOALS includes argvs, filter-out exclude argv[0]
RUN_ARGS := $(filter-out run,$(MAKECMDGOALS))
run: all
	$(TARGET) $(RUN_ARGS)
# pass through next argvs without getting warns
%:
	@:

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean run
