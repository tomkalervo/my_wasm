#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include "SDL_error.h"
#include "SDL_events.h"
#include "SDL_mouse.h"
#include "SDL_pixels.h"
#include "SDL_render.h"
#include "SDL_stdinc.h"
#include "SDL_surface.h"
#include "SDL_timer.h"
#include "SDL_video.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <climits>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <list>
#include <random>
#include <stdio.h>

// Constants
int HEIGHT = 480;
int WIDTH = 400;

// Container for Pi Approximation
struct MonteCarlo {
  long n = 0;
  long p_count = 0;
  std::mt19937 m_generator{std::random_device{}()};
  std::uniform_real_distribution<double> m_dist{0.0F, 1.0F};
  std::pair<double, double> randXY() {
    return std::pair<double, double>(
        {m_dist(m_generator), m_dist(m_generator)});
  };
} MonteCarlo;

// Container for SDL assets
struct Assets {
  SDL_Window *window;
  SDL_Surface *surface;
  SDL_Surface *surface_bg;
  SDL_Renderer *renderer;
  SDL_Texture *screenTexture;
  SDL_Texture *backgroundTexture;
  SDL_Rect dstrect = {0, 0, 400, 400};
  SDL_Rect dstrect_bar = {410, 0, 100, 400};
  SDL_Color textColor = {0, 0, 0, 255};
  SDL_Colour red = {255, 0, 0, 255};
  SDL_Colour blue = {0, 0, 255, 255};
  Uint64 runtime = 0;
  uint64_t prev_len = 0;
  const double time_delta = 0.01F;
  double time = 0.0F;
  double time_acc = 0;
  TTF_Font *font;
  TTF_Font *font_large;
  bool quit = false;
  bool pause = false;
  long random_pixels = 1;
} Assets;

/* getRandomPixels(const long &num_samples) -> std::list<std::pair<uint32_t,
 * bool>> Returns a list of pairs <pixel-index, true/false> where true/false
 * reflects the pixels position inside the unit circle. Also updates the
 * MonteCarlo values for Approximation.
 */
static std::list<std::pair<uint32_t, bool>>
getRandomPixels(const long &num_samples) {
  std::list<std::pair<uint32_t, bool>> pixel_list;
  if (!Assets.surface)
    return pixel_list;
  if (num_samples < 1) {
    return pixel_list;
  }
  const uint32_t w = Assets.surface->w;
  const uint32_t h = Assets.surface->h;
  const uint32_t pitch = Assets.surface->pitch;
  const uint32_t bytes = Assets.surface->format->BytesPerPixel;
  int i = 0;
  while (i++ < num_samples) {
    auto [x, y] = MonteCarlo.randXY();
    // Convert [0.0,1.0] to [-1.0,1.0]
    x = 2 * x - 1.0F;
    y = 2 * y - 1.0F;
    bool inside = false;
    if (x * x + y * y <= 1.0F) {
      MonteCarlo.p_count++;
      inside = true;
    }
    uint32_t gridX = (uint32_t)(((x + 1.0F) / 2.0F) * w);
    uint32_t gridY = (uint32_t)(((y + 1.0F) / 2.0F) * h);
    gridX = std::clamp(gridX, static_cast<uint32_t>(0), w - 1);
    gridY = std::clamp(gridY, static_cast<uint32_t>(0), h - 1);
    uint32_t pixel_index = (gridY * pitch) + gridX * bytes;
    pixel_list.push_back({pixel_index, inside});
  }
  MonteCarlo.n += num_samples;
  return pixel_list;
}

/*
 * Takes a list of pairs (pixel-index, true/false), color for inside and color
 * for outside. If pixel is true, color for inside is used. If pixer is false,
 * color for outside is used.
 */
