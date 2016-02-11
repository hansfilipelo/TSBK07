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
  #include "mac/MicroGlut.h"
#elif defined __linux__
  #include "Linux/MicroGlut.h"
#endif

#include "GL_utilities.h"
#include <math.h>
#include "loadobj.h"
#include "LoadTGA.h"
#include "VectorUtils3.h"

#define PI 3.14159265

#define near 1.0
#define far 30.0
#define right 0.5
#define left -0.5
#define top 0.5
#define bottom -0.5

// ---------------

void OnTimer(int value)
{
  glutPostRedisplay();
  glutTimerFunc(20, &OnTimer, value);
}

// Globals
// Data would normally be read from files

mat4 transformMatrix;

GLfloat projectionMatrix[] =
{
  2.0f*near/(right-left), 0.0f, (right+left)/(right-left), 0.0f,
  0.0f, 2.0f*near/(top-bottom), (top+bottom)/(top-bottom), 0.0f,
  0.0f, 0.0f, -(far + near)/(far - near), -2*far*near/(far - near),
  0.0f, 0.0f, -1.0f, 0.0f
};

int sizeOfGLfloat = sizeof(GLfloat)/2;
GLfloat phi = 0;
GLfloat cosPhi;
GLfloat sinPhi;
GLfloat minSin;

// Reference to shader program
GLuint program;
GLuint tex;

Model *bunny;
Model *bunny_model2;

// vertex array object
unsigned int vertexArrayObjID;
unsigned int bunnyVertexArrayObjID;
unsigned int bunnyTexCoordBufferObjID;
unsigned int vertexBufferObjID;
unsigned int colorBufferObjID;
unsigned int bunnyVertexBufferObjID;
unsigned int bunnyIndexBufferObjID;
unsigned int bunnyNormalBufferObjID;

//
unsigned int bunny_model2VertexArrayObjID;
unsigned int bunny_model2TexCoordBufferObjID;
unsigned int bunny_model2VertexBufferObjID;
unsigned int bunny_model2IndexBufferObjID;
unsigned int bunny_model2NormalBufferObjID;


void init(void)
{
  // vertex buffer object, used for uploading the geometry

  dumpInfo(); // Dump driver info to stdout

  // Load Model
  bunny = LoadModelPlus("bunnyplus.obj");
  bunny_model2 = LoadModelPlus("bunnyplus.obj");

  // GL inits
  glClearColor(1,1,1,0);
  //glClearColor(0.2,0.2,0.5,0); // Color in buffer upon clear buffer
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glEnable(GL_DEPTH_TEST);
  printError("GL inits");

  // Load and compile shader
  program = loadShaders("lab2-7.vert", "lab2-7.frag"); // These are the programs that run on GPU
  printError("init shader");



  // --------------------------------------

  glUniformMatrix4fv(glGetUniformLocation(program, "projectionMatrix"), 1, GL_TRUE, projectionMatrix);

  printError("init arrays");
}


void display(void)
{
  printError("pre display");

  // clear the screen (using chosen color earlier)
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(1,1,1,0);

  // Set rotation matrix
  phi = ( phi < 2*PI ) ? phi+PI/50 : phi-2*PI+PI/50;

  vec3 trans = {1,0,-3};
  vec3 trans2 = {-1,0,-3};
  vec3 lookAtPoint = {0,0,-3};
  vec3 cameraLocation = {3.0f*cos(phi),0.0f,-3+3.0f*sin(phi)};
  vec3 upVector = {0,1,0};

  mat4 translation = T(trans.x,trans.y,trans.z);
  mat4 rotations = IdentityMatrix(); //Mult(Ry(phi),Mult(Rx(phi), Rz(phi)));

  mat4 lookAtMatrix = lookAtv(cameraLocation,lookAtPoint,upVector);
  transformMatrix = Mult(translation,rotations);

  // Send translMatrix to Vertex
  glUniformMatrix4fv(glGetUniformLocation(program, "transformMatrix"), 1, GL_TRUE,  transformMatrix.m);
  // Send lookAt-vector to vertex shader
  glUniformMatrix4fv(glGetUniformLocation(program, "lookAtMatrix"), 1, GL_TRUE,  lookAtMatrix.m);

  DrawModel(bunny, program, "in_Position", "in_Normal", "inTexCoord");

  // Model 2
  transformMatrix = T(trans2.x, trans2.y, trans2.z);
  glUniformMatrix4fv(glGetUniformLocation(program, "transformMatrix"), 1, GL_TRUE,  transformMatrix.m);
  DrawModel(bunny_model2, program, "in_Position", "in_Normal", "inTexCoord");

  printError("display");

  glutSwapBuffers(); // Swap buffer so that GPU can use the buffer we uploaded to it and we can write to another
}

int main(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitContextVersion(3, 2);
  glutCreateWindow("Bunny");
  glutDisplayFunc(display);
  init ();
  OnTimer(0);
  glutMainLoop();
}
