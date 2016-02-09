UNAME_S=$(shell uname -s)
CCFLAGS = -Wall -std=c99 -DGL_GLEXT_PROTOTYPES
LDFLAGS = -lm
EXEC=lab2-4
SOURCES=$(EXEC).c loadobj.c GL_utilities.c LoadTGA.c VectorUtils3.c
MGLUT =

ifeq ($(UNAME_S),Linux)
	LDFLAGS += -lGL -lX11 -lXext
	MGLUT = Linux/MicroGlut.c
endif

ifeq ($(UNAME_S),Darwin)
	LDFLAGS += -framework Cocoa -framework CoreGraphics -framework OpenGL -framework CoreFoundation
	MGLUT = mac/MicroGlut.m
endif

all : $(EXEC)

$(EXEC): $(SOURCES)
	gcc $(CCFLAGS) -o $(EXEC) $(SOURCES) $(MGLUT) $(LDFLAGS)

clean :
	rm -f $(EXEC) *~
