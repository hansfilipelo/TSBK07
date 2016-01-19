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

all : clean lab1-1

lab1-1: lab1-1.c GL_utilities.c 
	gcc $(CCFLAGS) -o $(EXEC) $(SOURCES) $(LDFLAGS)

clean :
	rm -f lab1-1