static void drawPixels(const std::list<std::pair<uint32_t, bool>> &ps,
                       const SDL_Colour &in, const SDL_Colour &out) {
  //
  if (SDL_MUSTLOCK(Assets.surface)) {
    SDL_LockSurface(Assets.surface);
  }
  Uint8 *pixels = static_cast<Uint8 *>(Assets.surface->pixels);
  for (auto [p, inside] : ps) {
    if (inside) {
      pixels[p] = in.r;
      pixels[p + 1] = in.g;
      pixels[p + 2] = in.b;
      pixels[p + 3] = in.a;
    } else {
      pixels[p] = out.r;
      pixels[p + 1] = out.g;
      pixels[p + 2] = out.b;
      pixels[p + 3] = out.a;
    }
  }
  if (SDL_MUSTLOCK(Assets.surface)) {
    SDL_UnlockSurface(Assets.surface);
  }
  if (Assets.screenTexture)
    SDL_DestroyTexture(Assets.screenTexture);
  Assets.screenTexture =
      SDL_CreateTextureFromSurface(Assets.renderer, Assets.surface);
  if (!Assets.screenTexture) {
    printf("Error creating screenTexture: %s\n", SDL_GetError());
    exit(-1);
  }
}

// Render text related to the Approximation of Pi
static void drawText() {
  long double pi = (4L * (long double)MonteCarlo.p_count) / MonteCarlo.n;
  char txt[100];
  std::snprintf(txt, sizeof(txt), "Approximation of Pi: %.10Lf", pi);

  SDL_Surface *textSurface =
      TTF_RenderText_Solid(Assets.font, txt, Assets.textColor);
  SDL_Texture *textTexture =
      SDL_CreateTextureFromSurface(Assets.renderer, textSurface);
  SDL_Rect textRect = {10, 410, textSurface->w,
                       textSurface->h}; // Position to the right
  SDL_RenderCopy(Assets.renderer, textTexture, NULL, &textRect);
  SDL_FreeSurface(textSurface);
  SDL_DestroyTexture(textTexture);

  std::snprintf(txt, sizeof(txt), "Random pixels: %ld", MonteCarlo.n);
  textSurface = TTF_RenderText_Solid(Assets.font, txt, Assets.textColor);
  textTexture = SDL_CreateTextureFromSurface(Assets.renderer, textSurface);
  textRect = {10, 430, textSurface->w, textSurface->h}; // Position to the right
  SDL_RenderCopy(Assets.renderer, textTexture, NULL, &textRect);
  SDL_FreeSurface(textSurface);
  SDL_DestroyTexture(textTexture);

  std::snprintf(txt, sizeof(txt), "Random pixels with radii <= 1: %ld",
                MonteCarlo.p_count);
  textSurface = TTF_RenderText_Solid(Assets.font, txt, Assets.textColor);
  textTexture = SDL_CreateTextureFromSurface(Assets.renderer, textSurface);
  textRect = {10, 450, textSurface->w, textSurface->h}; // Position to the right
  SDL_RenderCopy(Assets.renderer, textTexture, NULL, &textRect);

  // Clean up text surfaces
  SDL_FreeSurface(textSurface);
  SDL_DestroyTexture(textTexture);
}

// Render text during the pause event
static void drawPauseText() {
  char txt[100];
  std::snprintf(txt, sizeof(txt), "Paused. Right click to reset.");
  SDL_Surface *ts =
      TTF_RenderText_Solid(Assets.font_large, txt, Assets.textColor);
  SDL_Texture *tt = SDL_CreateTextureFromSurface(Assets.renderer, ts);
  SDL_Rect tr = {20, 200, ts->w, ts->h};
  SDL_RenderCopy(Assets.renderer, tt, NULL, &tr);
  SDL_FreeSurface(ts);
  SDL_DestroyTexture(tt);
}

