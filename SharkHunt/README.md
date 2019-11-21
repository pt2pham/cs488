Compiling the program is done through the usual steps of: 

$ premake gmake4
$ make
$ ./A3 Assets/puppet.lua

Manual:

Objectives I did not complete
- Some puppet joints do not rotate about the hinged ends, they rotate about their centres
- I implemented rotation by rotating the Torso of my puppet to get it to rotate around its origin rather than using the virtual trackball method. Resetting position works but not orientation.
- I didn't implement undo/redo, so Reset Joints does not reset the joints.

----Overview of Hierarchical Model---

My model is the simple puppet as described in the Assignment 3 spec, but 
with a long pointy nose on the head to distinguish it for rotations.

Torso -> 
    Shoulders 
        -> Upper Arms -> Fore Arms -> Hands
        -> Neck -> Head -> Nose
    Hips
        -> Thighs -> Calves -> Feet