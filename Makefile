# Flags and dependencies are provided by nix development shell

TARGET = $(TARGET_DIR)/$(TARGET_NAME)
OBJ_FILES = $(SRC_FILES:.c=.o)

# Default command (since it's first), depends on the game executable
all: $(TARGET)

# Create bin directory as an order-only prerequisite (won't trigger rebuild)
ensure_target_dir:
	mkdir -p $(TARGET_DIR)

# Build the game executable, depends on the object files
$(TARGET): $(OBJ_FILES) | ensure_target_dir
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ_FILES) -o $(TARGET) 

# Build object files from source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	
.PHONY: clean lsp ensure_target_dir
clean:
	rm -rf $(OBJ_FILES) $(TARGET)

# Generate compile_commands.json for LSP
lsp:
	bear -- make clean all

