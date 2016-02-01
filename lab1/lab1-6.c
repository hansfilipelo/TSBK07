// Questions:
// 1) Why don't we need a buffer to upload translMatrix? Do we need buffers for everything that is not uniform (for example different colors)?


// Lab 1-2.
// This is the same as the first simple example in the course book,
// but with a few error checks.
// Remember to copy your file to a new on appropriate places during the lab so you keep old results.
// Note that the files "lab1-1.frag", "lab1-1.vert" are required.

// Should work as is on Linux and Mac. MS Windows needs GLEW or glee.
// See separate Visual Studio version of my demos.
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include "MicroGlut_Mac/MicroGlut.h"
// Linking hint for Lightweight IDE
// uses framework Cocoa
#endif
#include "GL_utilities.h"
#include <math.h>
#include "loadobj.h"

#define PI 3.14159265

void OnTimer(int value)
{
    glutPostRedisplay();
    glutTimerFunc(20, &OnTimer, value);
}

// Globals
// Data would normally be read from files
GLfloat vertices[] = {
	-0.3f,-0.3f,-0.3f,
	-0.3f,-0.3f, 0.3f,
	-0.3f, 0.3f, 0.3f,
	0.3f, 0.3f,-0.3f,
	-0.3f,-0.3f,-0.3f,
	-0.3f, 0.3f,-0.3f,
	0.3f,-0.3f, 0.3f,
	-0.3f,-0.3f,-0.3f,
	0.3f,-0.3f,-0.3f,
	0.3f, 0.3f,-0.3f,
	0.3f,-0.3f,-0.3f,
	-0.3f,-0.3f,-0.3f,
	-0.3f,-0.3f,-0.3f,
	-0.3f, 0.3f, 0.3f,
	-0.3f, 0.3f,-0.3f,
	0.3f,-0.3f, 0.3f,
	-0.3f,-0.3f, 0.3f,
	-0.3f,-0.3f,-0.3f,
	-0.3f, 0.3f, 0.3f,
	-0.3f,-0.3f, 0.3f,
	0.3f,-0.3f, 0.3f,
	0.3f, 0.3f, 0.3f,
	0.3f,-0.3f,-0.3f,
	0.3f, 0.3f,-0.3f,
	0.3f,-0.3f,-0.3f,
	0.3f, 0.3f, 0.3f,
	0.3f,-0.3f, 0.3f,
	0.3f, 0.3f, 0.3f,
	0.3f, 0.3f,-0.3f,
	-0.3f, 0.3f,-0.3f,
	0.3f, 0.3f, 0.3f,
	-0.3f, 0.3f,-0.3f,
	-0.3f, 0.3f, 0.3f,
	0.3f, 0.3f, 0.3f,
	-0.3f, 0.3f, 0.3f,
	0.3f,-0.3f, 0.3f
};

GLfloat color[] =
{
	0.383f,  0.771f,  0.014f,
	0.609f,  0.115f,  0.436f,
	0.327f,  0.483f,  0.844f,
	0.822f,  0.369f,  0.201f,
	0.435f,  0.602f,  0.223f,
	0.310f,  0.747f,  0.185f,
	0.397f,  0.770f,  0.761f,
	0.359f,  0.436f,  0.730f,
	0.359f,  0.383f,  0.152f,
	0.483f,  0.396f,  0.789f,
	0.359f,  0.861f,  0.639f,
	0.195f,  0.348f,  0.859f,
	0.014f,  0.184f,  0.376f,
	0.771f,  0.328f,  0.970f,
	0.406f,  0.615f,  0.116f,
	0.676f,  0.977f,  0.133f,
	0.971f,  0.372f,  0.833f,
	0.140f,  0.616f,  0.489f,
	0.997f,  0.313f,  0.064f,
	0.945f,  0.719f,  0.392f,
	0.343f,  0.021f,  0.978f,
	0.279f,  0.317f,  0.305f,
	0.167f,  0.620f,  0.077f,
	0.347f,  0.857f,  0.137f,
	0.055f,  0.953f,  0.042f,
	0.714f,  0.305f,  0.345f,
	0.783f,  0.290f,  0.734f,
	0.722f,  0.645f,  0.174f,
	0.302f,  0.455f,  0.848f,
	0.225f,  0.387f,  0.040f,
	0.317f,  0.713f,  0.338f,
	0.053f,  0.959f,  0.120f,
	0.393f,  0.621f,  0.362f,
	0.673f,  0.211f,  0.457f,
	0.820f,  0.883f,  0.371f,
	0.982f,  0.099f,  0.879f
};

