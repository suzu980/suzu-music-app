#include "raylib.h"
#include <complex.h>
#include <math.h>
#include <stdbool.h>
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
#define MOCHACRUST																														 \
  (Color) { 17, 17, 17, 255 }
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
const size_t N = 8192; // 4096;

typedef struct {
  float left;
  float right;
} Frame;

size_t global_frames_count = 0;
Frame *global_audio_buffer;
float pi;

float *hannCoeffs;
void initialize_coeff() {
  hannCoeffs = malloc(N * sizeof(*hannCoeffs));
  for (size_t k = 0; k < N; ++k) {
    hannCoeffs[k] = 0.5 - 0.5 * cos(2 * pi * k / (N - 1));
  }
}
void initialize_frame_buffer() {
  global_audio_buffer = malloc(N * sizeof(*global_audio_buffer));
  for (size_t k = 0; k < N; ++k) {
    global_audio_buffer[k].left = 0.0;
    global_audio_buffer[k].right = 0.0;
  }
}
void freeVariables() {
  free(hannCoeffs);
  free(global_audio_buffer);
}

void fft(float x[], size_t n, int s, complex float *out) {
  // Ported from https://github.com/suzu980/suzu-fft
  if (n <= 1) {
    out[0] = x[0];
    return;
  }
  fft(x, n / 2, s * 2, out);
  fft(x + s, n / 2, s * 2, out + n / 2);
  for (size_t k = 0; k < n / 2; ++k) {
    float a = -2 * (pi)*k / n;
    complex float t = out[k + n / 2] * (cos(a) + I * sin(a));
    complex float e = out[k];
    out[k] = e + t;
    out[(n / 2) + k] = e - t;
  }
}

void callback_audio(void *bufferData, unsigned int frames) {
  size_t buffer_capacity = N;
  if (frames <= buffer_capacity - global_frames_count) {
    memcpy(global_audio_buffer + global_frames_count, bufferData,
           sizeof(Frame) * frames);
    global_frames_count += frames;
  } else if (frames <= buffer_capacity) {
    memmove(global_audio_buffer,
            global_audio_buffer +
                (frames - buffer_capacity + global_frames_count),
            sizeof(Frame) * (buffer_capacity - frames));
    global_frames_count = buffer_capacity - frames;
    memcpy(global_audio_buffer + global_frames_count, bufferData,
           sizeof(Frame) * frames);
    global_frames_count = buffer_capacity;
  } else {
    printf("WARNING:Buffer OVERLOAD\n");
    memcpy(global_audio_buffer, bufferData, sizeof(Frame) * buffer_capacity);
    global_frames_count = buffer_capacity;
  }
}

