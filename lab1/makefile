UNAME_S=$(shell uname -s)
CCFLAGS = -Wall -std=c99 -DGL_GLEXT_PROTOTYPES
LDFLAGS = -lm
EXEC=lab1
SOURCES=lab1-3.c GL_utilities.c
MGLUT = 

ifeq ($(UNAME_S),Linux)
	LDFLAGS += -lGL -lX11 -lXext
	MGLUT = MicroGlut_Linux/MicroGlut.c
endif

ifeq ($(UNAME_S),Darwin)
	LDFLAGS += -framework Cocoa -framework CoreGraphics -framework OpenGL -framework CoreFoundation
	MGLUT = MicroGlut_Mac/MicroGlut.m
endif

all : clean lab1

lab1: $(SOURCES)
	gcc $(CCFLAGS) -o $(EXEC) $(SOURCES) $(MGLUT) $(LDFLAGS)

clean :
	rm -f $(EXEC)