GLfloat translMatrix[] =
{
    1.0f, 0.0f, 0.0f, 0.5f,
    0.0f, 1.0f, 0.0f, 0.5f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
};

GLfloat rotMatrix[] =
{
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
};

GLfloat rotMatrixX[] =
{
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
};

GLfloat phi = 0;
GLfloat cosPhi;
GLfloat sinPhi;
GLfloat minSin;

// Reference to shader program
GLuint program;

Model *m;

// vertex array object
unsigned int vertexArrayObjID;
unsigned int bunnyVertexArrayObjID;
unsigned int vertexBufferObjID;
unsigned int colorBufferObjID;
unsigned int bunnyVertexBufferObjID;
unsigned int bunnyIndexBufferObjID;
unsigned int bunnyNormalBufferObjID;

void init(void)
{
	// vertex buffer object, used for uploading the geometry

	dumpInfo(); // Dump driver info to stdout

  // Load Model
  m = LoadModelPlus("bunny.obj");

  // GL inits
  glClearColor(1,1,1,0);
  //glClearColor(0.2,0.2,0.5,0); // Color in buffer upon clear buffer
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glEnable(GL_DEPTH_TEST);
	printError("GL inits");

	// Load and compile shader
	program = loadShaders("lab1-6.vert", "lab1-6.frag"); // These are the programs that run on GPU
	printError("init shader");

	// Upload geometry to the GPU:
  glGenVertexArrays(1, &bunnyVertexArrayObjID);
	glGenBuffers(1, &bunnyVertexBufferObjID);
	glGenBuffers(1, &bunnyIndexBufferObjID);
	glGenBuffers(1, &bunnyNormalBufferObjID);
	printError("glGenBuffers");

  glBindVertexArray(bunnyVertexArrayObjID);

  // VBO for vertex data
  glBindBuffer(GL_ARRAY_BUFFER, bunnyVertexBufferObjID);
  glBufferData(GL_ARRAY_BUFFER, m->numVertices*3*sizeof(GLfloat), m->vertexArray, GL_STATIC_DRAW);
  glVertexAttribPointer(glGetAttribLocation(program, "in_Position"), 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(glGetAttribLocation(program, "in_Position"));

  // VBO for normal data
  glBindBuffer(GL_ARRAY_BUFFER, bunnyNormalBufferObjID);
  glBufferData(GL_ARRAY_BUFFER, m->numVertices*3*sizeof(GLfloat), m->normalArray, GL_STATIC_DRAW);
  glVertexAttribPointer(glGetAttribLocation(program, "in_Normal"), 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(glGetAttribLocation(program, "in_Normal"));

	// --------------------------------------

	glUniformMatrix4fv(glGetUniformLocation(program, "translMatrix"), 1, GL_TRUE, translMatrix);

	printError("init arrays");
}


void display(void)
{
	printError("pre display");

	// clear the screen (using chosen color earlier)
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set rotation matrix
	phi += PI/50;
	cosPhi = cos(phi);
	sinPhi = sin(phi);
	minSin = -sinPhi;

	rotMatrix[0] = cosPhi;
	rotMatrix[5] = cosPhi;
	rotMatrix[1] = minSin;
	rotMatrix[4] = sinPhi;

  rotMatrixX[5] = cosPhi;
  rotMatrixX[6] = minSin;
  rotMatrixX[9] = sinPhi;
  rotMatrixX[10] = cosPhi;

	// Send translMatrix to Vertex
	glUniformMatrix4fv(glGetUniformLocation(program, "rotMatrix"), 1, GL_TRUE, rotMatrix);
  glUniformMatrix4fv(glGetUniformLocation(program, "rotMatrixX"), 1, GL_TRUE, rotMatrixX);

  glBindVertexArray(bunnyVertexArrayObjID);	// Select VAO
  glDrawElements(GL_TRIANGLES, m->numIndices, GL_UNSIGNED_INT, 0L);

	printError("display");

	glutSwapBuffers(); // Swap buffer so that GPU can use the buffer we uploaded to it and we can write to another
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 2);
	glutCreateWindow ("Bunny");
	glutDisplayFunc(display);
	init ();
	OnTimer(0);
	glutMainLoop();
}
