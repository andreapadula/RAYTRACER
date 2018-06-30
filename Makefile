PROG = raytracer

CC = g++
CFLAGS = -D_DEBUG -g -Iinclude
LIBS = -L/usr/local/lib -lglfw3 -framework OpenGL -framework cocoa -framework IOKit -framework CoreVideo 
OBJS = $(PROG).o 

all: $(PROG)

gl3w.o: gl3w.c
	gcc -c $(CFLAGS) -o $@ $<

.cpp.o:
	$(CC) -c $(CFLAGS) -o $@ $<

$(PROG): $(OBJS)
	$(CC) -g -o $@ $^ $(LIBS)

clean:
	rm -f *.o $(PROG)
