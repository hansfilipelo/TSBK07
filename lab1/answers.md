# Lab1

## 1-1
 
**Where is the origin placed in the on-screen coordinate system?**
In the middle of the screen. 

**Which direction are the X and Y axes pointing in the on-screen coordinate system?**
X to the right, Y to the top of the screen.

**The triangle color is controlled from the fragment shader. Would it be possible to control it from the main program? How?**
One can upload a variable from the CPU to the GPU and then apply the color from the variable in the fragment shader. 

## 1-2

**What is the purpose of the "in", "out" and "uniform" modifiers?**
in means in variable.
out means out variable.
uniform means a constant variable used for all vertices.

**What is the output of the vertex shader?**
The vertex shaders outputs a geometry. Its "out" variables are also used as input for the fragment shader. 

**What does the function glUniformMatrix4fv do?**
Sends a 4 dimensional matrix to a uniform variable in the GPU/shaders. 

## 1-3

**What is the frame rate of the animation?**
50 fps

## 1-4
**Did you need to do anything different when uploading the color data?**
We did use a buffer so that each vertex could have its own color.

**The "in" and "out" modifiers are now used for something different. What?**
In and out is now used to pass variables between vertex and fragment shader. 

**What is this kind of shading called? What could we use otherwise?**
Gouraud shading. Flat shading or Phong shading are other techniques. 