# Compiler
CC = gcc

# Compiler flags
CFLAGS =  -pthread -lcunit

# Targets
TARGETS = init server client

# Source files
INIT_SRC = init.c
SERVER_SRC = server_func.c server_main.c
UNIT_TEST_SRC = unit_test.c server_func.c
CLIENT_SRC = client_main.c
HEADERS = globals.h book.h member.h server_func.h

# Binaries
INIT_BIN = init
SERVER_BIN = server
UNIT_TEST_BIN = unit_test
CLIENT_BIN = client

# Build all
all: $(TARGETS)

# Initialization binary
init: $(INIT_SRC) $(HEADERS)
	$(CC) $(CFLAGS) -o $(INIT_BIN) $(INIT_SRC)

# Server binary
server: $(SERVER_SRC) $(HEADERS)
	$(CC) $(CFLAGS) -o $(SERVER_BIN) $(SERVER_SRC)

# Unit test binary
unit_test: $(UNIT_TEST_SRC) $(HEADERS)
	$(CC) $(CFLAGS) -o $(UNIT_TEST_BIN) $(UNIT_TEST_SRC)

# Client binary
client: $(CLIENT_SRC) $(HEADERS)
	$(CC) $(CFLAGS) -o $(CLIENT_BIN) $(CLIENT_SRC)

# Run initialization before unit tests
test: init unit_test
	./$(INIT_BIN)
	./$(UNIT_TEST_BIN)

# Clean build artifacts
clean:
	rm -f $(INIT_BIN) $(SERVER_BIN) $(UNIT_TEST_BIN) $(CLIENT_BIN)

