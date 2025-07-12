# Detect operating system
OS := $(shell uname -s)

# Compiler and shared settings
CC := g++
SRC := src/*.cpp
OUT_DIR := out
EXECUTABLE := $(OUT_DIR)/main
INCLUDES := -Iinclude
LIB_DIR := -Llib
CFLAGS := -std=c++26 -Wall

# Platform-specific settings
ifeq ($(OS),Linux)
    PLATFORM := _LINUX
    LIBFLAGS := -lglfw -lwgpu_native
    RPATH := -Wl,-rpath,'$$ORIGIN'
    PRECOMMANDS :=
endif

ifeq ($(OS),Darwin)
    PLATFORM := _MACOS
    LIBFLAGS := -lglfw -lwgpu_native
    RPATH := -Wl,-rpath,'@loader_path'  # macOS uses @loader_path instead of $ORIGIN
    PRECOMMANDS :=
endif

ifeq ($(OS),Windows_NT)
    PLATFORM := _WINDOWS
    LIBFLAGS := -lglfw3 -lgdi32 -lwgpu_native
    RPATH :=
    PRECOMMANDS := cp ./lib/dll/*.dll $(OUT_DIR)
endif

# Final CFLAGS includes platform macro and paths
CFLAGS += -D$(PLATFORM) $(INCLUDES) $(LIB_DIR)

# -----------------------
# Build rule
# -----------------------
all:
	@mkdir -p $(OUT_DIR)
	$(PRECOMMANDS)
	$(CC) $(SRC) $(CFLAGS) -o $(EXECUTABLE) $(LIBFLAGS) $(RPATH)

# -----------------------
# Run the app
# -----------------------
run:
ifeq ($(OS),Windows_NT)
	$(EXECUTABLE).exe
else
	./$(EXECUTABLE)
endif

# -----------------------
# Clean build output
# -----------------------
clean:
	rm -rf $(OUT_DIR)
