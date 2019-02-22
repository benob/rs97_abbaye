#include "SDL2_compat.h"

SDL_Renderer* SDL_CreateRenderer(SDL_Window * window, int index, Uint32 flags) {
  SDL_Renderer* renderer = calloc(sizeof(SDL_Renderer), 1);
  renderer->window = window;
  renderer->color = SDL_MapRGB(window->screen->format, 0, 0, 0);
  renderer->last_frame_ticks = 0;
  renderer->scale_mode = 0;
  SDL_RenderSetLogicalSize(renderer, window->w, window->h);
  return renderer;
}

SDL_Texture * SDL_CreateTextureFromSurface(SDL_Renderer * renderer, SDL_Surface * surface) {
  SDL_Texture* texture = calloc(sizeof(SDL_Texture), 1);
  texture->surface = SDL_DisplayFormatAlpha(surface);
  return texture;
}

void SDL_DestroyTexture(SDL_Texture * texture) {
  SDL_FreeSurface(texture->surface);
  free(texture);
}

void SDL_DestroyRenderer(SDL_Renderer * renderer) {
  SDL_FreeSurface(renderer->surface);
  free(renderer);
}

int SDL_RenderClear(SDL_Renderer * renderer) {
  return SDL_FillRect(renderer->surface, NULL, renderer->color);
}

int SDL_RenderCopy(SDL_Renderer * renderer, SDL_Texture * texture, const SDL_Rect * srcrect, const SDL_Rect * dstrect) {
  return SDL_BlitSurface(texture->surface, (SDL_Rect*) srcrect, renderer->surface, (SDL_Rect*) dstrect);
}

#define map_rgb(format, r, g, b) \
  (r >> format->Rloss) << format->Rshift \
  | (g >> format->Gloss) << format->Gshift \
  | (b >> format->Bloss) << format->Bshift \
  | format->Amask

#define get_rgba(pixel, fmt, r, g, b, a) { \
  Uint32 v; \
  v = (pixel & fmt->Rmask) >> fmt->Rshift; \
  *r = (v << fmt->Rloss) + (v >> (8 - (fmt->Rloss << 1))); \
  v = (pixel & fmt->Gmask) >> fmt->Gshift; \
  *g = (v << fmt->Gloss) + (v >> (8 - (fmt->Gloss << 1))); \
  v = (pixel & fmt->Bmask) >> fmt->Bshift; \
  *b = (v << fmt->Bloss) + (v >> (8 - (fmt->Bloss << 1))); \
  if(fmt->Amask) { \
    v = (pixel & fmt->Amask) >> fmt->Ashift; \
    *a = (v << fmt->Aloss) + (v >> (8 - (fmt->Aloss << 1))); \
  } else { \
    *a = SDL_ALPHA_OPAQUE; \
  } \
}

#define get_rgb(pixel, fmt, r, g, b) { \
  Uint32 v; \
  v = (pixel & fmt->Rmask) >> fmt->Rshift; \
  *r = (v << fmt->Rloss) + (v >> (8 - (fmt->Rloss << 1))); \
  v = (pixel & fmt->Gmask) >> fmt->Gshift; \
  *g = (v << fmt->Gloss) + (v >> (8 - (fmt->Gloss << 1))); \
  v = (pixel & fmt->Bmask) >> fmt->Bshift; \
  *b = (v << fmt->Bloss) + (v >> (8 - (fmt->Bloss << 1))); \
}

