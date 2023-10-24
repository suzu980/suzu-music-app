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
            sizeof(Frame) * (buffer_capacity - frames));
    global_frames_count = buffer_capacity - frames;
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
  bool wireframe = false;
  SetConfigFlags(FLAG_WINDOW_ALWAYS_RUN);
  InitWindow(INITIALSCREEN_WIDTH, INITIALSCREEN_HEIGHT, "Music Visualizer");
  InitAudioDevice();
  SetAudioStreamBufferSizeDefault(65536);
  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
  Music currentMusic;
  float currentVolume = 0.75;
  Shader blurShader = LoadShader(0, "resources/shaders/blur.fs");
  Shader lightFilterShader = LoadShader(0, "resources/shaders/light_filter.fs");
  Shader glowPassShader = LoadShader(0, "resources/shaders/glow_pass.fs");

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

  RenderTexture2D target =
      LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
  RenderTexture2D targethdr =
      LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
  int horLoc = GetShaderLocation(blurShader, "horizontal");
	int cLoc = GetShaderLocation(glowPassShader, "blurred");

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
      if (IsKeyReleased(KEY_G)) {
        wireframe = !wireframe;
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
          UnloadMusicStream(currentMusic);
          free(global_frame_buffer);
          free(currentFileName);
          global_frames_count = 0;
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

    int w = GetRenderWidth();
    int h = GetRenderHeight();
    pHeadingPosition->x = w * 0.5 - HeadingVec2.x;
    pHeadingPosition->y = h * 0.5 - HeadingVec2.y - 25;

    pTextPosition->x = w * 0.5 - NormalTextVec2.x;
    pTextPosition->y = h * 0.5 - NormalTextVec2.y + 25;

    pcurrentFileTextPosition->x = w * 0.5 - currentFileTextVec2.x;
    pcurrentFileTextPosition->y = h * 0.5 - currentFileTextVec2.y + h * 0.35;

    float seek_ratio = 0;
    if (trackLoaded) {
      seek_ratio =
          GetMusicTimePlayed(currentMusic) / GetMusicTimeLength(currentMusic);
    }

    size_t stride = (!wireframe) ? 16 : 2; 
    int cell_width = 3;
    if (trackLoaded) {
      BeginTextureMode(target);
      ClearBackground(MOCHABASE);
      for (size_t i = 0;
           i < global_frames_count && (i + stride) < global_frames_count;
           i += stride) {
        float l = global_frame_buffer[i].left;
        float l2 = global_frame_buffer[i + stride].left;
        float r = global_frame_buffer[i].right;
        float r2 = global_frame_buffer[i + stride].right;
        float t = (l + r) * 0.5;
        float t2 = (l2 + r2) * 0.5;
        float scale = 0.75;

        if (t > 0) {
          (!wireframe)
              ? DrawRectangle(i * ((float)w / global_frames_count),
                              h * 0.5 - h * 0.5 * t * scale + 1, 1 * cell_width,
                              h * 0.5 * t * scale, MOCHABLUE)
              : DrawLine(i * ((float)w / global_frames_count),
                         h * 0.5 - h * 0.5 * t * scale,
                         (i + stride) * ((float)w / global_frames_count),
                         h * 0.5 - h * 0.5 * t2 * scale, MOCHABLUE);
        } else {
          (!wireframe)
              ? DrawRectangle(i * ((float)w / global_frames_count), h * 0.5 - 1,
                              1 * cell_width, -(h * 0.5 * t * scale), MOCHARED)
              : DrawLine(i * ((float)w / global_frames_count),
                         h * 0.5 - (h * 0.5 * t * scale),
                         (i + stride) * ((float)w / global_frames_count),
                         h * 0.5 - (h * 0.5 * t2 * scale), MOCHARED);
        }
      }
      EndTextureMode();

      BeginTextureMode(targethdr);
      BeginShaderMode(lightFilterShader);
      DrawTextureRec(target.texture,
                     (Rectangle){0, 0, (float)target.texture.width,
                                 (float)-target.texture.height},
                     (Vector2){0, 0}, WHITE);
      EndShaderMode();
      EndTextureMode();
      int horizontal = true;
      int amt = 10;
      for (int i = 0; i < amt; i++) {
        SetShaderValue(blurShader, horLoc, &horizontal, SHADER_UNIFORM_INT);
        BeginTextureMode(targethdr);
        BeginShaderMode(blurShader);
        DrawTextureRec(targethdr.texture,
                       (Rectangle){0, 0, (float)targethdr.texture.width,
                                   (float)-targethdr.texture.height},
                       (Vector2){0, 0}, WHITE);
        EndShaderMode();
        EndTextureMode();
        horizontal = !horizontal;
      }

			// DRAW
      BeginDrawing();
      ClearBackground(MOCHABASE);
			BeginShaderMode(glowPassShader);
			SetShaderValueTexture(glowPassShader, cLoc, targethdr.texture);
      DrawTextureRec(target.texture,
                     (Rectangle){0, 0, (float)target.texture.width,
                                 (float)-target.texture.height},
                     (Vector2){0, 0}, WHITE);
			EndShaderMode();
      DrawTextEx(NormalTextFont, currentFileName, *pcurrentFileTextPosition,
                 mainTextSize, 0, MOCHATEXT);
      DrawRectangle(0, h - h * 0.05, w * seek_ratio, h * 0.05, MOCHAOVERLAY0);
      //DrawFPS(50, 50);
      EndDrawing();
    } else {
      BeginDrawing();
      ClearBackground(MOCHABASE);

      DrawTextEx(HeadingFont, headerText, *pHeadingPosition, Heading1Size, 0,
                 MOCHATEXT);
      DrawTextEx(NormalTextFont, normalText, *pTextPosition, mainTextSize, 0,
                 MOCHATEXT);
      //DrawFPS(50, 50);
      EndDrawing();
    }

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
  UnloadShader(blurShader);
	UnloadShader(lightFilterShader);
	UnloadShader(glowPassShader);
  CloseAudioDevice();
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}
