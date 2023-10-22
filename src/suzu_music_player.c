#include "raylib.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define ARRAY_LEN(xs) sizeof(xs) / sizeof(xs[0])

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

typedef struct { 
	float left;
	float right;
} Frame;

Frame *GLOBAL_THING;
Frame global_frame_buffer[882] = {0}; // frame buffer TODO set for other sample rates too
size_t global_frames_count = 0; 

void callback(void *bufferData, unsigned int frames) {
  size_t buffer_capacity = ARRAY_LEN(global_frame_buffer);
  if (frames <= buffer_capacity - global_frames_count) { // if still have space in buffer in frames
    memcpy(global_frame_buffer + global_frames_count, bufferData, sizeof(Frame) * frames);
    global_frames_count += frames;
  } else if (frames <= buffer_capacity) { // if no space in buffer
    memmove(global_frame_buffer, global_frame_buffer + (frames - buffer_capacity + global_frames_count),sizeof(Frame) * (global_frames_count - frames + buffer_capacity - global_frames_count));
		global_frames_count = global_frames_count - frames + buffer_capacity - global_frames_count;
    memcpy(global_frame_buffer +  global_frames_count, bufferData, sizeof(Frame) * frames);
    global_frames_count = buffer_capacity;

  } else { 
		printf("WARNING:Buffer overload\n");
    memcpy(global_frame_buffer, bufferData, sizeof(Frame)*buffer_capacity);
    global_frames_count = buffer_capacity;
  }
  if (frames > ARRAY_LEN(global_frame_buffer)) {
    frames = ARRAY_LEN(global_frame_buffer);
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

  Color baseColor = (Color){30, 30, 46, 255};
  Color textColor = (Color){76, 79, 105, 255};

  Color posColor= (Color){137, 180, 250, 255};
  Color negColor= (Color){243, 139, 168, 255};
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(screenWidth, screenHeight, "Music Visualizer");
  InitAudioDevice();
  SetAudioStreamBufferSizeDefault(32768);
  Music currentMusic =
      LoadMusicStream("resources/sample-music/Feel_Vocal_LB_M.flac");
	size_t sample_rate = currentMusic.stream.sampleRate;
	GLOBAL_THING = malloc(sample_rate * sizeof(*GLOBAL_THING));
	printf("%f\n", GLOBAL_THING->left);

  // Music currentMusic =
   //    LoadMusicStream("resources/sample-music/nights.mp3");
  AttachAudioStreamProcessor(currentMusic.stream, callback);
  unsigned int sampleRate = currentMusic.stream.sampleRate;
  SetMusicVolume(currentMusic, 0.5);
  PlayMusicStream(currentMusic);
  int musicState = 1; // 0: None, 1:Playing, 2: Paused
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
			UpdateMusicStream(currentMusic);
		if (IsKeyReleased(KEY_SPACE)){
			printf("key released\n");
			if (musicState == 2){
			printf("audio resume\n");
				ResumeMusicStream(currentMusic);
				musicState = 1;
			}
			else if (musicState == 1){
			printf("audio pause\n");
			PauseMusicStream(currentMusic);
			musicState = 2;
			}
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
    //    screenWidth = GetScreenWidth();
    //    screenHeight = GetScreenHeight();
    //    Vector2 HeadingPosition = {
    //        screenWidth / 2 - HeadingVec2.x / 2,
    //        screenHeight / 2 - HeadingVec2.y / 2 - lineBreakSpacing -
    //            titleOffset}; // TODO CHANGE THE WAY TEXT IS CENTERED
    //    Vector2 TextPosition = {
    //        screenWidth / 2 - NormalTextVec2.x / 2,
    //        screenHeight / 2 - NormalTextVec2.y / 2 + lineBreakSpacing -
    //            titleOffset}; // TODO CHANGE THE WAY TEXT IS CENTERED
    // Draw
    //----------------------------------------------------------------------------------

    BeginDrawing();

    ClearBackground(baseColor);
    int w = GetRenderWidth();
    int h = GetRenderHeight();
	//	int bar = 200;
  //      DrawRectangle(1, h / 2 , 200, bar, textColor); //NEGATIVE
  //      DrawRectangle(1, h / 2 - 200, 200, bar, RED); //POSITIVE
		int stride = 1;
    for (size_t i = 0; i < global_frames_count; i += stride) {
			float l = global_frame_buffer[i].left;
			float r = global_frame_buffer[i].right;
      float t = (l + r)/2;
			int cell_width = 1;

      if (t > 0) {
       DrawRectangle(i*((float)w/global_frames_count), h / 2 - h / 2 * t +1, 1*cell_width, h / 2 * t, posColor);
      } else {
       DrawRectangle(i*((float)w/global_frames_count), h / 2 -1, 1*cell_width, -(h / 2 * t), negColor);
      }
    }

    //    DrawTextEx(Heading1, headerText, HeadingPosition, Heading1Size, 0,
    //               textColor);
    //    drawFileList(&fileList, &NormalText, mainSize, &textColor,
    //    &mousePoint,
    //                 &currentMusic, &musicState, &musicScrollValue);

    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
	free(GLOBAL_THING);
  UnloadFont(Heading1);
  UnloadFont(NormalText);
  if (musicState != 0)
    UnloadMusicStream(currentMusic);
  DetachAudioStreamProcessor(currentMusic.stream, callback);
  CloseAudioDevice();
  UnloadDirectoryFiles(fileList);
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}
