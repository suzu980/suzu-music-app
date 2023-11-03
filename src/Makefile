WARNING_CFLAGS = -Wall -Wextra -Wpedantic -Wno-missing-braces
DEBUG_CFLAG = -g
NOCMD_CFLAG = -mwindows
FLTO_CFLAG = -flto
WINDOWS_CFLAGS = -Os -std=c99 -I include/win -L lib/win -lraylib -lopengl32 -lgdi32 -lwinmm -luser32 -lkernel32 -lshell32
LINUX_CFLAGS = -Os -std=c99 -I include/linux -L lib/linux -lraylib -lm -ldl -lpthread -lX11 -lxcb -lGL -lGLX -lXext -lGLdispatch -lXau -lXdmcp

windows: suzu_music_player.c
	echo "Compiling app to windows build"
	gcc suzu_music_player.c -o suzu-music-win.exe $(WARNING_CFLAGS) $(WINDOWS_CFLAGS) $(NOCMD_CFLAG)
	./suzu-music-win.exe

windows-debug: suzu_music_player.c
	echo "Compiling app to windows debug build"
	gcc suzu_music_player.c -o suzu-music-win.exe $(DEBUG_CFLAG) $(WARNING_CFLAGS) $(WINDOWS_CFLAGS) $(NOCMD_CFLAG)
	./suzu-music-win.exe

linux: suzu_music_player.c
	echo "Compiling app to linux build"
	gcc suzu_music_player.c -o suzu-music-linux $(WARNING_CFLAGS) $(FLTO_CFLAG) $(LINUX_CFLAGS) 
	./suzu-music-linux

linux-debug: suzu_music_player.c
	echo "Compiling app to linux debug build"
	gcc suzu_music_player.c -o suzu-music-linux $(DEBUG_CFLAG) $(WARNING_CFLAGS) $(FLTO_CFLAG) $(LINUX_CFLAGS) 
	./suzu-music-linux
