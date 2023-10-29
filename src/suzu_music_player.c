#include "raylib.h"
#include <complex.h>
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

size_t global_buffer_size = 1024;
size_t global_frames_count = 0;
Frame global_frame_buffer[1024] = {0};
float pi;

float sent_frame_buffer[1024] = {0};
// fft buffers and values
size_t fft_buffer_size = 4410;
size_t fft_frames_count = 0;
float complex oldest_data, newest_data;
float *global_fft_buffer;

void sdft(complex float in[], float *out, int frame_count) {
  //	out[0] = 2048;
}

float complex *coeffs;
void initialize_coeff() {
  for (int i = 0; i < global_buffer_size; ++i) {
    double x = -2 * pi * i / (double)global_buffer_size;
    coeffs[i] = cosf(x) + I * sin(x);
  }
}

///
void callback(void *bufferData, unsigned int frames) {
  // frames come at irregular times, cap it at 1024 then send to sft algorithm
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
		//send over for FFT
		float spectrum[buffer_capacity];
		for(size_t i = 0; i < buffer_capacity; ++i){
			spectrum[i] = (global_frame_buffer[i].left + global_frame_buffer[i].right) / 2.0;
		}
		memcpy(sent_frame_buffer, spectrum, sizeof(spectrum));
    global_frames_count = 0;

  } else {
    printf("WARNING:Buffer OVERLOAD\n");
    memcpy(global_frame_buffer, bufferData, sizeof(Frame) * buffer_capacity);
    global_frames_count = buffer_capacity;
  }
}

// void callback(void *bufferData, unsigned int frames) {
//   size_t buffer_capacity = global_buffer_size;
//   if (frames <=
//       buffer_capacity -
//           global_frames_count) { // if still have space in buffer in frames
//     memcpy(global_frame_buffer + global_frames_count, bufferData,
//            sizeof(Frame) * frames);
//     global_frames_count += frames;
//   } else if (frames <= buffer_capacity) { // if no space in buffer
//     memmove(global_frame_buffer,
//             global_frame_buffer +
//                 (frames - buffer_capacity + global_frames_count),
//             sizeof(Frame) * (buffer_capacity - frames));
//     global_frames_count = buffer_capacity - frames;
//     memcpy(global_frame_buffer + global_frames_count, bufferData,
//            sizeof(Frame) * frames);
//     global_frames_count = buffer_capacity;
//
//   } else {
//     printf("WARNING:Buffer overload\n");
//     memcpy(global_frame_buffer, bufferData, sizeof(Frame) * buffer_capacity);
//     global_frames_count = buffer_capacity;
//		//set old data, new data;
//   }
// }

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
  Music currentMusic =
      LoadMusicStream("resources/sample-music/Someday_Vocal_LB_M.flac");
  AttachAudioStreamProcessor(currentMusic.stream, callback);
  PlayMusicStream(currentMusic);

  //--------------------------------------------------------------------------------------

  //  Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    UpdateMusicStream(currentMusic);

		//do fft here
    BeginDrawing();
    ClearBackground(MOCHABASE);
    DrawFPS(50, 50);
    EndDrawing();

    //----------------------------------------------------------------------------------
  }

  UnloadMusicStream(currentMusic);
  DetachAudioStreamProcessor(currentMusic.stream, callback);
  free(global_fft_buffer);

  CloseAudioDevice();
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}
