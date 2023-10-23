#include "raylib.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ARRAY_LEN(xs) sizeof(xs) / sizeof(xs[0])

#define INITIALSCREEN_WIDTH 1280
#define INITIALSCREEN_HEIGHT 720

#define MOCHABASE                                                              \
  (Color) { 30, 30, 46, 255 }
#define MOCHATEXT                                                              \
  (Color) { 205, 214, 244, 255 }
#define MOCHABLUE                                                              \
  (Color) { 137, 180, 250, 255 }
#define MOCHARED                                                               \
  (Color) { 243, 139, 168, 255 }
#define MOCHAOVERLAY0                                                          \
  (Color) { 108, 112, 134, 255 }

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------

typedef struct {
  float left;
  float right;
} Frame;

size_t global_buffer_size;
Frame *global_frame_buffer;
size_t global_frames_count = 0;
float pi;

void callback(void *bufferData, unsigned int frames) {
  size_t buffer_capacity = global_buffer_size;
  if (frames <=
      buffer_capacity -
          global_frames_count) { // if still have space in buffer in frames
    memcpy(global_frame_buffer + global_frames_count, bufferData,
           sizeof(Frame) * frames);
    global_frames_count += frames;
  } else if (frames <= buffer_capacity) { // if no space in buffer
    memmove(global_frame_buffer,
            global_frame_buffer +
                (frames - buffer_capacity + global_frames_count),
            sizeof(Frame) * (global_frames_count - frames + buffer_capacity -
                             global_frames_count));
    global_frames_count =
        global_frames_count - frames + buffer_capacity - global_frames_count;
    memcpy(global_frame_buffer + global_frames_count, bufferData,
           sizeof(Frame) * frames);
    global_frames_count = buffer_capacity;

  } else {
    printf("WARNING:Buffer overload\n");
    memcpy(global_frame_buffer, bufferData, sizeof(Frame) * buffer_capacity);
    global_frames_count = buffer_capacity;
  }
}

