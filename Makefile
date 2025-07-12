# operating system to compile for
OS := $(shell uname -s)

ifeq ($(OS),Linux) 
    # Linux-specific commands or settings
    CC = g++
    CFLAGS = -std=c++26 -Wall -D_LINUX -Iinclude -Llib
	LIBFLAGS = -lglfw -lwgpu_native
    EXECUTABLE = out/main
endif

# ifeq ($(OS),Darwin) # macOS
#     # macOS-specific commands or settings
#     CC = g++
#     CFLAGS = -std=c++26 -Wall -D_MACOS -Iinclude -Llib
# 	LIBFLAGS = -lglfw3 -lwgpu_native
#     EXECUTABLE = out/main
# endif

ifeq ($(OS),Windows_NT) # Windows (via Cygwin/MinGW, etc.)
    # Windows-specific commands or settings
    CC = g++
    CFLAGS = -std=c++26 -Wall -D_WINDOWS -Iinclude -Llib
	LIBFLAGS = -lglfw3 -lgdi32 -lwgpu_native
	PRECOMMANDS = cp ./lib/dll/*.dll out
    EXECUTABLE = out/main

endif

all:
	mkdir -p out
	${PRECOMMANDS}
	${CC} src/*.cpp ${CFLAGS} -o ${EXECUTABLE} ${LIBFLAGS}

run:
	./out/main

clean:
	rm -rf out