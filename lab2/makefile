UNAME_S=$(shell uname -s)
CCFLAGS = -Wall -std=c99 -DGL_GLEXT_PROTOTYPES
LDFLAGS = -lm
EXEC=lab2
SOURCES=lab2-1.c loadobj.c GL_utilities.c LoadTGA.c
MGLUT =

ifeq ($(UNAME_S),Linux)
	LDFLAGS += -lGL -lX11 -lXext
	MGLUT = Linux/MicroGlut.c
endif

ifeq ($(UNAME_S),Darwin)
	LDFLAGS += -framework Cocoa -framework CoreGraphics -framework OpenGL -framework CoreFoundation
	MGLUT = mac/MicroGlut.m
endif

all : clean $(EXEC)

$(EXEC): $(SOURCES)
	gcc $(CCFLAGS) -o $(EXEC) $(SOURCES) $(MGLUT) $(LDFLAGS)

clean :
	rm -f $(EXEC) *~
