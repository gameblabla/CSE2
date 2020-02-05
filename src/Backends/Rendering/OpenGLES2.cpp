// I would use -DUSE_OPENGLES2 when RENDERER=OpenGLES2 is passed instead, but CMake likes to rebuild the entire
// project if I do that, and Make doesn't bother rebuilding anything at all. This method avoids both of those problems.
#define USE_OPENGLES2
#include "OpenGL3.cpp"
