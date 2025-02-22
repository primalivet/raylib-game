CC =
CFLAGS = -g -O0 -Wall -Wextra -Werror
LDFLAGS =
OPERATIN_SYSTEM := $(shell uname -s)

# Setup compiler it's flags depending on operating system
ifeq ($(OPERATIN_SYSTEM),Darwin)
	CC = clang
	CFLAGS += -I./vendor/darwin/include
	LDFLAGS += -L./vendor/darwin/lib -lraylib -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
else
	$(error Unsupported operating system)
endif

TARGET = ./bin/game
# SRC_FILES = vector2.c prio_queue.c a_star.c dynlist.c level.c physics.c entity.c camera.c main.c 
SRC_FILES = level.c vector2.c render.c camera.c window.c entity.c entity_physics.c entity_input.c main.c
OBJ_FILES = $(SRC_FILES:.c=.o)

# Default command (since it's first), depends on the game executable
all: $(TARGET)

# Build the game executable, depends on the object files
$(TARGET): $(OBJ_FILES)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ_FILES) -o $(TARGET) 

# Build object files from source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	
.PHONY: clean
clean:
	rm -rf $(OBJ_FILES) $(TARGET)