// Initialize SDL, TTF
static void initSDL() {
  Assets.window = nullptr;
  Assets.renderer = nullptr;
  Assets.surface = nullptr;
  Assets.surface_bg = nullptr;
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("Could not initialize! Error: %s\n", SDL_GetError());
    exit(-1);
  }
  Assets.window = SDL_CreateWindow(
      "Monte Carlo pi aprox", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
  if (!Assets.window) {
    printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
  }
  Assets.renderer =
      SDL_CreateRenderer(Assets.window, -1, SDL_RENDERER_ACCELERATED);
  if (!Assets.renderer) {
    printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
  }
  Assets.surface = SDL_CreateRGBSurface(0, 400, 400, 32, 0, 0, 0, 0);
  if (!Assets.surface) {
    printf("surface could not be created! SDL Error: %s\n", SDL_GetError());
  }
  // Fill the surface with white
  Uint32 white = SDL_MapRGB(Assets.surface->format, 255, 255, 255);
  SDL_FillRect(Assets.surface, NULL, white);

  Assets.surface_bg = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32, 0, 0, 0, 0);
  if (!Assets.surface_bg) {
    printf("surface bg could not be created! SDL Error: %s\n", SDL_GetError());
  }

  if (TTF_Init() < 0) {
    printf("TTF_Init() could not be initialized! SDL Error: %s\n",
           TTF_GetError());
    exit(-1);
  }
  Assets.font = TTF_OpenFont("fonts/amoitar.ttf", 16);
  if (!Assets.font) {
    printf("TTF_OpenFont coud not be created: %s\n", TTF_GetError());
    exit(-1);
  }
  Assets.font_large = TTF_OpenFont("fonts/amoitar.ttf", 22);
  if (!Assets.font_large) {
    printf("TTF_OpenFont coud not be created: %s\n", TTF_GetError());
    exit(-1);
  }

  // Init timing
  Assets.runtime = SDL_GetTicks64();
}

// Clean up SDL,TTF
static void exitSDL() {
  TTF_CloseFont(Assets.font);
  TTF_CloseFont(Assets.font_large);
  TTF_Quit();
  SDL_FreeSurface(Assets.surface_bg);
  SDL_FreeSurface(Assets.surface);
  if (Assets.backgroundTexture)
    SDL_DestroyTexture(Assets.backgroundTexture);
  if (Assets.screenTexture)
    SDL_DestroyTexture(Assets.screenTexture);
  SDL_DestroyRenderer(Assets.renderer);
  SDL_DestroyWindow(Assets.window);
  SDL_Quit();
}

static void reset_loop() {
  MonteCarlo.n = 0;
  MonteCarlo.p_count = 0;
  Assets.random_pixels = 1;
  Uint32 white = SDL_MapRGB(Assets.surface->format, 255, 255, 255);
  SDL_LockSurface(Assets.surface);
  SDL_FillRect(Assets.surface, NULL, white);
  SDL_UnlockSurface(Assets.surface);
  SDL_LockSurface(Assets.surface_bg);
  SDL_FillRect(Assets.surface_bg, NULL, white);
  SDL_UnlockSurface(Assets.surface_bg);
  if (Assets.screenTexture)
    SDL_DestroyTexture(Assets.screenTexture);

  Assets.screenTexture =
      SDL_CreateTextureFromSurface(Assets.renderer, Assets.surface_bg);

  SDL_RenderCopy(Assets.renderer, Assets.screenTexture, NULL, NULL);
  drawText();
  SDL_SetRenderDrawColor(Assets.renderer, 0, 0, 0, 0xFF);
  SDL_RenderDrawRect(Assets.renderer, &Assets.dstrect);
  SDL_RenderReadPixels(Assets.renderer, NULL, SDL_PIXELFORMAT_BGRA32,
                       Assets.surface_bg->pixels, Assets.surface_bg->pitch);

  if (Assets.backgroundTexture) {
    SDL_DestroyTexture(Assets.backgroundTexture);
    Assets.backgroundTexture = nullptr;
  }
  if (Assets.surface_bg == nullptr) {
    printf("Invalid surface_bg\n");
    exit(-1);
  }
  Assets.backgroundTexture =
      SDL_CreateTextureFromSurface(Assets.renderer, Assets.surface_bg);

  Assets.pause = !Assets.pause;
}

