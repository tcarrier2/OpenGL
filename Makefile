CFLAGS=-I/opt/local/include/
LDFLAGS=-L/opt/local/lib/ -I/opt/local/include/
LDLIBS=-lGLEW -framework GLUT -framework OpenGL -framework SDL2 -framework Cocoa -framework SDL2_image
all: blending depth_buffer lighting_diffuse lighting_specular rectangles stencil_buffer triangle_rebound
clean:
	rm -f *.o blending depth_buffer lighting_diffuse lighting_specular rectangles stencil_buffer triangle_rebound
