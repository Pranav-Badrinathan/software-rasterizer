## A simple Software Rasterizer

This was primarily made as an exercise to try to learn and understand how graphics rendering works. A GPU usually abstracts away a lot of the details regarding how pixels are rendered into complex shapes and effects, and the math behind it. Being interested in this aspect of rendering, I decided to make a software rasterizer myself.

Reminicient of the early days of gaming, this program could theoretically be used to make stylized games like [Starfox](https://en.wikipedia.org/wiki/Star_Fox_%281993_video_game%29), [Doom](https://en.wikipedia.org/wiki/Doom_(1993_video_game)), etc.

## Building and Running

This program uses the SDL2 library to draw to the screen, handle windows, etc. To run the program, you'll need to download [SDL2 from its github page](https://github.com/libsdl-org/SDL/releases/) and place the SDL2 binary file (found in the `bin` folder inside the zip file) either on your OS's PATH, or adjacent to the compiled executable.

To supply your own SDL2 binary, create a folder called `lib` in the main folder, and extract the contents of the 64-bit version of SDL2 into the folder. It should look something like this:
```
software-renderer
|- lib
    |- SDL
        |- bin
        |- include
        |- lib
        |- share
|- src 
    |- ...
|- .gitignore
|- Makefile
```

Then, just run the make file with `make init`, which creates a new `bin` folder and copies the SDL binary into it. Running `make` after should build and run the project.

You only really use this sort of system on Windows or MacOS. On Linux, SDL2 is probably availaible to you via your distro's package manager. So ideally you would just use that as the shared runtime.

## To do
- [x] Draw Lines.
- [x] Drawing triangles.
- [x] 3D Projection.
    - [x] Model matrix.
    - [x] View matrix.
    - [x] Perspective matrix.
    - [x] Perspective division (**N**ormalized **D**evice **C**oordinates).
    - [x] Hide backfaces. Use triangle face normals and camera direction vector to stop them rendering.
    - [ ] Convert to Clip Space.
        - [ ] Discard vertices outside screen bounds.
        - [ ] Re-triangulate broken triangles.
- [ ] Set Camera movement to be relative to it's current orientation (not global).
- [ ] 3D model importer (.obj).
- [ ] An option to fill in faces. Shading?
- [ ] Ray-tracing to figure out what object camera is looking at?
- [ ] Simple UI to edit onscreen objects' transform properties.
- [ ] Write tests (begin with linalg) and add Github Workflows.
- [ ] Switch to using CMake as build system (removes mingw dependency for Windows, auto download SDL)
- [ ] Quaternion rotations?
