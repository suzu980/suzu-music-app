WARNING_CFLAGS = -Wall -Wextra -Wpedantic -Wno-missing-braces
DEBUG_CFLAG = -g
NOCMD_CFLAG = -mwindows
WINDOWS_CFLAGS = -Os -std=c99 -I include/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm -luser32 -lkernel32 -lshell32

windows: suzu_music_player.c
	echo "Compiling app to windows build"
	gcc suzu_music_player.c -o suzu-music-win.exe $(WARNING_CFLAGS) $(WINDOWS_CFLAGS) $(NOCMD_CFLAG)
