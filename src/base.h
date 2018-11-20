/* base.h */

#pragma once

# include <stdio.h>
# include <stdlib.h>

#ifdef USE_SDL2_COMPAT
  #include "SDL2_compat.h"
  # include "SDL/SDL_image.h"
  # include "SDL/SDL_mixer.h"
#else
  # include "SDL2/SDL.h"
  # include "SDL2/SDL_image.h"
  # include "SDL2/SDL_mixer.h"
#endif

// SDL2 renderer. Needs to be declared here because several different units access to it
// directly to draw on it because there's no discrete graphics unit, but it works, so no complains :D
SDL_Renderer *renderer;

// Default layout to PSX gamepad with USB adapter
#define JUMP_JOYBUTTON 2
#define SELECT_JOYBUTTON 8
#define START_JOYBUTTON 9
#define X_JOYAXIS 0
#define Y_JOYAXIS 1
