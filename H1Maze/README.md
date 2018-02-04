# H1 Maze GL 
# Maze Explorer Win32 ConsoleApplication

### OpenGL API reference
Link: https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/

#### Steps 
```
Load the project in Visual studio, build and debug 
The code is reading a maze*.txt file along with texture images
Execute the following:
H1Maze.exe assets/maze2.txt
```

#### Features
```
1. Ability to load the maze*.txt file available in MzParser.cpp as a class
2. Load textures from Tiff files and copy to the device
2. Bind multiple texture to the floor and wall objects
3. Ability to make movements using translate (a,w,s,d) and rotate (i,j,k,l) keys 

Pending: Actual Maze exploration with various Eye space & 2d perspective transform
		 Fix the rotation and translation action with respect to the world space.
```

-------------------------------------
 