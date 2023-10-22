#include "raylib.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ARRAY_LEN(xs) sizeof(xs) / sizeof(xs[0])

//void drawFileList(FilePathList *pFileList, Font *pNormalText, int fontSize,
//                  Color *textColor, Vector2 *mousePos, Music *currentMusic,
//                  int *musicState, int *musicScrollValue);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
//void drawFileList(FilePathList *pFileList, Font *pNormalText, int fontSize,
//                  Color *textColor, Vector2 *mousePos, Music *currentMusic,
//                  int *musicState, int *musicScrollValue) {
//  int initialx = 500;
//  int initialy = 150;
//  int lineSpacing = 25;
//  int boxWidth = 500;
//  int boxHeight = 50;
//  int pl = 10;
//  int boxSpacing = 5;
//
//  int size = pFileList->count;
//  Vector2 positions[size];
//  Rectangle buttonBounds[size];
//  Vector2 textSize[size];
//  // TODO : DO NOT RENDER WHEN OFF SCREEN
//  for (int idx = 0; idx < size; ++idx) {
//    bool buttonAction = false;
//    positions[idx].x = initialx + pl;
//    positions[idx].y =
//        boxHeight * idx + initialy + (float)boxHeight / 2 -
//        MeasureTextEx(*pNormalText,
//                      GetFileNameWithoutExt(pFileList->paths[idx]), fontSize, 0)
//                .y /
//            2 +
//        boxSpacing * idx + *musicScrollValue;
//
//    buttonBounds[idx].x = initialx;
//    buttonBounds[idx].y =
//        initialy + boxHeight * idx + boxSpacing * idx + *musicScrollValue;
//    buttonBounds[idx].width = boxWidth;
//    buttonBounds[idx].height = boxHeight;
//    Color colorToUse = GRAY;
//    if (CheckCollisionPointRec(*mousePos, buttonBounds[idx])) {
//      colorToUse = WHITE;
//      if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
//        buttonAction = true;
//      }
//    }
//    DrawRectangle(initialx,
//                  initialy + boxHeight * idx + boxSpacing * idx +
//                      *musicScrollValue,
//                  boxWidth, boxHeight, colorToUse);
//    DrawTextEx(*pNormalText, GetFileNameWithoutExt(pFileList->paths[idx]),
//               positions[idx], fontSize, 0, *textColor);
//    if (buttonAction) {
//      if (*musicState != 0) {
//        TraceLog(LOG_INFO, "Music Unloaded");
//        UnloadMusicStream(*currentMusic);
//      }
//      *currentMusic = LoadMusicStream(pFileList->paths[idx]);
//      PlayMusicStream(*currentMusic);
//      *musicState = 1;
//    }
//  }
//}

typedef struct {
  float left;
  float right;
} Frame;

int *global_buffer_size;
Frame *global_frame_buffer;
size_t global_frames_count = 0;
int musicState = 0; // 0: None, 1:Playing, 2: Paused 3: Stopped

