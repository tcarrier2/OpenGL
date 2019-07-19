CFLAGS=-I/opt/local/include/
LDFLAGS=-L/opt/local/lib/ -I/opt/local/include/
LDLIBS=-lGLEW -framework GLUT -framework OpenGL -framework SDL2 -framework Cocoa -framework SDL2_image
all: triangle25
clean:
	rm -f *.o triangle25
