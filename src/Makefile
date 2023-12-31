WARNING_CFLAGS = -Wall -Wextra -Wpedantic -Wno-missing-braces
DEBUG_CFLAG = -g
NOCMD_CFLAG = -mwindows
FLTO_CFLAG = -flto
WINDOWS_CFLAGS = -Os -std=c99 -I include/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm -luser32 -lkernel32 -lshell32
FILE_OUTPUT_WIN = suzu-music-win.exe
FILE_OUTPUT_LINUX = suzu-music-linux
LINUX_CFLAGS = -Os -std=c99 -I include/ -L lib/ -lraylib -lm -ldl -lpthread -lX11 -lxcb -lGL -lGLX -lXext -lGLdispatch -lXau -lXdmcp

release: suzu_music_player.c ./lib ./include
ifeq ($(OS), Windows_NT)
	echo "Building RELEASE for Windows"
	gcc suzu_music_player.c -o suzu-music-win.exe $(WARNING_CFLAGS) $(WINDOWS_CFLAGS) $(NOCMD_CFLAG)
	.\suzu-music-win.exe
else
	echo "Building RELEASE for Linux"
	gcc suzu_music_player.c -o suzu-music-linux $(WARNING_CFLAGS) $(FLTO_CFLAG) $(LINUX_CFLAGS) 
	echo "==Compilation Complete=="
	./suzu-music-linux
endif

build-release: suzu_music_player.c ./lib ./include
ifeq ($(OS), Windows_NT)
	echo "Building RELEASE for Windows"
	gcc suzu_music_player.c -o suzu-music-win.exe $(WARNING_CFLAGS) $(WINDOWS_CFLAGS) $(NOCMD_CFLAG)
else
	echo "Building RELEASE for Linux"
	gcc suzu_music_player.c -o suzu-music-linux $(WARNING_CFLAGS) $(FLTO_CFLAG) $(LINUX_CFLAGS) 
	echo "==Compilation Complete=="
endif

debug: suzu_music_player.c ./lib ./include
ifeq ($(OS), Windows_NT)
	echo "Building DEBUG for Windows"
	gcc suzu_music_player.c -o suzu-music-win-debug.exe $(DEBUG_CFLAG) $(WARNING_CFLAGS) $(WINDOWS_CFLAGS) $(NOCMD_CFLAG)
	.\suzu-music-win-debug.exe
else 
	echo "Building DEBUG for Linux"
	gcc suzu_music_player.c -o suzu-music-linux-debug $(WARNING_CFLAGS) $(DEBUG_CFLAG) $(LINUX_CFLAGS) 
	echo "==Compilation Complete=="
	./suzu-music-linux-debug
endif


build-debug: suzu_music_player.c ./lib ./include
ifeq ($(OS), Windows_NT)
	echo "Building DEBUG for Windows"
	gcc suzu_music_player.c -o suzu-music-win-debug.exe $(DEBUG_CFLAG) $(WARNING_CFLAGS) $(WINDOWS_CFLAGS) $(NOCMD_CFLAG)
else 
	echo "Building DEBUG for Linux"
	gcc suzu_music_player.c -o suzu-music-linux-debug $(WARNING_CFLAGS) $(DEBUG_CFLAG) $(LINUX_CFLAGS) 
	echo "==Compilation Complete=="
endif

./lib: lib/libraylib.a 

./lib/libraylib.a: ../raylib/src/libraylib.a
ifeq ($(OS), Windows_NT)
	powershell mkdir lib
	powershell cp ..\raylib\src\libraylib.a .\lib
else 
	mkdir -p lib
	cp ../raylib/src/libraylib.a ./lib/
endif

./include: include/raylib.h 

./include/raylib.h: ../raylib/src/raylib.h
ifeq ($(OS), Windows_NT)
	powershell mkdir include
	powershell cp ..\raylib\src\raylib.h .\include
else 
	mkdir -p include
	cp ../raylib/src/raylib.h ./include/
endif

../raylib/src/libraylib.a: 
	$(MAKE) -C ../raylib/src/ -f Makefile PLATFORM=PLATFORM_DESKTOP

../raylib/src/raylib.h: 
	$(MAKE) -C ../raylib/src/ -f Makefile PLATFORM=PLATFORM_DESKTOP

clean:
ifeq ($(OS), Windows_NT)
	if exist lib powershell rm -r lib
	if exist include powershell rm -r include
	if exist suzu-music-win.exe powershell rm suzu-music-win.exe
	if exist suzu-music-win-debug.exe powershell rm suzu-music-win-debug.exe
else 
	rm -rfv ./lib
	rm -rfv ./include
	rm -f suzu-music-linux 
	rm -f suzu-music-linux-debug
endif
	$(MAKE) -C ../raylib/src/ -f Makefile clean
