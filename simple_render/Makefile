CC = g++
CFLAGS = -g -O0 -std=c++11
CLIBS = -lm -lGL -lglfw -lGLEW
CINC = -I /usr/include/GLFW
EXEC = render

all: object.o pointlight.o
	$(CC) $(CINC) $(CFLAGS) $(CLIBS) pointlight.o object.o -o $(EXEC) main.cpp
	make clean

object.o: src/object.cpp
	$(CC) $(CINC) $(CFLAGS) $(CLIBS) -c src/object.cpp 

pointlight.o: src/pointlight.cpp
	$(CC) $(CINC) $(CFLAGS) $(CLIBS) -c src/pointlight.cpp 

clean:
	rm *.o