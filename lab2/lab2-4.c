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
// Linking hint for Lightweight IDE
// uses framework Cocoa
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

Model *m;

// vertex array object
unsigned int vertexArrayObjID;
unsigned int bunnyVertexArrayObjID;
unsigned int bunnyTexCoordBufferObjID;
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
  m = LoadModelPlus("bunnyplus.obj");

  // GL inits
  glClearColor(1,1,1,0);
  //glClearColor(0.2,0.2,0.5,0); // Color in buffer upon clear buffer
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glEnable(GL_DEPTH_TEST);
  printError("GL inits");

  // Load and compile shader
  program = loadShaders("lab2-4.vert", "lab2-4.frag"); // These are the programs that run on GPU
  printError("init shader");

  // Upload geometry to the GPU:
  glGenVertexArrays(1, &bunnyVertexArrayObjID);
  glGenBuffers(1, &bunnyVertexBufferObjID);
  glGenBuffers(1, &bunnyIndexBufferObjID);
  glGenBuffers(1, &bunnyNormalBufferObjID);
  printError("glGenBuffers");

  glBindVertexArray(bunnyVertexArrayObjID);

  // Lab2
  //--------------
  glGenBuffers(1, &bunnyTexCoordBufferObjID);

  if (m->texCoordArray != NULL)
  {
    // Load model and its texture mapping coordinates
    glBindBuffer(GL_ARRAY_BUFFER, bunnyTexCoordBufferObjID);
    glBufferData(GL_ARRAY_BUFFER, m->numVertices*2*sizeof(GLfloat), m->texCoordArray, GL_STATIC_DRAW);
    glVertexAttribPointer(glGetAttribLocation(program, "inTexCoord"), 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(glGetAttribLocation(program, "inTexCoord"));
  }

  // Load texture into uniform in fragment shader
  glUniform1i(glGetUniformLocation(program, "tex"), 0); // Create texture unit
  LoadTGATextureSimple("grass.tga", &tex); // Load texture to texture unit
  glBindTexture(GL_TEXTURE_2D, tex); // Bind texture unit
  glActiveTexture(GL_TEXTURE0); // Activate texture unit 0 (in case we have multiple textures, this is nessecary)

  // Create a texture unit

  // -----------------

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

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bunnyIndexBufferObjID);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m->numIndices*sizeof(GLuint), m->indexArray, GL_STATIC_DRAW);

  // --------------------------------------

  glUniformMatrix4fv(glGetUniformLocation(program, "projectionMatrix"), 1, GL_TRUE, projectionMatrix);

  printError("init arrays");
}


void display(void)
{
  printError("pre display");

  // clear the screen (using chosen color earlier)
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Set rotation matrix
  phi = ( phi < 2*PI ) ? phi+PI/50 : phi-2*PI+PI/50;

  vec3 trans = {0,0,-3};
  vec3 cameraLocation = {3.0f*cos(phi),0.0f,-3+3.0f*sin(phi)};
  vec3 upVector = {0,1,0};

  mat4 translation = T(trans.x,trans.y,trans.z);
  mat4 rotations = IdentityMatrix(); // Mult(Ry(phi),Mult(Rx(phi), Rz(phi)));

  mat4 lookAtMatrix = lookAtv(cameraLocation,trans,upVector);
  transformMatrix = Mult(translation,rotations);

  // Send translMatrix to Vertex
  glUniformMatrix4fv(glGetUniformLocation(program, "transformMatrix"), 1, GL_TRUE,  transformMatrix.m); // Variate shader

  // Send lookAt-vector to vertex shader
  glUniformMatrix4fv(glGetUniformLocation(program, "lookAtMatrix"), 1, GL_TRUE,  lookAtMatrix.m);

  glBindVertexArray(bunnyVertexArrayObjID);	// Select VAO
  glDrawElements(GL_TRIANGLES, m->numIndices, GL_UNSIGNED_INT, 0L);

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
