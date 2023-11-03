WARNING_CFLAGS = -Wall -Wextra -Wpedantic -Wno-missing-braces
DEBUG_CFLAG = -g
NOCMD_CFLAG = -mwindows
FLTO_CFLAG = -flto
WINDOWS_CFLAGS = -Os -std=c99 -I include/win -L lib/win -lraylib -lopengl32 -lgdi32 -lwinmm -luser32 -lkernel32 -lshell32
FILE_OUTPUT_WIN = suzu-music-win.exe
FILE_OUTPUT_LINUX = suzu-music-linux
LINUX_CFLAGS = -Os -std=c99 -I include/ -L lib/ -lraylib -lm -ldl -lpthread -lX11 -lxcb -lGL -lGLX -lXext -lGLdispatch -lXau -lXdmcp
PLATFORM := $(shell uname)

release: suzu_music_player.c ./lib ./include
ifeq ($(PLATFORM), Linux)
	echo "Building RELEASE for Linux"
	gcc suzu_music_player.c -o suzu-music-linux $(WARNING_CFLAGS) $(FLTO_CFLAG) $(LINUX_CFLAGS) 
	echo "==Compilation Complete=="
	./suzu-music-linux
else ifeq ($(PLATFORM), Windows_NT)
	echo "Building RELEASE for Windows"
endif

debug: suzu_music_player.c ./lib ./include
ifeq ($(PLATFORM), Linux)
	echo "Building DEBUG for Linux"
	gcc suzu_music_player.c -o suzu-music-linux-debug $(WARNING_CFLAGS) $(DEBUG_CFLAG) $(LINUX_CFLAGS) 
	echo "==Compilation Complete=="
	./suzu-music-linux-debug
else ifeq ($(PLATFORM), Windows_NT)
	echo "Building DEBUG for Windows"
endif

./lib: lib/libraylib.a 

./lib/libraylib.a: ../raylib/src/libraylib.a
	rm -rf ./lib
	mkdir -p lib
	cp ../raylib/src/libraylib.a ./lib/

./include: include/raylib.h 

./include/raylib.h: ../raylib/src/raylib.h
	rm -rf ./include
	mkdir -p include
	cp ../raylib/src/raylib.h ./include/

../raylib/src/libraylib.a: 
	$(MAKE) -C ../raylib/src/ -f Makefile PLATFORM=PLATFORM_DESKTOP

../raylib/src/raylib.h: 
	$(MAKE) -C ../raylib/src/ -f Makefile PLATFORM=PLATFORM_DESKTOP

clean:
	$(MAKE) -C ../raylib/src/ -f Makefile clean
	rm -rf ./lib
	rm -rf ./include
ifeq ($(PLATFORM), Linux)
	rm suzu-music-linux 
	rm suzu-music-linux-debug
else ifeq ($(PLATFORM), Windows_NT)
endif
##windows: suzu_music_player.c
##	echo "Compiling app to windows build"
##	gcc suzu_music_player.c -o suzu-music-win.exe $(WARNING_CFLAGS) $(WINDOWS_CFLAGS) $(NOCMD_CFLAG)
##	./suzu-music-win.exe
##
##windows-debug: suzu_music_player.c
##	echo "Compiling app to windows debug build"
##	gcc suzu_music_player.c -o suzu-music-win.exe $(DEBUG_CFLAG) $(WARNING_CFLAGS) $(WINDOWS_CFLAGS) $(NOCMD_CFLAG)
##	./suzu-music-win.exe
##
##linux: suzu_music_player.c
##	echo "Compiling app to linux build"
##	gcc suzu_music_player.c -o suzu-music-linux $(WARNING_CFLAGS) $(FLTO_CFLAG) $(LINUX_CFLAGS) 
##	./suzu-music-linux
##
##linux-debug: suzu_music_player.c
##	echo "Compiling app to linux debug build"
##	gcc suzu_music_player.c -o suzu-music-linux $(DEBUG_CFLAG) $(WARNING_CFLAGS) $(FLTO_CFLAG) $(LINUX_CFLAGS) 
##	./suzu-music-linux
##
##cross-platform: 
##	ifeq ($(OS), Linux)
##
##
##clean: ##clean stuff