static void render_out() {
  if (MonteCarlo.n >= LONG_MAX) {
    reset_loop();
  }
  // every loop does >= 1 render of pixels
  SDL_SetRenderDrawColor(Assets.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  SDL_RenderClear(Assets.renderer);

  // Timing settings
  Uint64 new_runtime = SDL_GetTicks64();
  double frametime = (new_runtime - Assets.runtime) / 6000.0F;
  Assets.runtime = new_runtime;
  Assets.time_acc += frametime;
  while (Assets.time_acc >= Assets.time_delta) {
    Assets.time_acc -= Assets.time_delta;
    if (Assets.random_pixels > Assets.prev_len &&
        Assets.random_pixels <= UINT64_MAX >> 1) {
      Assets.prev_len = Assets.random_pixels;
      Assets.random_pixels += log(Assets.random_pixels + 3);
    }

    // Shift colors to make the render more visual
    if (MonteCarlo.n % 17 == 0) {
      Assets.blue.g =
          Assets.blue.g + 25 > 155 ? Assets.blue.g = 0 : Assets.blue.g + 25;
    }
    if (MonteCarlo.n % 13 == 0) {
      Assets.red.g =
          Assets.red.g - 25 < 0 ? Assets.red.g = 155 : Assets.red.g - 25;
    }
    Assets.blue.b =
        Assets.blue.b + 1 > 255 ? Assets.blue.b = 200 : Assets.blue.b + 1;
    Assets.red.r =
        Assets.red.r + 1 > 255 ? Assets.red.r = 200 : Assets.red.r + 1;
    // randomize new pixels
    auto ps = getRandomPixels(Assets.random_pixels);
    // Render calls
    drawPixels(ps, Assets.blue, Assets.red);
  }
  // render pixel-surface and text-surface
  SDL_RenderCopy(Assets.renderer, Assets.screenTexture, NULL, &Assets.dstrect);
  drawText();
  SDL_SetRenderDrawColor(Assets.renderer, 0, 0, 0, 0xFF);
  SDL_RenderDrawRect(Assets.renderer, &Assets.dstrect);
}

static void pause_loop() {
  Assets.pause = !Assets.pause;
  if (Assets.pause) {
    render_out();
    if (!Assets.surface_bg) {
      printf("No surface_bg error: %s\n", SDL_GetError());
      exit(-1);
    }
    SDL_LockSurface(Assets.surface_bg);
    SDL_RenderReadPixels(Assets.renderer, NULL, SDL_PIXELFORMAT_BGRA32,
                         Assets.surface_bg->pixels, Assets.surface_bg->pitch);

    SDL_UnlockSurface(Assets.surface_bg);
    if (Assets.backgroundTexture) {
      SDL_DestroyTexture(Assets.backgroundTexture);
      Assets.backgroundTexture = nullptr;
    }
    Assets.backgroundTexture =
        SDL_CreateTextureFromSurface(Assets.renderer, Assets.surface_bg);

    // Render it
    SDL_SetRenderDrawColor(Assets.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(Assets.renderer);
    SDL_RenderCopy(Assets.renderer, Assets.backgroundTexture, NULL, NULL);
    drawPauseText();
    SDL_RenderPresent(Assets.renderer);
  }
}

// The loop logic
static void SDL_Loop() {
  SDL_Event e;
  // SDL_Rect rect = {40, 40, 400, 400};

  // Handle interaction
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      Assets.quit = true;
      break;
    }
    if (e.type == SDL_MOUSEBUTTONDOWN) {
      if (e.button.button == SDL_BUTTON_LEFT) {
        pause_loop();
      } else if (e.button.button == SDL_BUTTON_RIGHT && Assets.pause) {
        reset_loop();
      }
    }
  }
  if (!Assets.pause) {
    render_out();
    SDL_RenderPresent(Assets.renderer);
  }
}
int main(int argc, char *argv[]) {
  initSDL();
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(SDL_Loop, 0, 1);
#else
  while (Assets.quit == false) {
    SDL_Loop();
  }
#endif
  exitSDL();
}
