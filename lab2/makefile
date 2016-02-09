UNAME_S=$(shell uname -s)
CCFLAGS = -Wall -std=c99 -DGL_GLEXT_PROTOTYPES
LDFLAGS = -lm
EXEC=lab2-5
SOURCES = $(EXEC).c loadobj.c GL_utilities.c LoadTGA.c VectorUtils3.c
INCLUDES =
MGLUT =
OBJS = $(SOURCES:.c=.o)

ifeq ($(UNAME_S),Linux)
	LDFLAGS += -lGL -lX11 -lXext
	MGLUT = Linux/MicroGlut.c
	OBJS += $(MGLUT:.c=.o)
	INCLUDES += -ILinux
endif

ifeq ($(UNAME_S),Darwin)
	LDFLAGS += -framework Cocoa -framework CoreGraphics -framework OpenGL -framework CoreFoundation
	MGLUT = mac/MicroGlut.m
	OBJS += $(MGLUT:.m=.o)
	INCLUDES += -Imac
endif

all : $(EXEC)

$(EXEC): $(OBJS)
	gcc $(CCFLAGS) -o $@ $(OBJS) $(INCLUDES) $(LDFLAGS)

./%.o : ./%.c
	gcc $(CCFLAGS) -c -o $@ $< $(INCLUDES)

./%.o : ./%.m
	gcc $(CCFLAGS) -c -o $@ $< $(INCLUDES)

clean :
	rm -f $(EXEC) $(OBJS) *~
