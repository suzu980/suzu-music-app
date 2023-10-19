#include "raylib.h"
#include <stdio.h>
#include <time.h>

void drawFileList(FilePathList *pFileList, Font *pNormalText, int fontSize,
                  Color *textColor);

void CustomLog(int msgType, const char *text, va_list args) {
  char timeStr[64] = {0};
  time_t now = time(NULL);
  struct tm *tm_info = localtime(&now);

  strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", tm_info);
  printf("[%s] ", timeStr);

  switch (msgType) {
  case LOG_INFO:
    printf("[INFO] : ");
    break;
  case LOG_ERROR:
    printf("[ERROR]: ");
    break;
  case LOG_WARNING:
    printf("[WARN] : ");
    break;
  case LOG_DEBUG:
    printf("[DEBUG]: ");
    break;
  default:
    break;
  }

  vprintf(text, args);
  printf("\n");
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
void drawFileList(FilePathList *pFileList, Font *pNormalText, int fontSize,
                  Color *textColor) {
  // draw file list
  int lineSpacing = 25;
  int size = pFileList->count;
  Vector2 positions[size];
  for (int idx = 0; idx < size; ++idx) {
    positions[idx].x = 200;
    positions[idx].y = lineSpacing * idx + 200;
  }
  for (int idx = 0; idx < size; ++idx) {
    DrawTextEx(*pNormalText, GetFileNameWithoutExt(pFileList->paths[idx]), positions[idx], fontSize, 0,
               *textColor); // DrawText(text, xpos, 200, 20, textColor);
  }
}
int main(void) {
  // Initialization
  //--------------------------------------------------------------------------------------
  int screenWidth = 1280;
  int screenHeight = 720;
  const int lineBreakSpacing = 24;
  const int titleOffset = 250;

  Color baseColor = (Color){239, 241, 245, 255};
  Color textColor = (Color){76, 79, 105, 255};

  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(screenWidth, screenHeight, "Music Player and Metadata Editor");

  // Getting filelists
  FilePathList fileList =
      LoadDirectoryFilesEx("resources/sample-music", ".flac;.mp3;.m4a", true);

  const char *headerText = "Suzu Music Player";
  const char *normalText = "Just a normal music player written in C and Raylib library" ;
  // const char *normalText =
  //"This app will be made with Cappuccin theme. https://github.com/suzu980";

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

  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    // Update
    //----------------------------------------------------------------------------------
    // TODO: Update your variables here
    //----------------------------------------------------------------------------------
    SetTextureFilter(Heading1.texture, TEXTURE_FILTER_POINT);
    SetTextureFilter(NormalText.texture, TEXTURE_FILTER_POINT);
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
               textColor); // DrawText(text, xpos, 200, 20, textColor);
    DrawTextEx(NormalText, normalText, TextPosition, mainSize, 0,
               textColor); // DrawText(text, xpos, 200, 20, textColor);
		drawFileList(&fileList, &NormalText, mainSize, &textColor);

    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  UnloadFont(Heading1);
  UnloadFont(NormalText);
  UnloadDirectoryFiles(fileList);
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}
