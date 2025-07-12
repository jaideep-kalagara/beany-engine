all:
	mkdir -p out
	cp lib/dll/*.dll out
	g++ src/*.cpp -std=c++26 -Iinclude -Llib -o out/main -lglfw3 -lgdi32 -lwgpu_native

run:
	./out/main