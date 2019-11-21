Compiling the program is done through the usual steps of: 

$ premake gmake4
$ make
$ ./A4 Assets/{lua-file}.lua

Manual:

Objectives I did not complete:
- The triangle meshes render, but the colouring/lighting are shadowed in some places, 
    making it look odd.  
- My novel scene is very simplistic due to my inability to create a nice triangle mesh    
    object. It contains a modified of the test script with certain things omitted so that 
    what I can actually render is visible. 'Assets/sample.png' showcases hierarchical 
    transformations but with the "plane" underneath removed to make the other objects
    visible.
- I did not implement bounding volumes due to strange interactions with triangle meshes
- I did not implement an extra feature, which would've been Anti Aliasing via Supersampling

Notes:
- Hierarchical transformations can perform correctly. Spheres and cubes are transformed 
via arbitrary affine transformations, which will be shown in my test images.
- I used the Blinn-Phong Light Model
- For my non-trivial background I did a blue-black gradient based on the pixel height, 
    similarly to the backgrounds rendered in the test images.


I ask if it's possible to still extend my Ray Tracer for the final project and just 
continue working on this past the A4 due date. I really find this subject interesting but 
just ran out of time because of a lot of things going on at once. 
I think with another day or 2 I can manage to finish this and move directly into work for 
the final project. I would really appreciate it, but if not I understand.
