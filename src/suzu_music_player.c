#include "raylib.h"
#include <stdio.h>
#include <time.h>

void drawFileList(FilePathList *pFileList, Font *pNormalText, int fontSize,
                  Color *textColor, Vector2 *mousePos, Music *currentMusic,
                  int *musicState, int *musicScrollValue);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
void drawFileList(FilePathList *pFileList, Font *pNormalText, int fontSize,
                  Color *textColor, Vector2 *mousePos, Music *currentMusic,
                  int *musicState, int *musicScrollValue) {
  int initialx = 500;
  int initialy = 150;
  int lineSpacing = 25;
  int boxWidth = 500;
  int boxHeight = 50;
  int pl = 10;
  int boxSpacing = 5;

  int size = pFileList->count;
  Vector2 positions[size];
  Rectangle buttonBounds[size];
  Vector2 textSize[size];
  // TODO : DO NOT RENDER WHEN OFF SCREEN
  for (int idx = 0; idx < size; ++idx) {
    bool buttonAction = false;
    positions[idx].x = initialx + pl;
    positions[idx].y =
        boxHeight * idx + initialy + (float)boxHeight / 2 -
        MeasureTextEx(*pNormalText,
                      GetFileNameWithoutExt(pFileList->paths[idx]), fontSize, 0)
                .y /
            2 +
        boxSpacing * idx + *musicScrollValue;

    buttonBounds[idx].x = initialx;
    buttonBounds[idx].y =
        initialy + boxHeight * idx + boxSpacing * idx + *musicScrollValue;
    buttonBounds[idx].width = boxWidth;
    buttonBounds[idx].height = boxHeight;
    Color colorToUse = GRAY;
    if (CheckCollisionPointRec(*mousePos, buttonBounds[idx])) {
      colorToUse = WHITE;
      if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        buttonAction = true;
      }
    }
    DrawRectangle(initialx,
                  initialy + boxHeight * idx + boxSpacing * idx +
                      *musicScrollValue,
                  boxWidth, boxHeight, colorToUse);
    DrawTextEx(*pNormalText, GetFileNameWithoutExt(pFileList->paths[idx]),
               positions[idx], fontSize, 0, *textColor);
    if (buttonAction) {
      if (*musicState != 0) {
        TraceLog(LOG_INFO, "Music Unloaded");
        UnloadMusicStream(*currentMusic);
      }
      *currentMusic = LoadMusicStream(pFileList->paths[idx]);
      PlayMusicStream(*currentMusic);
      *musicState = 1;
    }
  }
}
int main(void) {
  // Initialization
  //--------------------------------------------------------------------------------------
  int screenWidth = 1280;
  int screenHeight = 720;
  int musicScrollValue = 0;

  const int lineBreakSpacing = 25;
  const int titleOffset = 250;

  Color baseColor = (Color){239, 241, 245, 255};
  Color textColor = (Color){76, 79, 105, 255};

  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(screenWidth, screenHeight, "Music Player and Metadata Editor");
  InitAudioDevice();
  SetAudioStreamBufferSizeDefault(32768);
  Music currentMusic;
  int musicState = 0; // 0: None, 1:Playing, 2: Paused
  // Getting filelists
  FilePathList fileList =
      LoadDirectoryFilesEx("resources/sample-music", ".flac;.mp3;.m4a", true);

  const char *headerText = "Suzu Music Player";
  const char *normalText = "Just a normal music player";

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

  SetTextureFilter(Heading1.texture, TEXTURE_FILTER_POINT);
  SetTextureFilter(NormalText.texture, TEXTURE_FILTER_POINT);
  Vector2 mousePoint = {0.0f, 0.0f};
  // PlayMusicStream(currentMusic);
  //  Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    // Update
    //----------------------------------------------------------------------------------
    // TODO: Update your variables here
    //----------------------------------------------------------------------------------
    if (musicState == 1) {
      UpdateMusicStream(currentMusic);
    }
    mousePoint = GetMousePosition();
    int mouseTranslation = GetMouseWheelMove() * 16;
    if ((musicScrollValue + mouseTranslation) != 0) {

      musicScrollValue += mouseTranslation;
    }

    screenWidth = GetScreenWidth();
    screenHeight = GetScreenHeight();
    Vector2 HeadingPosition = {
        screenWidth / 2 - HeadingVec2.x / 2,
        screenHeight / 2 - HeadingVec2.y / 2 - lineBreakSpacing -
            titleOffset}; // TODO CHANGE THE WAY TEXT IS CENTERED
    Vector2 TextPosition = {
        screenWidth / 2 - NormalTextVec2.x / 2,
        screenHeight / 2 - NormalTextVec2.y / 2 + lineBreakSpacing -
            titleOffset}; // TODO CHANGE THE WAY TEXT IS CENTERED
    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(baseColor);

    DrawTextEx(Heading1, headerText, HeadingPosition, Heading1Size, 0,
               textColor);
    drawFileList(&fileList, &NormalText, mainSize, &textColor, &mousePoint,
                 &currentMusic, &musicState, &musicScrollValue);

    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  UnloadFont(Heading1);
  UnloadFont(NormalText);
  if (musicState != 0)
    UnloadMusicStream(currentMusic);
  CloseAudioDevice();
  UnloadDirectoryFiles(fileList);
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}
