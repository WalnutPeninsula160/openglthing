all:
	g++ -o bin/main.bin -c src/main.cpp -I../include -I./include -I/usr/include -L/usr/lib -lglfw -lGL -march=native
	g++ -o bin/shaders.bin -c src/shaders.cpp -I../include -I./include -I/usr/include -L/usr/lib -lglfw -march=native
	g++ -o bin/objects.bin -c src/objects.cpp -I../include -I./include -I/usr/include -L/usr/lib -lglfw -march=native
	g++ -o bin/glad.bin -c ../src/glad.c
	g++ -o main bin/main.bin bin/shaders.bin bin/objects.bin bin/glad.bin
