CC = gcc
CFLAGS = -g -Wall

SRC_FOLDER = src
TEST_FOLDER = test
BIN_FOLDER = bin

SRCS = $(wildcard $(SRC_FOLDER)/*.c)
TEST_SRCS = $(wildcard $(TEST_FOLDER)/*.c)

TEST_BIN = $(BIN_FOLDER)/test

all: $(BIN_FOLDER) $(TEST_BIN)
	$(info Running test...)
	@./$(TEST_BIN)

$(BIN_FOLDER):
	@mkdir -p $@

$(TEST_BIN): $(SRCS) $(TEST_SRCS)
	$(CC) $(CFLAGS) -D DEBUG $? -o $@

.PHONY: clean

clean:
	-rm -rf $(BIN_FOLDER)