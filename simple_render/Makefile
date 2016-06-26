CC = g++
CFLAGS = -g -O0 -std=c++11
CLIBS = -lm -lGL -lglfw -lGLEW
CINC = -I /usr/include/GLFW
EXEC = render

all: object.o pointlight.o
	$(CC) $(CINC) $(CFLAGS) pointlight.o object.o -o $(EXEC) main.cpp $(CLIBS)
	make clean

object.o: src/object.cpp
	$(CC) $(CINC) $(CFLAGS) -c src/object.cpp $(CLIBS)

pointlight.o: src/pointlight.cpp
	$(CC) $(CINC) $(CFLAGS) -c src/pointlight.cpp $(CLIBS)

clean:
	rm *.o
