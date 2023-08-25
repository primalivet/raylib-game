CC = gcc

CFLAGS = \
	 -Wall \
	 -Wextra \
	 -Werror \
	 -I./include \
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

all: $(TARGET)

# Link target executable
$(TARGET): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $(TARGET) $(LDFLAGS)

# Pattern rule to compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_FILES) $(TARGET)
