CC = gcc

CFLAGS = \
	 -Wall \
	 -Wextra \
	 -Werror \
	 -I/opt/homebrew/Cellar/raylib/4.5.0/include 

LDFLAGS = \
	 -L/opt/homebrew/Cellar/raylib/4.5.0/lib \
	 -lraylib 

# Source files and derive object files from source files
SRC_FILES = level.c \
	    player.c \
	    camera.c \
	    main.c 

OBJ_FILES = $(SRC_FILES:.c=.o)

TARGET = ./bin/game

# Test source files and derive object files from source files
TEST_SRC_FILES = main_test.c

TEST_OBJ_FILES = $(TEST_SRC_FILES:.c=.o)

TEST_TARGET = ./bin/test

all: $(TARGET)

# Link target executable
$(TARGET): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $(TARGET) $(LDFLAGS)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(filter-out main.o, $(OBJ_FILES)) $(TEST_OBJ_FILES)
	$(CC) $(filter-out main.o, $(OBJ_FILES)) $(TEST_OBJ_FILES) -o $(TEST_TARGET) $(LDFLAGS)

# Pattern rule to compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_FILES) $(TARGET) $(TEST_OBJ_FILES) $(TEST_TARGET)
