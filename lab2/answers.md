## 2-1

**How are the textures coordinates mapped on the bunny? Can you see what geometry was used?**

Linear mapping.

**What kind of procedural texture did you make?**
Made it scale with cos(t)

## 2-2

**Can we modify how we access the texture? How?**



**Why can't we just pass the texture object to the shader? There is a specific reason for this, a limited resource. What?**

Memory bandwidth. If we create a reference to the texture and then use this reference to refer to the uploaded shader then we can save bandwidth since we are not uploading the texture to the GPU over and over again.

## 2-3

**How did you move the bunny to get it in view?**

Translated it -3 in Z-direction.

## 2-4

**Given a certain vector for v, is there some place you can't place the camera?**

Nicht?

## 2-5


**Did you implement your light calculations in the vertex or fragment shader? So, which kind of shading did you implement?**

Vertex shader. Gouraud?

**Some geometry data must be vec4, others are just as well vec3's. Which ones, and why? How about vertices, light source, normal vectors...?**
