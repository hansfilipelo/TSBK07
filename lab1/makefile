UNAME_S=$(shell uname -s)
CCFLAGS = -Wall -std=c99 -DGL_GLEXT_PROTOTYPES
LDFLAGS = -lm
EXEC=lab1-1
SOURCES=lab1-1.c GL_utilities.c
MGLUT = 

ifeq ($(UNAME_S),Linux)
	LDFLAGS += -lGL -lXext -lX11
	MGLUT = MicroGlut_Linux/MicroGlut.c
endif

ifeq ($(UNAME_S),Darwin)
	LDFLAGS += -framework Cocoa -framework CoreGraphics -framework OpenGL -framework CoreFoundation
	MGLUT = MicroGlut_Mac/MicroGlut.m
endif

all : clean lab1-1

lab1-1: lab1-1.c GL_utilities.c 
	gcc $(CCFLAGS) -o $(EXEC) $(SOURCES) $(MGLUT) $(LDFLAGS)

clean :
	rm -f lab1-1

