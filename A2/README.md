Compiling the program is done through the usual steps of: 

$ premake gmake4
$ make
$ ./A2

Manual:

I completed every objective except for supporting a resizable viewport. I ran out of time to get it working. 
However, if you go into the code, at lines 82 & 83, the variables `viewportHeight` and `viewportWidth` can be modified. 

Simply change it from: 
```
viewportWidth = 0.9 * m_framebufferWidth;
viewportHeight = 0.9 * m_framebufferHeight;
```
to something like:
```
viewportWidth = 0.4 * m_framebufferWidth;
viewportHeight = 0.6 * m_framebufferHeight;
```
and this will still show that viewports are partially supported. I'd like to request code credit here if possible!
