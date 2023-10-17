#include "raylib.h"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) {
  // Initialization
  //--------------------------------------------------------------------------------------
  const int screenWidth = 800;
  const int screenHeight = 450;

  InitWindow(screenWidth, screenHeight, "Music Player and Metadata Editor");
  const char *text = "This app will be made with Catppuccin theme";

  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
  int textSize = MeasureText(text, 20);
  int xpos =
      screenWidth / 2 - textSize / 2; // MeasureText(text, 20);
  //--------------------------------------------------------------------------------------
	Color baseColor = (Color){239, 241, 245, 255};
	Color textColor = (Color){76, 79, 105, 255};

  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    // Update
    //----------------------------------------------------------------------------------
    // TODO: Update your variables here
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(baseColor);

    DrawText(text, xpos, 200, 20, textColor);

    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}
