
all : lab1-1

lab1-1: lab1-1.c GL_utilities.c 
	gcc -Wall -std=c99 -I/usr/X11R6/include -L/usr/X11R6/lib -lX11 -o lab1-1 -lGL -lm -DGL_GLEXT_PROTOTYPES lab1-1.c GL_utilities.c MicroGlut.c
	#gcc -Wall -std=c99 -o lab1-1 -lGL -lm -DGL_GLEXT_PROTOTYPES -L/usr/X11R6/lib -lXext -lX11 lab1-1.c GL_utilities.c MicroGlut.c

clean :
	rm lab1-1

