/*
Unity build for raylib.
Makes it easy to build raylib into a single library by compiling just this source file.
*/

// cc -c shapes.c -Wall -D_DEFAULT_SOURCE -Wno-missing-braces -Werror=pointer-arith -fno-strict-aliasing -std=c99 -fPIC -s -O1 -Werror=implicit-function-declaration -I. -Iexternal/glfw/include -Iexternal/glfw/deps/mingw -DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_33
// cc -c raylib.c -Wall -Wno-missing-braces -Werror=pointer-arith -fno-strict-aliasing -std=c99 -fPIC -s -O1 -Werror=implicit-function-declaration -I.

#include "raylib.h"

// #include "rlgl.h"
#include "core.c"
// #include "models.c"
// #include "raudio.c"
// #include "rglfw.c"
// #include "shapes.c"
#include "text.c"
// #include "utils.c"
