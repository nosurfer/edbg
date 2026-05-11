CC       := g++
CFLAGS   := -Wall -Wextra -O3 -Iinclude
LDFLAGS  :=

SRC_DIR  := src
OBJ_DIR  := obj
BIN_DIR  := $(CURDIR)
TARGET   := $(BIN_DIR)/edbg

SRCS     := $(wildcard $(SRC_DIR)/*.cc)
OBJS     := $(SRCS:$(SRC_DIR)/%.cc=$(OBJ_DIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

run: all
	$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean run
