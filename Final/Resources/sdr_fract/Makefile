obj=image_ppm.o sdr.o timer.o

CC=gcc
CCFLAGS=-std=c89 -pedantic -Wall -I/usr/include/GL -I/usr/include/SDL2 -g
#LDFLAGS = -F/Library/Frameworks -framework OpenGL -lGL -lGLU -lglut -lm
#LDFLAGS = -F/Library/Frameworks -framework OpenGL -framework GLUT
LDFLAGS=-lSDL2 -lSDL2main -lGL -ldl -lglut -lGLU

.PHONY: all
all: mbrot julia

mbrot.o: mbrot.c
	$(CC) $(CCFLAGS) $(LDFLAGS) mbrot.c -o $@

julia.o: julia.c
	$(CC) $(CCFLAGS) $(LDFLAGS) julia.c -o $@

mbrot: mbrot.o $(obj)
	$(CC) $(CCFLAGS) $(LDFLAGS) mbrot.o $(obj) -o $@

julia: julia.o $(obj)
	$(CC) $(CCFLAGS) $(LDFLAGS) julia.o $(obj) -o $@

.PHONY: clean
clean:
	rm -f $(obj) mbrot.o julia.o mbrot julia
