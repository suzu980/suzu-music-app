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
#define INITIALSCREEN_HEIGHT 360

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
const size_t N = 8192;

typedef struct {
  float left;
  float right;
} Frame;

size_t global_frames_count = 0;
Frame *global_frame_buffer;
float pi;

float *hannCoeffs;
void initialize_coeff() {
  hannCoeffs = malloc(N * sizeof(*hannCoeffs));
  for (size_t k = 0; k < N; ++k) {
    hannCoeffs[k] = 0.5 - 0.5 * cos(2 * pi * k / (N - 1));
  }
}
void initialize_frame_buffer() {
  global_frame_buffer = malloc(N * sizeof(*global_frame_buffer));
  for (size_t k = 0; k < N; ++k) {
    global_frame_buffer[k].left = 0.0;
    global_frame_buffer[k].right = 0.0;
  }
}
void freeVariables() {
  free(hannCoeffs);
  free(global_frame_buffer);
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
    memcpy(global_frame_buffer + global_frames_count, bufferData,
           sizeof(Frame) * frames);
    global_frames_count += frames;
  } else if (frames <= buffer_capacity) {
    memmove(global_frame_buffer,
            global_frame_buffer +
                (frames - buffer_capacity + global_frames_count),
            sizeof(Frame) * (buffer_capacity - frames));
    global_frames_count = buffer_capacity - frames;
    memcpy(global_frame_buffer + global_frames_count, bufferData,
           sizeof(Frame) * frames);
    global_frames_count = buffer_capacity;
  } else {
    printf("WARNING:Buffer OVERLOAD\n");
    memcpy(global_frame_buffer, bufferData, sizeof(Frame) * buffer_capacity);
    global_frames_count = buffer_capacity;
  }
}

int main(void) {

  // Initialization
  //--------------------------------------------------------------------------------------
  pi = atan2f(1, 1) * 4;
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

  //--------------------------------------------------------------------------------------

  //  Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    f += 1;
    UpdateMusicStream(currentMusic);

    float spectrum[N];
    for (size_t k = 0; k < N; ++k) {
      if (k > global_frames_count) {
        spectrum[k] = 0.0;
      } else {
        spectrum[k] =
            (global_frame_buffer[k].left + global_frame_buffer[k].right) * 0.5;
      }
    }
    for (size_t k = 0; k < N; ++k) {
      spectrum[k] = spectrum[k] * hannCoeffs[k];
    }

    complex float fft_out[N];
    float magnitudes[N / 2];
    float lowest = 0.0;
    float peak = 0.0;
    fft(spectrum, N, 1, fft_out);
    for (size_t k = 0; k < (N / 2); ++k) {
      float r = crealf(fft_out[k]);
      float i = cimag(fft_out[k]);
      magnitudes[k] = logf(powf(r, 2) + powf(i, 2));
      if (peak < magnitudes[k])
        peak = magnitudes[k];
      if (lowest > magnitudes[k])
        lowest = magnitudes[k];
    }
		//peak = peak - lowest;
    for (size_t k = 0; k < (N / 2); ++k) {
			//magnitudes[k] += -lowest;
		}

    BeginDrawing();
    ClearBackground(MOCHABASE);
    // Fast Fourier Transform
    int bar_idx = 0;
    for (size_t k = 0; k < (N / 2); ++k) {
      float log, logNext;
      int logPos, logNextPos, cell_width, bar_h, bar_h_next;
      log = log10((float)(k)*nyquist / (float)(N / 2));
      logNext = log10((float)(k + 1) * nyquist / (float)(N / 2));
      logPos = ceilf(log / log_max * w);
      logNextPos = ceilf(logNext / log_max * w);
      cell_width = logNextPos - logPos;
      if (k == 0) {
        logPos = 0;
        cell_width = logNextPos;
      }
      bar_h = h * (magnitudes[k] / peak) - 200;
      bar_h_next = h * (magnitudes[k + 1] / peak);
      DrawRectangle(logPos, h - bar_h - 100, 1, bar_h, MOCHABLUE);
      bar_idx += 1;
    }
    DrawFPS(50, 50);
    EndDrawing();

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
