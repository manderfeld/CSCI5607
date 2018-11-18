CC=g++
MAIN=maze.cpp
EXECUTABLE=maze

all: $(MAIN)
	$(CC) $(MAIN) -o $(EXECUTABLE) -x c glad/glad.c -g -F/Library/Frameworks -framework SDL2 -framework OpenGL

clean:
	rm -rf $(OBJFILES)