static void scaled_blit_alpha(SDL_Surface* source, const SDL_Rect* srect, SDL_Surface* dest, const SDL_Rect* drect, int flip_x, int flip_y) {
  int sx = srect->x, sy = srect->y, sw = srect->w, sh = srect->h;
  int dx = drect->x, dy = drect->y, dw = drect->w, dh = drect->h;
  Uint8* spixels = (Uint8*) source->pixels;
  Uint8* dpixels = (Uint8*) dest->pixels;
  int sbpp = source->format->BytesPerPixel;
  int dbpp = dest->format->BytesPerPixel;
  //printf("alpha=%d haskey=%d colorkey=%08x Amask=%08x\n", source->flags & SDL_SRCALPHA, source->flags & SDL_SRCCOLORKEY, source->format->colorkey, source->format->Amask);
  for(int j = 0; j < dh; j++) {
    for(int i = 0; i < dw; i++) {
      int x = flip_x ? sx + sw - (i * sw) / dw : sx + (i * sw) / dw;
      int y = flip_y ? sy + sh - (j * sh) / dh : sy + (j * sh) / dh;

      if(x >= 0 && x < source->w && y >= 0 && y < source->h && dx + i >= 0 && dx + i < dest->w && dy + j >= 0 && dy + j < dest->h) { 
        if(source->flags & SDL_SRCALPHA) {
          if(source->format->Amask != 0 || !(source->flags & SDL_SRCCOLORKEY)) {
            Uint32 spixel = 0;
            memcpy(&spixel, spixels + y * source->pitch + x * sbpp, sbpp);

            Uint8 r1, g1, b1, a1;
            get_rgba(spixel, source->format, &r1, &g1, &b1, &a1);

            Uint32 dpixel = 0;
            memcpy(&dpixel, dpixels + (dy + j) * dest->pitch + (dx + i) * dbpp, dbpp);

            Uint8 r2, g2, b2;
            get_rgb(dpixel, dest->format, &r2, &g2, &b2);

            Uint32 color = map_rgb(dest->format, (r1 * a1 + r2 * (255 - a1)) >> 8, (g1 * a1 + g2 * (255 - a1)) >> 8, (b1 * a1 + b2 * (255 - a1)) >> 8);

            memcpy(dpixels + (dy + j) * dest->pitch + (dx + i) * dbpp, &color, dbpp);
          } else {
            Uint32 spixel = 0;
            memcpy(&spixel, spixels + y * source->pitch + x * sbpp, sbpp);

            if(spixel != source->format->colorkey) {
              Uint8 r1, g1, b1, a1;
              get_rgba(spixel, source->format, &r1, &g1, &b1, &a1);

              Uint32 dpixel = 0;
              memcpy(&dpixel, dpixels + (dy + j) * dest->pitch + (dx + i) * dbpp, dbpp);

              Uint8 r2, g2, b2;
              get_rgb(dpixel, dest->format, &r2, &g2, &b2);

              Uint32 color = map_rgb(dest->format, (r1 * a1 + r2 * (255 - a1)) >> 8, (g1 * a1 + g2 * (255 - a1)) >> 8, (b1 * a1 + b2 * (255 - a1)) >> 8);

              memcpy(dpixels + (dy + j) * dest->pitch + (dx + i) * dbpp, &color, dbpp);
            }
          }
        } else if(source->flags & SDL_SRCCOLORKEY) {
          if(sbpp == dbpp) {
            Uint32 spixel = 0;
            memcpy(&spixel, spixels + y * source->pitch + x * sbpp, sbpp);
            if(spixel != source->format->colorkey) {
              memcpy(dpixels + (dy + j) * dest->pitch + (dx + i) * dbpp, &spixel, sbpp);
            }
          } else {
            Uint32 spixel = 0;
            memcpy(&spixel, spixels + y * source->pitch + x * sbpp, sbpp);
            if(spixel != source->format->colorkey) {
              Uint8 r1, g1, b1;
              get_rgb(spixel, source->format, &r1, &g1, &b1);

              Uint32 color = map_rgb(dest->format, r1, g1, b1);

              memcpy(dpixels + (dy + j) * dest->pitch + (dx + i) * dbpp, &color, dbpp);
            }
          }
        } else {
          if(sbpp == dbpp) {
            memcpy(dpixels + (dy + j) * dest->pitch + (dx + i) * dbpp, spixels + y * source->pitch + x * sbpp, sbpp);
          } else {
            Uint32 spixel = 0;
            memcpy(&spixel, spixels + y * source->pitch + x * sbpp, sbpp);

            Uint8 r1, g1, b1;
            get_rgb(spixel, source->format, &r1, &g1, &b1);

            Uint32 color = map_rgb(dest->format, r1, g1, b1);

            memcpy(dpixels + (dy + j) * dest->pitch + (dx + i) * dbpp, &color, dbpp);
          }
        }
      }
    }
  }
}

int SDL_RenderCopyEx(SDL_Renderer * renderer, SDL_Texture * texture, const SDL_Rect * srcrect, const SDL_Rect * dstrect, const double angle, const SDL_Point *center, const SDL_RendererFlip flip) {
  if(angle != 0 || center != NULL) {
    fprintf(stderr, "WARNING: rotated blit not supported\n");
    return -1;
  }
  if (flip == 0 && srcrect != NULL && dstrect != NULL && srcrect->w == dstrect->w && srcrect->h == dstrect->h) return SDL_BlitSurface(texture->surface, (SDL_Rect*) srcrect, renderer->surface, (SDL_Rect*) dstrect);
  scaled_blit_alpha(texture->surface, srcrect, renderer->surface, dstrect, flip & SDL_FLIP_HORIZONTAL, flip & SDL_FLIP_VERTICAL);
  return 0;
}

