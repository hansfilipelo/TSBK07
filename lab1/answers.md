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

## 1-5
**What problems did you encounter while building the cube?**
I forgot to update the command glDrawArrays which took a while to debug.

**How do you change the facing of a polygon?**
Using different colors? Z-buffer? I don't get it.

## 1-6
**Why do we need normal vectors for a model?**
For calculatin the correct reflection for light?

**What did you do in your fragment shader?**
Just set the colors to the normal vector.

**Should a normal vector always be perpendicular to a certain triangle? If not, why?**
It should.

**Now we are using glBindBuffer and glBufferData again. They deal with buffers, but in what way?**
Enables a buffer on the GPU, gives us a reference to that buffer and then we can uppload data to it.
