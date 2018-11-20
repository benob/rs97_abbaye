#ifndef __SDL2_COMPAT__
#define __SDL2_COMPAT__

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#define DEFAULT_BPP 16

typedef enum
{
    SDL_RENDERER_SOFTWARE = 0x00000001,         /**< The renderer is a software fallback */
    SDL_RENDERER_ACCELERATED = 0x00000002,      /**< The renderer uses hardware
                                                     acceleration */
    SDL_RENDERER_PRESENTVSYNC = 0x00000004,     /**< Present is synchronized
                                                     with the refresh rate */
    SDL_RENDERER_TARGETTEXTURE = 0x00000008     /**< The renderer supports
                                                     rendering to texture */
} SDL_RendererFlags;

typedef enum
{
    /* !!! FIXME: change this to name = (1<<x). */
    SDL_WINDOW_FULLSCREEN = 0x00000001,         /**< fullscreen window */
    SDL_WINDOW_OPENGL = 0x00000002,             /**< window usable with OpenGL context */
    SDL_WINDOW_SHOWN = 0x00000004,              /**< window is visible */
    SDL_WINDOW_HIDDEN = 0x00000008,             /**< window is not visible */
    SDL_WINDOW_BORDERLESS = 0x00000010,         /**< no window decoration */
    SDL_WINDOW_RESIZABLE = 0x00000020,          /**< window can be resized */
    SDL_WINDOW_MINIMIZED = 0x00000040,          /**< window is minimized */
    SDL_WINDOW_MAXIMIZED = 0x00000080,          /**< window is maximized */
    SDL_WINDOW_INPUT_GRABBED = 0x00000100,      /**< window has grabbed input focus */
    SDL_WINDOW_INPUT_FOCUS = 0x00000200,        /**< window has input focus */
    SDL_WINDOW_MOUSE_FOCUS = 0x00000400,        /**< window has mouse focus */
    SDL_WINDOW_FULLSCREEN_DESKTOP = ( SDL_WINDOW_FULLSCREEN | 0x00001000 ),
    SDL_WINDOW_FOREIGN = 0x00000800,            /**< window not created by SDL */
    SDL_WINDOW_ALLOW_HIGHDPI = 0x00002000,      /**< window should be created in high-DPI mode if supported.
                                                     On macOS NSHighResolutionCapable must be set true in the
                                                     application's Info.plist for this to have any effect. */
    SDL_WINDOW_MOUSE_CAPTURE = 0x00004000,      /**< window has mouse captured (unrelated to INPUT_GRABBED) */
    SDL_WINDOW_ALWAYS_ON_TOP = 0x00008000,      /**< window should always be above others */
    SDL_WINDOW_SKIP_TASKBAR  = 0x00010000,      /**< window should not be added to the taskbar */
    SDL_WINDOW_UTILITY       = 0x00020000,      /**< window should be treated as a utility window */
    SDL_WINDOW_TOOLTIP       = 0x00040000,      /**< window should be treated as a tooltip */
    SDL_WINDOW_POPUP_MENU    = 0x00080000,      /**< window should be treated as a popup menu */
    SDL_WINDOW_VULKAN        = 0x10000000       /**< window usable for Vulkan surface */
} SDL_WindowFlags;

#define SDL_WINDOWPOS_UNDEFINED_MASK    0x1FFF0000u
#define SDL_WINDOWPOS_UNDEFINED_DISPLAY(X)  (SDL_WINDOWPOS_UNDEFINED_MASK|(X))
#define SDL_WINDOWPOS_UNDEFINED         SDL_WINDOWPOS_UNDEFINED_DISPLAY(0)
#define SDL_WINDOWPOS_ISUNDEFINED(X)    \
            (((X)&0xFFFF0000) == SDL_WINDOWPOS_UNDEFINED_MASK)

/**
 *  \brief Used to indicate that the window position should be centered.
 */
#define SDL_WINDOWPOS_CENTERED_MASK    0x2FFF0000u
#define SDL_WINDOWPOS_CENTERED_DISPLAY(X)  (SDL_WINDOWPOS_CENTERED_MASK|(X))
#define SDL_WINDOWPOS_CENTERED         SDL_WINDOWPOS_CENTERED_DISPLAY(0)
#define SDL_WINDOWPOS_ISCENTERED(X)    \
            (((X)&0xFFFF0000) == SDL_WINDOWPOS_CENTERED_MASK)

#define SDL_HINT_RENDER_SCALE_QUALITY       "SDL_RENDER_SCALE_QUALITY"

typedef enum
{
    SDL_FLIP_NONE = 0x00000000,     /**< Do not flip */
    SDL_FLIP_HORIZONTAL = 0x00000001,    /**< flip horizontally */
    SDL_FLIP_VERTICAL = 0x00000002     /**< flip vertically */
} SDL_RendererFlip;

typedef struct 
{
    int x;
    int y;
} SDL_Point;

typedef struct {
  SDL_Surface* screen;
  int w, h;
  Uint32 flags;
} SDL_Window;

typedef struct {
  SDL_Surface* surface;
  SDL_Window* window;
  int w, h;
  Uint32 color;
  Uint32 last_frame_ticks;
  int scale_mode;
} SDL_Renderer;

typedef struct {
  SDL_Surface* surface;
} SDL_Texture;

SDL_Renderer* SDL_CreateRenderer(SDL_Window * window, int index, Uint32 flags);
SDL_Texture * SDL_CreateTextureFromSurface(SDL_Renderer * renderer, SDL_Surface * surface);
void SDL_DestroyTexture(SDL_Texture * texture);
void SDL_DestroyRenderer(SDL_Renderer * renderer);
int SDL_RenderClear(SDL_Renderer * renderer);
int SDL_RenderCopy(SDL_Renderer * renderer, SDL_Texture * texture, const SDL_Rect * srcrect, const SDL_Rect * dstrect);
int SDL_RenderCopyEx(SDL_Renderer * renderer, SDL_Texture * texture, const SDL_Rect * srcrect, const SDL_Rect * dstrect, const double angle, const SDL_Point *center, const SDL_RendererFlip flip);
void SDL_RenderPresent(SDL_Renderer * renderer);
int SDL_RenderSetLogicalSize(SDL_Renderer * renderer, int w, int h);
int SDL_SetRenderDrawColor(SDL_Renderer * renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

SDL_Window * SDL_CreateWindow(const char *title, int x, int y, int w, int h, Uint32 flags);
void SDL_DestroyWindow(SDL_Window * window);
SDL_bool SDL_SetHint(const char *name, const char *value);
int SDL_SetWindowFullscreen(SDL_Window * window, Uint32 flags);

SDL_Texture* IMG_LoadTexture(SDL_Renderer *renderer, const char *file);

int SDL2_SetColorKey(SDL_Surface *surface, Uint32 flag, Uint32 key);
int SDL_RendererSetScaleMode(SDL_Renderer* renderer, int mode);

#endif
