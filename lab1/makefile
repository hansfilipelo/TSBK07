UNAME_S=$(shell uname -s)
CCFLAGS = -Wall -std=c99 -DGL_GLEXT_PROTOTYPES
LDFLAGS = -lGL -lX11 -lm
EXEC=lab1-1
SOURCES=lab1-1.c GL_utilities.c MicroGlut.c

ifeq ($(UNAME_S),Linux)
	LDFLAGS += -lXext
endif

ifeq ($(UNAME_S),Darwin)
	LDFLAGS += -I/usr/X11R6/include -L/usr/X11R6/lib
endif

all : lab1-1

lab1-1: lab1-1.c GL_utilities.c 
	gcc $(CCFLAGS) -o $(EXEC) $(SOURCES) $(LDFLAGS)
	#gcc -Wall -std=c99 -I/usr/X11R6/include -L/usr/X11R6/lib -lX11 -o lab1-1 -lGL -lm -DGL_GLEXT_PROTOTYPES lab1-1.c GL_utilities.c MicroGlut.c
	#gcc -Wall -std=c99 -o lab1-1 -DGL_GLEXT_PROTOTYPES lab1-1.c GL_utilities.c MicroGlut.c -lXext -lX11 -lGL -lm

clean :
	rm -f lab1-1