void callback(void *bufferData, unsigned int frames) {
  size_t buffer_capacity = *global_buffer_size;
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
  int screenWidth = 1280;
  int screenHeight = 720;
//  int musicScrollValue = 0;

  const int lineBreakSpacing = 20;
  //const int titleOffset = 250;

  Color baseColor = (Color){30, 30, 46, 255};
  Color textColor = (Color){166, 173, 200, 255};

  Color posColor = (Color){137, 180, 250, 255};
  Color negColor = (Color){243, 139, 168, 255};
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(screenWidth, screenHeight, "Music Visualizer");
  InitAudioDevice();
  Music currentMusic;
  // Music currentMusic =
  //     LoadMusicStream("resources/sample-music/nights.mp3");
  float currentVolume = 0.5;
  // unsigned int sampleRate = currentMusic.stream.sampleRate;
  //  Getting filelists
  FilePathList fileList =
      LoadDirectoryFilesEx("resources/sample-music", ".flac;.mp3;.m4a", true);

  const char *headerText = "Suzu Music Visualizer";
  const char *normalText = "Drag and drop an audio file to get started (.mp3)";
  char *currentFileText;

  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
  int Heading1Size = 48.0f;
  int mainSize = 24.0f;

  //--------------------------------------------------------------------------------------

  Font Heading1 = LoadFontEx("resources/Inter-Bold.ttf", Heading1Size, 0, 0);
  Font NormalText = LoadFontEx("resources/Inter-Regular.ttf", mainSize, 0, 0);
  GenTextureMipmaps(&NormalText.texture);
  GenTextureMipmaps(&Heading1.texture);

  // get Vector2 x and y values
  Vector2 HeadingVec2 = MeasureTextEx(Heading1, headerText, Heading1Size, 0);
  Vector2 NormalTextVec2 = MeasureTextEx(NormalText, normalText, mainSize, 0);
  Vector2 currentFileTextVec2;

  SetTextureFilter(Heading1.texture, TEXTURE_FILTER_POINT);
  SetTextureFilter(NormalText.texture, TEXTURE_FILTER_POINT);
//  Vector2 mousePoint = {0.0f, 0.0f};

  size_t sample_rate;
  int buff_size;
  // PlayMusicStream(currentMusic);
  //  Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    if (musicState != 0) {
      UpdateMusicStream(currentMusic);
      if (IsKeyReleased(KEY_SPACE)) {
        if (musicState == 2) {
          printf("RESUMED\n");
          ResumeMusicStream(currentMusic);
          musicState = 1;
        } else if (musicState == 1) {
          printf("PAUSED\n");
          PauseMusicStream(currentMusic);
          musicState = 2;
        }
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
      Music loadedMusic = LoadMusicStream(droppedFiles.paths[0]);
      bool musicIsReady = false;
      if (IsMusicReady(loadedMusic)) {
        printf("Music Stream ready!\n");
        musicIsReady = true;
      } else {
        printf("Unsupported File, music not loaded\n");
      }
      if (musicState != 0 && musicIsReady) {
        StopMusicStream(currentMusic);
        DetachAudioStreamProcessor(currentMusic.stream, callback);
        free(global_frame_buffer);
        free(currentFileText);
        UnloadMusicStream(currentMusic);
        musicState = 0;
      }
      if (musicState == 0 && musicIsReady) {
        currentMusic = LoadMusicStream(droppedFiles.paths[0]);
        SetAudioStreamBufferSizeDefault(32768);
        sample_rate = currentMusic.stream.sampleRate;
        buff_size = sample_rate / 100 *
                    (1.0 / 60.0 * 100.0); // Capture per frame of data
        global_buffer_size = &buff_size;
        global_frame_buffer =
            malloc(*global_buffer_size * sizeof(global_frame_buffer));
        for (size_t i = 0; i < 15; i++) {
          global_frame_buffer[i].left = 0.0;
          global_frame_buffer[i].right = 0.0;
        }
        AttachAudioStreamProcessor(currentMusic.stream, callback);
        SetMusicVolume(currentMusic, currentVolume);
        printf("Scan buffer size: %d\n", *global_buffer_size);
        PlayMusicStream(currentMusic);
        musicState = 1;
        currentFileTextVec2 = MeasureTextEx(
            NormalText, GetFileNameWithoutExt(droppedFiles.paths[0]), mainSize,
            0);
        currentFileText =
            malloc(strlen(GetFileNameWithoutExt(droppedFiles.paths[0]) + 1));
        strcpy(currentFileText, GetFileNameWithoutExt(droppedFiles.paths[0]));
      }
      UnloadDroppedFiles(droppedFiles);
      UnloadMusicStream(loadedMusic);
    }

    // Update
    //----------------------------------------------------------------------------------
    // TODO: Update your variables here
    //----------------------------------------------------------------------------------
    //    if (musicState == 1) {
    //      UpdateMusicStream(currentMusic);
    //    }
    //    mousePoint = GetMousePosition();
    //    int mouseTranslation = GetMouseWheelMove() * 16;
    //    if ((musicScrollValue + mouseTranslation) != 0) {
    //
    //      musicScrollValue += mouseTranslation;
    //    }
    //
    screenWidth = GetRenderWidth();
    screenHeight = GetRenderHeight();
    Vector2 HeadingPosition = {
        screenWidth / 2 - HeadingVec2.x / 2,
        screenHeight / 2 - HeadingVec2.y / 2 -
            lineBreakSpacing}; // TODO CHANGE THE WAY TEXT IS CENTERED
    Vector2 TextPosition = {
        screenWidth / 2 - NormalTextVec2.x / 2,
        screenHeight / 2 - NormalTextVec2.y / 2 +
            lineBreakSpacing}; // TODO CHANGE THE WAY TEXT IS CENTERED

    Vector2 currentFileTextPosition = {
        screenWidth / 2 - currentFileTextVec2.x / 2,
        screenHeight / 2 - currentFileTextVec2.y / 2 + lineBreakSpacing +
            screenHeight / 4}; // TODO CHANGE THE WAY TEXT IS CENTERED
    // Draw
    //----------------------------------------------------------------------------------

    BeginDrawing();

    ClearBackground(baseColor);
    int w = GetRenderWidth();
    int h = GetRenderHeight();

    int stride = 2;
    int cell_width = 1;
    if (musicState != 0) {
      for (size_t i = 0; i < global_frames_count; i += stride) {
        float l = global_frame_buffer[i].left;
        float r = global_frame_buffer[i].right;
        float t = (l + r) / 2;
        float scale = 0.8;

        if (t > 0) {
          DrawRectangle(i * ((float)w / global_frames_count),
                        h / 2 - h / 2 * t * currentVolume * scale + 1,
                        1 * cell_width, h / 2 * t * currentVolume * scale,
                        posColor);
        } else {
          DrawRectangle(i * ((float)w / global_frames_count), h / 2 - 1,
                        1 * cell_width, -(h / 2 * t * currentVolume * scale),
                        negColor);
        }
      }

      DrawTextEx(NormalText, currentFileText, currentFileTextPosition, mainSize,
                 0, textColor);
      //    DrawTextEx(Heading1, headerText, HeadingPosition, Heading1Size, 0,
      //               textColor);
      //    drawFileList(&fileList, &NormalText, mainSize, &textColor,
      //    &mousePoint,
      //                 &currentMusic, &musicState, &musicScrollValue);

    } else {

      DrawTextEx(Heading1, headerText, HeadingPosition, Heading1Size, 0,
                 textColor);
      DrawTextEx(NormalText, normalText, TextPosition, mainSize, 0, textColor);
    }
    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  UnloadFont(Heading1);
  UnloadFont(NormalText);
  if (musicState != 0) {
    UnloadMusicStream(currentMusic);
    DetachAudioStreamProcessor(currentMusic.stream, callback);
  }
  free(global_frame_buffer);
  CloseAudioDevice();
  UnloadDirectoryFiles(fileList);
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}
