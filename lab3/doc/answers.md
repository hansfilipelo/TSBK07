## 3-1

**How can you get all four blades to rotate with just one time-dependent rotation matrix?**

Give each win an ID (0-3) and then apply ID*PI/2 with each matrix.

**How do you make the wings follow the body's movements?**

Use the same translations for both objects once we've gotten them in the wings in the right spot for the mill.

**You may have to tweak some numbers to make the placement right. How can you do that without making a lot of small changes in numbers in the code followed by recompilations?**

Set the numbers in the vertex shader (which I don't do).

## 3-2

**What kind of control did you implement?**

FPS-like controls

**Can you make this kind of control in some other way than manipulating a "look-at" matrix?**

You could use transform on the entire world (which is essentially what we do).

## 3-3

**How did you handle the camera matrix for the skybox?**
The skybox constantly moves above the camera. Handling skybox camera matrix in the same way as always?

**How did you represent the objects? Is this a good way to manage a scene or would you do it differently for a "real" application?**
Used global variables. Probably not reasonable to use all in the same file in real world applications. Use one shader pair for each type of model (textured or shaded).

**What special considerations are needed when rendering a skybox?**
No Z-buffer.
