# make       - build
# make clean - remove build setup
# make rm    - remove build setup and spdlog lib

CC      := g++
CFLAGS  := -Wall -std=c++26 -Wextra -O3 -Iinclude 
LDFLAGS := -lcapstone -lreadline
# LOGS    := err

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := $(CURDIR)
TARGET  := $(BIN_DIR)/edbg

# explicity wildcard keyword used for formating works
SRCS    := $(wildcard $(SRC_DIR)/*.cc)
# syntax for convert SRCS list with .cc extensions to .o extensions 
OBJS    := $(SRCS:$(SRC_DIR)/%.cc=$(OBJ_DIR)/%.o)

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

pull:
# 	@rm -rf /tmp/spdlog_tmp
# 	@git clone --depth 1 --filter=blob:none --sparse https://github.com/gabime/spdlog.git /tmp/spdlog_tmp
# 	@git -C /tmp/spdlog_tmp sparse-checkout set include/spdlog
# 	@mkdir -p include
# 	@rm -rf include/spdlog
# 	@cp -a /tmp/spdlog_tmp/include/spdlog include/spdlog
# 	@rm -rf /tmp/spdlog_tmp

	@rm -rf /tmp/cli11_tmp
	@git clone --depth 1 --branch v2.4.2 https://github.com/CLIUtils/CLI11.git /tmp/cli11_tmp
	@mkdir -p include/CLI
	@cp -a /tmp/cli11_tmp/include/CLI/. include/CLI/
	@rm -rf /tmp/cli11_tmp

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

rm: clean
	rm -rf include/spdlog

.PHONY: all clean run spdlog