void fast_scale_up(SDL_Surface* source, SDL_Surface* dest) {
  if(dest->w < source->w) {
    fprintf(stderr, "ERROR: fast_scale_up expected dest->w > source->w\n");
    return;
  }
  if(dest->format->BytesPerPixel != 2 || source->format->BytesPerPixel != 2) {
    fprintf(stderr, "ERROR: fast_scale_up only supports surfaces with 2 bytes per pixel\n");
    return;
  }
  Uint16* spixels = (Uint16*) source->pixels;
  Uint16* dpixels = (Uint16*) dest->pixels;
  int x_step = (source->w << 16) / dest->w;
  int y_step = (source->h << 16) / dest->h;
  for(int j = 0, y = 0; j < dest->h; j++, y += y_step) {
    if((y - y_step) >> 16 != y >> 16) {
      for(int i = 0, x = 0; i < dest->w; i++, x += x_step) {
        dpixels[j * dest->w + i] = spixels[(y >> 16) * source->w + (x >> 16)];
      }
    } else {
      memcpy(dpixels + j * dest->w, dpixels + (j - 1) * dest->w, dest->w * 2);
    }
  }
}

void SDL_RenderPresent(SDL_Renderer * renderer) {
  SDL_Rect srect = {0, 0, renderer->w, renderer->h};
  if(renderer->scale_mode == 1) {
    SDL_Rect drect = {0, 0, renderer->window->w, renderer->window->h};
    fast_scale_up(renderer->surface, renderer->window->screen);
  } else {
    SDL_Rect drect = {(renderer->window->w - renderer->w) / 2, (renderer->window->h - renderer->h) / 2, renderer->w, renderer->h};
    SDL_FillRect(renderer->window->screen, NULL, SDL_MapRGB(renderer->window->screen->format, 0, 0, 0));
    SDL_BlitSurface(renderer->surface, &srect, renderer->window->screen, &drect);
  }
  SDL_Flip(renderer->window->screen);
  Uint32 ticks = SDL_GetTicks();
  if(ticks - renderer->last_frame_ticks < 1000 / 60) {
    SDL_Delay(1000 / 60 - (ticks - renderer->last_frame_ticks));
  }
  renderer->last_frame_ticks = ticks;
}

int SDL_RenderSetLogicalSize(SDL_Renderer * renderer, int w, int h) {
  SDL_PixelFormat* format = renderer->window->screen->format;
  renderer->surface = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, format->BitsPerPixel, format->Rmask, format->Gmask, format->Bmask, format->Amask);
  renderer->w = w;
  renderer->h = h;
  return 0;
}

int SDL_SetRenderDrawColor(SDL_Renderer * renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
  renderer->color = SDL_MapRGBA(renderer->window->screen->format, r, g, b, a);
  return 0;
}

SDL_Window * SDL_CreateWindow(const char *title, int x, int y, int w, int h, Uint32 flags) {
  static int already_has_window = 0;
  if(already_has_window) {
    fprintf(stderr, "ERROR: cannot create multiple windows\n");
    return NULL;
  }
  already_has_window = 1;
  if(flags & SDL_WINDOW_OPENGL) {
    fprintf(stderr, "WARNING: flag SDL_WINDOW_OPENGL not supported\n");
    flags &= ~SDL_WINDOW_OPENGL;
  }
  SDL_Window* window = calloc(sizeof(SDL_Window), 1);
  window->screen = SDL_SetVideoMode(w, h, DEFAULT_BPP, flags | SDL_HWSURFACE | SDL_DOUBLEBUF);
  window->w = w;
  window->h = h;
  window->flags = flags;
  SDL_WM_SetCaption(title, NULL);
  return window;
}

void SDL_DestroyWindow(SDL_Window * window) {
  free(window);
}

SDL_bool SDL_SetHint(const char *name, const char *value) {
  return SDL_FALSE;
}

int SDL_SetWindowFullscreen(SDL_Window * window, Uint32 flags) {
  if(flags & SDL_WINDOW_FULLSCREEN || flags & SDL_WINDOW_FULLSCREEN_DESKTOP) {
    window->screen = SDL_SetVideoMode(window->w, window->h, DEFAULT_BPP, flags | SDL_FULLSCREEN | SDL_HWSURFACE | SDL_DOUBLEBUF);
  } else {
    window->screen = SDL_SetVideoMode(window->w, window->h, DEFAULT_BPP, flags & (~SDL_FULLSCREEN) | SDL_HWSURFACE);
  }
  return 0;
}

SDL_Texture* IMG_LoadTexture(SDL_Renderer *renderer, const char *file) {
  return SDL_CreateTextureFromSurface(renderer, IMG_Load(file));
}

int SDL2_SetColorKey(SDL_Surface *surface, Uint32 flag, Uint32 key) {
  if(flag == SDL_TRUE) {
    SDL_SetColorKey(surface, SDL_SRCCOLORKEY, key);
  } else {
    SDL_SetColorKey(surface, 0, key);
  }
}

int SDL_RendererSetScaleMode(SDL_Renderer* renderer, int mode) {
  renderer->scale_mode = mode;
}

