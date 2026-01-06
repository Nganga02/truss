#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "truss-render.h"
#include "truss-io.h"
#include "truss.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

/**
 *Function to initiialize the renderer
 */
static int SDL_initialize(Truss_Renderer *truss_renderer)
{
    if (SDL_Init(SDL_INIT_VIDEO))
    {
        fprintf(stderr, "Error initialising video: %s\n", SDL_GetError());
        return 0;
    }

    truss_renderer->window = SDL_CreateWindow("Truss render",
                                              SDL_WINDOWPOS_CENTERED,
                                              SDL_WINDOWPOS_CENTERED,
                                              WINDOW_WIDTH,
                                              WINDOW_HEIGHT,
                                              0);

    if (!truss_renderer->window)
    {
        fprintf(stderr, "Error creating animation window: %s\n", SDL_GetError());
        return 0;
    }

    truss_renderer->renderer = SDL_CreateRenderer(truss_renderer->window, -1, 0);

    if (!truss_renderer->renderer)
    {
        fprintf(stderr, "Error creating renderer: %s\n", SDL_GetError());
        return 0;
    }
    return 1;
}

/**
 *Used to clean up memory on the event of failure or when closing the animation
 *Accepts to arguments animation and exit_status
 */
static void animation_cleanup(Truss_Renderer *truss_render_ptr, int exit_status)
{

    SDL_DestroyTexture(truss_render_ptr->texture);
    SDL_DestroyRenderer(truss_render_ptr->renderer);
    SDL_DestroyWindow(truss_render_ptr->window);
    SDL_Quit();
    fprintf(stderr, "[EXIT STATUS]____________%i_____________\n", exit_status);
    if (exit_status == EXIT_FAILURE)
    {
        if (truss_render_ptr != NULL)
            free_truss(truss_render_ptr->truss);
        exit(exit_status);
    }
}

/**
 * Rendering fixed points
 * Accepts the following arguments:
 * \param node: This is the node structure of the fixed joint of the truss
 * \returns 0 on success or a negative error code on failure; call
 *          SDL_GetError() for more information.
 */
static int render_fjoint(SDL_Renderer *renderer, struct node *fixed_node)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 127);

    const SDL_FPoint line_points[] = {
        {fixed_node->x, fixed_node->y},
        {fixed_node->x - 12, fixed_node->y + 24},
        {fixed_node->x + 12, fixed_node->y + 24},
        {fixed_node->x, fixed_node->y},
    };

    return SDL_RenderDrawLinesF(renderer, line_points, SDL_arraysize(line_points));

    /**
     * `TODO`: Rendering the translated lines to show a fixed joint
     */
}

/**
 * Rendering a moving joint
 * Accepts the following arguments:
 * \param node: This is the node structure of the moving joint of the truss
 */
int render_mjoint(SDL_Renderer *renderer, struct node *moving_node)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 127);

    SDL_FPoint line_points[] = {
        {moving_node->x, moving_node->y},
        {moving_node->x - 12, moving_node->y + 24},
        {moving_node->x + 12, moving_node->y + 24},
        {moving_node->x, moving_node->y},
    };

    return SDL_RenderDrawLinesF(renderer, line_points, SDL_arraysize(line_points));
}

/**
 * Rendering nodes
 */

/**
 * Rendring a link
 * Accepts the following arguments:
 * \param link This is the link structure of the truss
 * The following properties are used to render the link:
 * \param length
 * \param nodes_1_and_2
 * \param area
 */
static int render_link(SDL_Renderer *renderer, struct link *link)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    // We will be using the filled rectangle
    SDL_FPoint points[] = {
        {link->np1->x, link->np1->y},
        {link->np2->x, link->np2->y}};

    return SDL_RenderDrawLinesF(renderer, points, SDL_arraysize(points));
    /**
     * `TODO`: Rendering the links width to show stretching and compression
     */
}

/**
 * Rendering labels
 */

/**
 * Since we are getting a truss object when we read the tdf file, we are accepting
 * the pointer to the truss object created when we call the read_truss function.
 */

void render_truss(struct truss *truss)
{
    Truss_Renderer truss_render = {
        .truss = truss,
        .window = NULL,
        .renderer = NULL,
        .texture = NULL};

    Truss_Renderer *truss_render_ptr = &truss_render;
    // Initializing the SDL object
    if (!SDL_initialize(truss_render_ptr))
    {
        printf("An error occured:*** Error initializing a window");
        animation_cleanup(truss_render_ptr, EXIT_FAILURE);
    }

    // Positioning a stress key
    SDL_Rect rects[15];
    int live = 1;
    while (live)
    {

        SDL_Event event;
        while (SDL_PollEvent(&event)) // The sdl_pollevent function expects a pointer to an event
        {
            switch (event.type)
            {
            case SDL_QUIT:
                live = 0;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                    live = 0;
                break;
            default:
                break;
            }
        }

         // Creating the Canvas
        SDL_SetRenderDrawColor(truss_render_ptr->renderer, 0, 0, 0,
                            SDL_ALPHA_OPAQUE);
        SDL_RenderClear(truss_render_ptr->renderer);

        // Rendering key
        for (int i = 0; i < 15; i++)
        {
            rects[i].w = rects[i].h = 30.0;
            rects[i].x = (WINDOW_WIDTH * 7.0) / 8.0;
            rects[i].y = (0.5 * (WINDOW_HEIGHT - 15 * (rects[i].h))) + i * rects[i].h;

            SDL_SetRenderDrawColor(truss_render_ptr->renderer,
                                   MAX(0, 252 - (36 * i)),
                                   252 - (36 * abs(7 - i)),
                                   MAX(0, (36 * (i - 7))),
                                   255);
            SDL_RenderFillRect(truss_render_ptr->renderer, &rects[i]);
        }

        // Rendering links
        for (
            Conscell *p = truss_render_ptr->truss->links_list;
            p != NULL;
            p = p->next)
        {
            struct link *lk_ptr = (struct link *)p->data;
            render_link(truss_render_ptr->renderer, lk_ptr);
        }

        // Rendering nodes(fixed and movable)
        for (
            Conscell *p = truss_render_ptr->truss->nodes_list;
            p != NULL;
            p = p->next)
        {
            struct node *node_ptr = (struct node *)p->data;
            if (node_ptr->xfixed && node_ptr->yfixed)
            {
                render_fjoint(truss_render_ptr->renderer, node_ptr);
            }
            if (node_ptr->xfixed != node_ptr->yfixed)//This is performing a logical XOR
            {
                render_mjoint(truss_render_ptr->renderer, node_ptr);
            }
        }

        SDL_RenderPresent(truss_render_ptr->renderer);
        SDL_Delay(32);
    }
    animation_cleanup(truss_render_ptr, EXIT_SUCCESS);
}
