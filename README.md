# Trying out Raylib

## Create build script for Raylib installed via Homebrew (macos)
```
echo "cc main.c $(pkg-config --libs --cflags raylib) -o game" >> build.sh
chmod +x build.sh
```
