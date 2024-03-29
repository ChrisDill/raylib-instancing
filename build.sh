#!/usr/bin/bash
RAYLIB_DIR=src/raylib
FLAGS="-g -Wall -Werror -Wno-missing-braces"
INCLUDES="-I./src -I$RAYLIB_DIR/src -I$RAYLIB_DIR/src/external"
LIBRARIES="./src/raylib/src/libraylib.a -L$RAYLIB_DIR/src -lGL -lm -lpthread -ldl -lrt -lX11"

mkdir -p build

# Clean build output
find build -type f -executable -delete

# Build custom raylib
patch -u src/raylib/src/rlgl.h -i src/rlgl.patch
make -C $RAYLIB_DIR/src

# Build instancing examples
cc -o build/asteroids_instanced src/instancing/asteroids_instanced.c $FLAGS $INCLUDES $LIBRARIES
cc -o build/particles_instanced src/instancing/particles_instanced.c $FLAGS $INCLUDES $LIBRARIES
cc -o build/textures_bunnymark_instanced src/instancing/textures_bunnymark_instanced.c $FLAGS $INCLUDES $LIBRARIES
cc -o build/quads_instanced src/instancing/quads_instanced.c $FLAGS $INCLUDES $LIBRARIES
cc -o build/shapes_instanced_2d src/instancing/shapes_instanced_2d.c $FLAGS $INCLUDES $LIBRARIES
cc -o build/shapes_instanced_3d src/instancing/shapes_instanced_3d.c $FLAGS $INCLUDES $LIBRARIES
