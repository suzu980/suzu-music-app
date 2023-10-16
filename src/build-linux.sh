#!/bin/sh

set -e

# Get arguments
while getopts ":hdusrcq" opt; do
    case $opt in
        h)
            echo "Usage: ./build-linux.sh [-hdusrcqq]"
            echo " -h  Show this information"
            echo " -c  Build All"
            echo " -d  Faster builds that have debug symbols, and enable warnings"
            echo " -r  Run the executable after compilation"
            echo ""
            echo ""
            echo "Examples:"
            echo " Build a release build:                    ./build-linux.sh"
            echo " Build a debug build and run:              ./build-linux.sh -d -r"
            exit 0
            ;;
				c)
            BUILD_ALL="1"
            ;;
        d)
            BUILD_DEBUG="1"
            ;;
        r)
            RUN_AFTER_BUILD="1"
            ;;
        \?)
            echo "Invalid option: -$OPTARG" >&2
            exit 1
            ;;
    esac
done



RAYLIB_SRC="../raylib/src"
SOURCES="suzu_music_player.c"
GAME_NAME="suzu_music_player"
APP_RESOURCE_DIR="resources"

#set directories
ROOT_DIR=$PWD
SOURCES="$ROOT_DIR/$SOURCES"
RAYLIB_SRC="$ROOT_DIR/$RAYLIB_SRC"

#Compilation Flags
OUTPUT_DIR="builds/linux"
COMPILATION_FLAGS="-std=c99 -Os -flto" 
FINAL_COMPILE_FLAGS="-s"
WARNING_FLAGS="-Wall -Wextra -Wpedantic"
LINK_FLAGS="-flto -lm -ldl -lpthread -lX11 -lxcb -lGLX -lXext -lGLdispatch -lXau -lXdmcp"
CC=cc

# Debug changes to flags
if [ -n "$BUILD_DEBUG" ]; then
		echo "Building in DEBUG mode..."
    OUTPUT_DIR="builds-debug/linux"
    COMPILATION_FLAGS="-std=c99 -O0 -g"
    FINAL_COMPILE_FLAGS=""
    LINK_FLAGS="-lm -ldl -lpthread -lX11 -lxcb -lGL -lGLX -lXext -lGLdispatch -lXau -lXdmcp"
else
	echo "Building in RELEASE mode"
fi

#Build raylib
#If temp directory doesn't exist, build raylib
TEMP_DIR="raylib" 

if [ -n "$BUILD_ALL" ]; then
	echo "Rebuilding all.."
	if [ -d "$TEMP_DIR" ]; then 
		rm -r $TEMP_DIR
	fi
fi

if [ ! -d "$TEMP_DIR" ]; then 
	echo "Building Raylib..."
	mkdir -p $TEMP_DIR
	cd $TEMP_DIR
	RAYLIB_DEFINES="-D_DEFAULT_SOURCE -DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_33"
	RAYLIB_C_FILES="$RAYLIB_SRC/rcore.c $RAYLIB_SRC/rshapes.c $RAYLIB_SRC/rtextures.c $RAYLIB_SRC/rtext.c $RAYLIB_SRC/rmodels.c $RAYLIB_SRC/utils.c $RAYLIB_SRC/raudio.c $RAYLIB_SRC/rglfw.c"
	RAYLIB_INCLUDE_FLAGS="-I$RAYLIB_SRC -I$RAYLIB_SRC/external/glfw/include"
	$CC -c $RAYLIB_DEFINES $RAYLIB_INCLUDE_FLAGS $COMPILATION_FLAGS $RAYLIB_C_FILES
	cd $ROOT_DIR
fi
mkdir -p $OUTPUT_DIR
cd $OUTPUT_DIR
$CC -c -I$RAYLIB_SRC $COMPILATION_FLAGS $WARNING_FLAGS $SOURCES
$CC -o $GAME_NAME $ROOT_DIR/$TEMP_DIR/*.o *.o $LINK_FLAGS

echo "Build Completed~"
# Cleanup
rm *.o
cd $ROOT_DIR
cp -r ./$APP_RESOURCE_DIR/ ./$OUTPUT_DIR/
if [ -n "$RUN_AFTER_BUILD" ]; then
	./$OUTPUT_DIR/$GAME_NAME
fi
