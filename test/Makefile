CC = gcc
CFLAGS = -Wall -Wextra -g


BUILD_DIR = test_build
BIN_DIR = test_bin
TEST_SRC_DIR = test_src

SRC_DIR = ../src
SRC_LIB_DIR = $(SRC_DIR)/m_http

TARGET = $(BIN_DIR)/test__judge_Internet

all: $(TARGET)

$(TARGET): $(BUILD_DIR)/m_net.o $(BUILD_DIR)/m_url_process.o $(BUILD_DIR)/m_socks5_client.o $(BUILD_DIR)/m_http.o $(BUILD_DIR)/judge_Internet.o $(BUILD_DIR)/test__judge_Internet.o
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_LIB_DIR)/m_net/%.c
	$(CC) $(CFLAGS) -o $@ -c $<
$(BUILD_DIR)/%.o: $(SRC_LIB_DIR)/m_url_process/%.c
	$(CC) $(CFLAGS) -o $@ -c $<
$(BUILD_DIR)/%.o: $(SRC_LIB_DIR)/m_socks5/%.c
	$(CC) $(CFLAGS) -o $@ -c $<
$(BUILD_DIR)/%.o: $(SRC_LIB_DIR)/%.c
	$(CC) $(CFLAGS) -o $@ -c $<
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -o $@ -c $<
$(BUILD_DIR)/%.o: $(TEST_SRC_DIR)/%.c
	$(CC) $(CFLAGS) -o $@ -c $<


clean:
	rm -rf $(BIN_DIR)/* $(BUILD_DIR)/*
.PHONY: all clean
