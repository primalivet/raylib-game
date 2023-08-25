CC = gcc

CFLAGS = \
	 -Wall \
	 -Wextra \
	 -Werror \
	 -I/opt/homebrew/Cellar/raylib/4.5.0/include \
	 -I/opt/homebrew/Cellar/raylib/4.5.0/lib 
LDFLAGS = \
	 -L/opt/homebrew/Cellar/raylib/4.5.0/lib \
	 -lraylib 

# Name of executable
TARGET = game

# Source files
SRCS = main.c

# Derive object files from source files
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Link target executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Pattern rule to compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean object files and executable
clean:
	rm -f $(OBJS) $(TARGET)
