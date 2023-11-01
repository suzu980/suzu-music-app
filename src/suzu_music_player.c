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

#define INITIALSCREEN_WIDTH 256
#define INITIALSCREEN_HEIGHT 256

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
const size_t N = 2048;

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
  // even
  fft(x, n / 2, s * 2, out);
  // odd
  fft(x + s, n / 2, s * 2, out + n / 2);
  for (size_t k = 0; k < n / 2; ++k) {
    float a = -2 * (pi)*k / n;
    complex float t = out[k + n / 2] * (cos(a) + I * sin(a));
    complex float e = out[k];
    out[k] = e + t;
    out[(n / 2) + k] = e - t;
  }
}

void callbackFFT(void *bufferData, unsigned int frames) {
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
  float twelfth = powf(2.0, 1.0 / 12.0);
  int f = 0;
  int nyquist;
  SetConfigFlags(FLAG_WINDOW_ALWAYS_RUN);
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(INITIALSCREEN_WIDTH, INITIALSCREEN_HEIGHT, "Music Visualizer");
  InitAudioDevice();
  SetAudioStreamBufferSizeDefault(65536);
  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
  initialize_coeff();
  initialize_frame_buffer();
  Music currentMusic = LoadMusicStream("resources/sample-music/MUSIC.mp3");

  nyquist = currentMusic.stream.sampleRate / 2;
  float log_max = log10(nyquist);

  AttachAudioStreamProcessor(currentMusic.stream, callbackFFT);
  PlayMusicStream(currentMusic);
  float interpolated_magnitudes[N / 2];
  RenderTexture2D renderTex = LoadRenderTexture(256, 1);

  //--------------------------------------------------------------------------------------

  //  Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    float deltaTime = GetFrameTime();
    f += 1;
    UpdateMusicStream(currentMusic);

    float spectrum[N];
    for (size_t k = 0; k < N; ++k) {
      if (k > global_frames_count) {
        spectrum[k] = 0.0;
      } else {
        spectrum[k] =
            (global_audio_buffer[k].left + global_audio_buffer[k].right) * 0.5;
      }
      spectrum[k] = spectrum[k] * hannCoeffs[k];
    }

    complex float fft_out[N];
    // float lowest = 0.0f;
    float peak = 0.0f;
    fft(spectrum, N, 1, fft_out);
    float magnitudes[N / 2];
    size_t M = 0;
    //// Fixed Bin Sizes
    // for (float k = 1.0f; k < N / 2; k *= twelfth) {
    //   float k_next = ceil(k * twelfth);
    //   float peak_amp = 0.0f;
    //   float low_amp = 0.0f;
    //   for (size_t f = (size_t)k; f < N / 2 && f < (size_t)k_next; ++f) {
    //     float r = crealf(fft_out[f]);
    //     float i = cimag(fft_out[f]);
    //     float x = 20 * log10f(powf(r, 2) + powf(i, 2));
    //     if (x > peak_amp)
    //       peak_amp = x;
    //   }
    //   if (peak < peak_amp)
    //     peak = peak_amp;
    //   magnitudes[M++] = peak_amp;
    // }
    //// ==================
    /// LOG SCALE
    for (size_t k = 0; k < (N / 2); ++k) {
      float r = crealf(fft_out[k]);
      float i = cimag(fft_out[k]);
      magnitudes[k] = logf(powf(r, 2) + powf(i, 2));
      if (peak < magnitudes[k])
        peak = magnitudes[k];
    }

    // Generate blank image
    // Image glsl_image = GenImageColor(256, 256, BLACK);
    // int img_cells[256] = {0};
    // for (size_t k = 0; k < (N / 2); ++k) {
    //  int cell_width, new_color;
    //  int xpos = k * w / M;
    //  new_color = 255 * (magnitudes[k] / peak);
    //  Color c = {0, 0, new_color, 255};
    //  int x_log = (log10f(k) / log10f(N / 2)) * 256;
    //  cell_width = 1;
    //  if (new_color > img_cells[x_log]) {
    //    //ImageDrawPixel(&glsl_image, x_log, 128, c);
    //    img_cells[x_log] = new_color;
    //  }
    //}
    // Texture2D glsl_tex = LoadTextureFromImage(glsl_image);

    BeginTextureMode(renderTex);
    ClearBackground(BLACK);
    for (size_t k = 0; k < (N / 2); ++k) {
      int cell_width, bar_h, new_color;
      int xpos = k * w / M;
      bar_h = h * (magnitudes[k] / peak);
      new_color = 255 * (magnitudes[k] / peak);
      Color c = {new_color, 0, 0, 255};
      int x_log = (log10f(k) / log10f(N / 2)) * 256;
      int x_log_next = (log10f(k + 1) / log10f(N / 2)) * 256;
      cell_width = x_log_next - x_log;
      // DrawRectangle(xpos, h - bar_h - 50, w / M - 2, bar_h, MOCHABLUE); //
      // LOGARITMIC SCALE BUT SAME BIN SIZE
      DrawRectangle(x_log, 0, cell_width, 1,
                    c); // LOGARITHMIC SCALE
    }
    EndTextureMode();
    BeginDrawing();
    ClearBackground(MOCHABASE);
    // DrawTextureRec(renderTex.texture,
    //                (Rectangle){0, 0, (float)renderTex.texture.width,
    //                            (float)-renderTex.texture.height},
    //                (Vector2){0, 0}, WHITE);
    // for (size_t k = 0; k < (N / 2); ++k) {
    //   int cell_width, bar_h, new_color;
    //   int xpos = k * w / M;
    //   bar_h = h * (magnitudes[k] / peak);
    //   new_color = 255 * (magnitudes[k] / peak);
    //   Color c = {new_color, 0, 0, 255};
    //   int x_log = (log10f(k) / log10f(N / 2)) * w;
    //   int x_log_next = (log10f(k + 1) / log10f(N / 2)) * w;
    //   cell_width = x_log_next - x_log;
    //   // DrawRectangle(xpos, h - bar_h - 50, w / M - 2, bar_h, MOCHABLUE); //
    //   // LOGARITMIC SCALE BUT SAME BIN SIZE
    //   DrawRectangle(x_log, 0, cell_width, h,
    //                 c); // LOGARITHMIC SCALE
    // }
    //  DrawFPS(50, 50);
    EndDrawing();
    // UnloadImage(glsl_image);
    // UnloadTexture(glsl_tex);

    //----------------------------------------------------------------------------------
  }

  UnloadMusicStream(currentMusic);
  DetachAudioStreamProcessor(currentMusic.stream, callbackFFT);

  CloseAudioDevice();
  freeVariables();
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}
