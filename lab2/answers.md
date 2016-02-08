## 2-1

**How are the textures coordinates mapped on the bunny? Can you see what geometry was used?**

Linear mapping. 

**What kind of procedural texture did you make?**
Made it scale with cos(t)

## 2-2

**Can we modify how we access the texture? How?**



**Why can't we just pass the texture object to the shader? There is a specific reason for this, a limited resource. What?**

Memory bandwidth. If we create a reference to the texture and then use this reference to refer to the uploaded shader then we can save bandwidth since we are not uploading the texture to the GPU over and over again. 