int main(void) {

  // Initialization
  //--------------------------------------------------------------------------------------
	pi = atan2f(1, 1) * 4;
	float x = pi / 180; 
  bool trackLoaded = false;
  // SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(INITIALSCREEN_WIDTH, INITIALSCREEN_HEIGHT, "Music Visualizer");
  InitAudioDevice();
  SetAudioStreamBufferSizeDefault(65536);
  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
  Music currentMusic;
  float currentVolume = 0.75;

  const char *headerText = "Music Visualizer";
  const char *normalText = "Drag and drop an audio file to get started (.mp3)";
  char *currentFileName;

  int Heading1Size = 48.0f;
  int mainTextSize = 24.0f;

  //--------------------------------------------------------------------------------------

  Font HeadingFont = LoadFontEx("resources/Inter-Bold.ttf", Heading1Size, 0, 0);
  Font NormalTextFont =
      LoadFontEx("resources/Inter-Regular.ttf", mainTextSize, 0, 0);
  GenTextureMipmaps(&NormalTextFont.texture);
  GenTextureMipmaps(&HeadingFont.texture);

  // get Vector2 x and y values
  Vector2 HeadingVec2 = MeasureTextEx(HeadingFont, headerText, Heading1Size, 0);
  HeadingVec2.x = HeadingVec2.x / 2;
  HeadingVec2.y = HeadingVec2.y / 2;
  Vector2 NormalTextVec2 =
      MeasureTextEx(NormalTextFont, normalText, mainTextSize, 0);
  NormalTextVec2.x = NormalTextVec2.x / 2;
  NormalTextVec2.y = NormalTextVec2.y / 2;
  Vector2 currentFileTextVec2;

  SetTextureFilter(HeadingFont.texture, TEXTURE_FILTER_POINT);
  SetTextureFilter(NormalTextFont.texture, TEXTURE_FILTER_POINT);

  float sample_rate;
  size_t buff_size;
  Vector2 HeadingPosition;
  Vector2 TextPosition;
  Vector2 currentFileTextPosition;

  Vector2 *pHeadingPosition = &HeadingPosition;
  Vector2 *pTextPosition = &TextPosition;
  Vector2 *pcurrentFileTextPosition = &currentFileTextPosition;
  //  Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    if (trackLoaded) {
      UpdateMusicStream(currentMusic);
      if (IsKeyReleased(KEY_SPACE)) {
        if (!IsMusicStreamPlaying(currentMusic)) {
          printf("RESUMED\n");
          ResumeMusicStream(currentMusic);
        } else if (IsMusicStreamPlaying(currentMusic)) {
          printf("PAUSED\n");
          PauseMusicStream(currentMusic);
        }
      }
      if (IsKeyReleased(KEY_R)) {
        StopMusicStream(currentMusic);
        PlayMusicStream(currentMusic);
      }
      if (GetMouseWheelMove() != 0) {
        currentVolume += GetMouseWheelMove() * 0.05;
        if (currentVolume < 0) {
          currentVolume = 0;
        }
        if (currentVolume > 1) {
          currentVolume = 1;
        }
        SetMusicVolume(currentMusic, currentVolume);
      }
    }
    if (IsFileDropped()) {
      FilePathList droppedFiles = LoadDroppedFiles();
      printf("Dropped File: %s\n", droppedFiles.paths[0]);
      Music tempMusic = LoadMusicStream(droppedFiles.paths[0]);
      if (IsMusicReady(tempMusic)) {
        if (trackLoaded) {
          StopMusicStream(currentMusic);
          DetachAudioStreamProcessor(currentMusic.stream, callback);
          free(global_frame_buffer);
          free(currentFileName);
        }
        currentMusic = LoadMusicStream(droppedFiles.paths[0]);
        UnloadMusicStream(tempMusic);
        sample_rate = currentMusic.stream.sampleRate;
        buff_size = sample_rate / 100 *
                    (6.0 / 60.0 * 100.0); // Capture per frame of data
        global_buffer_size = buff_size;
        global_frame_buffer =
            malloc(global_buffer_size * sizeof(*global_frame_buffer));
        for (size_t i = 0; i < buff_size; i++) {
          global_frame_buffer[i].left = 0.0;
          global_frame_buffer[i].right = 0.0;
        }
        AttachAudioStreamProcessor(currentMusic.stream, callback);
        SetMusicVolume(currentMusic, currentVolume);
        printf("Scan buffer size: %zu\n", global_buffer_size);
        currentFileTextVec2 = MeasureTextEx(
            NormalTextFont, GetFileNameWithoutExt(droppedFiles.paths[0]),
            mainTextSize, 0);
        currentFileTextVec2.x = currentFileTextVec2.x / 2;
        currentFileTextVec2.y = currentFileTextVec2.y / 2;
        currentFileName =
            malloc(strlen(GetFileNameWithoutExt(droppedFiles.paths[0]) + 1));
        strcpy(currentFileName, GetFileNameWithoutExt(droppedFiles.paths[0]));
        PlayMusicStream(currentMusic);
        UnloadDroppedFiles(droppedFiles);
        trackLoaded = true;
      } else {
        printf("Unsupported File, music not loaded\n");
        UnloadDroppedFiles(droppedFiles);
      }
    }

    // Update
    //----------------------------------------------------------------------------------
    // TODO: Update your variables here
    //----------------------------------------------------------------------------------
    int w = GetRenderWidth();
    int h = GetRenderHeight();
    pHeadingPosition->x = w * 0.5 - HeadingVec2.x;
    pHeadingPosition->y = h * 0.5 - HeadingVec2.y - 25;

    pTextPosition->x = w * 0.5 - NormalTextVec2.x;
    pTextPosition->y = h * 0.5 - NormalTextVec2.y + 25;

    pcurrentFileTextPosition->x = w * 0.5 - currentFileTextVec2.x;
    pcurrentFileTextPosition->y = h * 0.5 - currentFileTextVec2.y + h * 0.25;
    // TEXT IS CENTERED Draw
    //----------------------------------------------------------------------------------
    float seek_ratio = 0;
    if (trackLoaded) {
      seek_ratio =
          GetMusicTimePlayed(currentMusic) / GetMusicTimeLength(currentMusic);
    }

    BeginDrawing();

    ClearBackground(MOCHABASE);

    size_t stride = 8;
    int cell_width = 1;
    if (trackLoaded) {
      for (size_t i = 0; i < global_frames_count; i += stride) {
        float l = global_frame_buffer[i].left;
        float r = global_frame_buffer[i].right;
        float t = (l + r) * 0.5;
        float scale = 0.8;

        if (t > 0) {
          DrawRectangle(i * ((float)w / global_frames_count),
                        h * 0.5 - h * 0.5 * t * currentVolume * scale + 1,
                        1 * cell_width, h * 0.5 * t * currentVolume * scale,
                        MOCHABLUE);
        } else {
          DrawRectangle(i * ((float)w / global_frames_count), h / 2 - 1,
                        1 * cell_width, -(h * 0.5 * t * currentVolume * scale),
                        MOCHARED);
        }
      }

      DrawTextEx(NormalTextFont, currentFileName, *pcurrentFileTextPosition,
                 mainTextSize, 0, MOCHATEXT);
      DrawRectangle(0, h - h * 0.05, w * seek_ratio, h * 0.05, MOCHAOVERLAY0);
    } else {

      DrawTextEx(HeadingFont, headerText, *pHeadingPosition, Heading1Size, 0,
                 MOCHATEXT);
      DrawTextEx(NormalTextFont, normalText, *pTextPosition, mainTextSize, 0,
                 MOCHATEXT);
    }
    DrawFPS(50, 50);
    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  UnloadFont(HeadingFont);
  UnloadFont(NormalTextFont);
  if (trackLoaded) {
    UnloadMusicStream(currentMusic);
    DetachAudioStreamProcessor(currentMusic.stream, callback);
    free(global_frame_buffer);
    free(currentFileName);
  }
  CloseAudioDevice();
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}
