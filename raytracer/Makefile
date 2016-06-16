CC = g++
CFLAGS = -g -O2 -std=c++11
CLIBS = -lm -lGL -lglfw -lGLEW
CINC = -I /usr/include/GLFW
EXEC = render

all:
	$(CC) $(CINC) $(CFLAGS) $(CLIBS) -o $(EXEC) main.cpp

clean:
	rm *.o
