## 3-1

**How can you get all four blades to rotate with just one time-dependent rotation matrix?**

Give each win an ID (0-3) and then apply ID*PI/2 with each matrix. 

**How do you make the wings follow the body's movements?**

Use the same translations for both objects once we've gotten them in the wings in the right spot for the mill. 

**You may have to tweak some numbers to make the placement right. How can you do that without making a lot of small changes in numbers in the code followed by recompilations?**

Set the numbers in the vertex shader (which I don't do). 

## 3-2

