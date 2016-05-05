CC = g++
CFLAGS = -g -O0 -std=c++11
CLIBS = -lm -lGL -lglfw -lGLEW
CINC = -I /usr/include/GLFW
EXEC = render

all: object.o
	$(CC) $(CINC) $(CFLAGS) $(CLIBS) object.o -o $(EXEC) main.cpp

object.o: src/object.cpp
	$(CC) $(CINC) $(CFLAGS) $(CLIBS) -c src/object.cpp 

clean:
	rm *.o