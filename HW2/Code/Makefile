CC=g++
MAIN=raytracer.cpp
DEP=image.cpp pixel.cpp vector.cpp objects.cpp
EXECUTABLE=raytracer

all: $(MAIN)
	$(CC) $(MAIN) -o $(EXECUTABLE) $(DEP)

clean:
	rm -rf $(OBJFILES) $(MAIN)
