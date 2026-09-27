# ProjectXA-X-Treme_Attempt
Project XA: (short for X-treme Attempt) is my attempt to create a level reader and viewer for the various versions and repositories of original level files for the cancelled 1998 game "Sonic X-treme" in C++, SDL2 and OpenGL.

**DEPENDANCIES:**

* Assimp

* Glew

* Glm

* SDL2

* SDL2_image

* SDL2_ttf

**NOTES:**

Previous code of mine from another project is used as base-plate codebase

AI only used for debugging and finding equivelants of Unity/C# functions to C++/SDL

**SPECIAL THANKS:**

Voxel's Level Reader used as reference (https://archive.org/details/xtreme-level-reader-source.-7z)

**CURRENT FEATURES:**

* Ability to read and print details of Cubes & Textures from version 40 & 42 .DEF files 

* Detect and locate Level layout file from "QUBIX" value in .DEF files

* Read And build levels from level layouts (only single block type and texture currently implemented)

* Moveable Camera

**PLANNED FEATURES:**

* Assign Material to cubes

* Render materials
  
* Non-Cube Meshes
  
* Animated Textures
  
* Texture Scroll
  
* Ai Textures
  
* Billboards
  
* Version 37 and Prior level compatability
