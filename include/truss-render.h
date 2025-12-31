#ifndef H_TRUSS_RENDER_H
#define H_TRUSS_RENDER_H

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "truss.h"

typedef struct truss_render{
    struct truss *truss;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
} Truss_Renderer;

int render_truss(struct truss *truss);

#endif