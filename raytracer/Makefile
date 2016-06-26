CC = g++
CFLAGS = -g -O2 -std=c++11
CLIBS = -lm -lGL -lglfw -lGLEW
CINC = -I /usr/include/GLFW
EXEC = render

all:
	$(CC) $(CINC) $(CFLAGS) -o $(EXEC) main.cpp $(CLIBS)

clean:
	rm *.o
