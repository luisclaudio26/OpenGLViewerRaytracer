# CG-Project [https://github.com/luisclaudio26/CG-Project/]
Project for Computer Graphics class in UFRN, 2016.

1. Compiling

Once you have unzipped the code, simply MAKE it.

[Skip this if you don't care about what I did inside the makefile]

Notice that this operation takes an amount of time which is maybe too much for such a small code, but it seems that C++ 11 regular expressions are parsed (have their automats built) in compile time, so the extra time. This is not important, anyway.
Obviously I linked the GL, GLFW and GLEW libraries, so make sure you have the needed headers and SDKs installed. I had to manually set the include paths for GLFW (as you can see in the -I /usr/include/GLFW line); I'm running Fedora 21, so I do not know whether one should always manually do this or not.
Finally, notice that I'm compiling things with NO compiler optimization, because it makes debugging with GDB easier.

2. Running

To run, simply type ./render, then the path to the .obj file and the interpolation type we're using, gouraud or flat:

  ./render ./path/to/file.obj gouraud OR
  ./render ./path/to/file.obj flat
  
CAVEAT: This simple renderer expects to receive .obj files with ONE AND ONLY ONE model, so it is possible that files with more then one model inside it won't work (I have not tested it).
Also, the .obj parser is very simple and can break for very subtle reasons: in one of the tests I did (sphere.obj), it broke because of a "/r" character in the end of a line. Summarizing: use "correct" models (as the ones in this repository, except for sphere.obj).