int main(void) {

  // Initialization
  //--------------------------------------------------------------------------------------
  pi = atan2f(1, 1) * 4;
  bool trackLoaded = false;
  char *currentFileName;
  float currentVolume = 0.75;

  SetConfigFlags(FLAG_WINDOW_ALWAYS_RUN);
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(INITIALSCREEN_WIDTH, INITIALSCREEN_HEIGHT, "Music Visualizer");
  InitAudioDevice();
  SetAudioStreamBufferSizeDefault(65536);
  SetTargetFPS(60);
  initialize_coeff();
  initialize_frame_buffer();
  Music currentMusic;

  RenderTexture2D renderTex = LoadRenderTexture(4096, 2);
  RenderTexture2D screenTex = LoadRenderTexture(2560, 1440);
  Shader fftVisualizerShader = LoadShader(0, "resources/shaders/music.fs");
  int xyLoc = GetShaderLocation(fftVisualizerShader, "windowres");
  int texLoc = GetShaderLocation(fftVisualizerShader, "audioFreq");
  float smooth[N / 2];
  for (size_t i = 0; i < N / 2; ++i) {
    smooth[i] = 0.0f;
  }

  //--------------------------------------------------------------------------------------

  //  Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    // UnloadRenderTexture(screenTex);
    // screenTex = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    if (IsFileDropped()) {
      FilePathList droppedFiles = LoadDroppedFiles();
      printf("Dropped File: %s\n", droppedFiles.paths[0]);
      Music tempMusic = LoadMusicStream(droppedFiles.paths[0]);
      if (IsMusicReady(tempMusic)) {
        if (trackLoaded) {
          StopMusicStream(currentMusic);
          DetachAudioStreamProcessor(currentMusic.stream, callback_audio);
          UnloadMusicStream(currentMusic);
          free(currentFileName);
          global_frames_count = 0;
        }
        currentMusic = LoadMusicStream(droppedFiles.paths[0]);
        UnloadMusicStream(tempMusic);
        for (size_t i = 0; i < N; i++) {
          global_audio_buffer[i].left = 0.0;
          global_audio_buffer[i].right = 0.0;
        }
        for (size_t i = 0; i < N / 2; i++) {
          smooth[i] = 0.0;
        }
        AttachAudioStreamProcessor(currentMusic.stream, callback_audio);
        SetMusicVolume(currentMusic, currentVolume);
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

    int w = GetScreenWidth();
    int h = GetScreenHeight();
    float c[2];
    c[0] = w;
    c[1] = h;
    SetShaderValue(fftVisualizerShader, xyLoc, c, SHADER_UNIFORM_VEC2);

    float deltaTime = GetFrameTime();
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
      if (IsKeyReleased(KEY_F5)) {
        StopMusicStream(currentMusic);
        for (size_t i = 0; i < N; i++) {
          global_audio_buffer[i].left = 0.0;
          global_audio_buffer[i].right = 0.0;
        }
        for (size_t i = 0; i < N / 2; i++) {
          smooth[i] = 0.0;
        }

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
      float spectrum[N];
      float spectrum_t[N];
      for (size_t k = 0; k < N; ++k) {
        if (k > global_frames_count) {
          spectrum[k] = 0.0;
          spectrum_t[k] = 0.0;
        } else {
          spectrum[k] =
              (global_audio_buffer[k].left + global_audio_buffer[k].right) *
              0.5;
          spectrum_t[k] =
              (global_audio_buffer[k].left + global_audio_buffer[k].right) *
                  0.125 +
              0.5;
        }
        spectrum[k] = spectrum[k] * hannCoeffs[k];
      }

      complex float fft_out[N];
      // float lowest = 0.0f;
      float peak = 0.0f;
      fft(spectrum, N, 1, fft_out);
      float magnitudes[N / 2];

      /// LOG SCALE
      for (size_t k = 0; k < (N / 2); ++k) {
        float r = crealf(fft_out[k]);
        float i = cimag(fft_out[k]);
        magnitudes[k] = log10f(powf(r, 2) + powf(i, 2));
        if (isnan(magnitudes[k]) || isinf(magnitudes[k])) {
          magnitudes[k] = 0;
        }
        if (peak < magnitudes[k])
          peak = magnitudes[k];
        smooth[k] += (magnitudes[k] - smooth[k]) * 10.0 * deltaTime;
      }

      BeginTextureMode(renderTex);
      ClearBackground(BLACK);
      for (size_t k = 0; k < (N / 2); ++k) {
        int cell_width, new_color;
        new_color = 255 * (smooth[k] / peak);
        if (new_color > 255) {
          new_color = 255;
        }
        if (new_color < 0) {
          new_color = 0;
        }
        Color c = {new_color, 0, 0, 255};
        int x_log = (log10f(k) / log10f(N / 2)) * 4096;
        int x_log_next = (log10f(k + 1) / log10f(N / 2)) * 4096;
        h = 512 * (smooth[k] / peak);
        cell_width = x_log_next - x_log;
        DrawRectangle(x_log, 0, cell_width, 1, c);
        // For Debug
        // DrawRectangle(x_log, 127-h, cell_width, h, c);
      }
      for (size_t k = 0; k < (N); ++k) {
        int new_color;
        new_color = 255 * spectrum_t[k];
        if (new_color > 255) {
          new_color = 255;
        }
        if (new_color < 0) {
          new_color = 0;
        }
        Color c = {0, new_color, 0, 255};
        int xy = ((float)k / (float)N) * 4096;
        DrawRectangle(xy, 1, 1, 1, c);
        // For Debug
        // DrawRectangle(xy, 127, 1, 128, c);
      }
      EndTextureMode();

      BeginTextureMode(screenTex);
      ClearBackground(BLACK);

      EndTextureMode();

      BeginDrawing();
      ClearBackground(MOCHACRUST);
      BeginShaderMode(fftVisualizerShader);
      SetShaderValueTexture(fftVisualizerShader, texLoc, renderTex.texture);
      DrawTexturePro(screenTex.texture,
                     (Rectangle){0, 0, (float)screenTex.texture.width,
                                 (float)-screenTex.texture.height},
                     (Rectangle){0, 150, (float)GetScreenWidth(),
                                 (float)GetScreenHeight() - 150},
                     (Vector2){0, 0}, 0.0, WHITE);
      EndShaderMode();
      // To Check Frame Buffer Tex
      // DrawTextureRec(renderTex.texture,
      //                (Rectangle){0, 0, (float)renderTex.texture.width,
      //                            (float)-renderTex.texture.height},
      //                (Vector2){0, 0}, WHITE);

      //DrawFPS(50, 50);
      EndDrawing();
    } else {
      BeginDrawing();
      ClearBackground(MOCHACRUST);
      DrawText("Drag and Drop a Music file to start", w / 2 - 220, h / 2, 24,
               MOCHATEXT);
      EndDrawing();
    }

    //----------------------------------------------------------------------------------
  }

  if (trackLoaded) {
    StopMusicStream(currentMusic);
    UnloadMusicStream(currentMusic);
    DetachAudioStreamProcessor(currentMusic.stream, callback_audio);
  }
  UnloadRenderTexture(renderTex);
  UnloadRenderTexture(screenTex);
  UnloadShader(fftVisualizerShader);

  CloseAudioDevice();
  freeVariables();
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}
