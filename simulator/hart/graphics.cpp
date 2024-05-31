#include "graphics.h"

#include <elf.h>
#include <sysexits.h>
#include <err.h>
#include <SDL2/SDL.h>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <iostream>
#include <utility>
#include <cstdint>

namespace rvsim::graphics {

static int PIXEL_BUFFER1[SGL_HEIGHT_DEFAULT][SGL_WIDTH_DEFAULT];
static int PIXEL_BUFFER2[SGL_HEIGHT_DEFAULT][SGL_WIDTH_DEFAULT];

static uint8_t *curr_buffer = (uint8_t *) PIXEL_BUFFER1;
static uint8_t *prev_buffer = (uint8_t *) PIXEL_BUFFER2;

static bool BUFFER_WAS_USED = false;

const size_t FRAME_TICKS = 3;

static SDL_Renderer *RENDERER = nullptr;
static SDL_Window *WINDOW = nullptr;
static SDL_Texture *TEXTURE = nullptr;
static Uint32 N_TICKS = 0;

int initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    WINDOW = SDL_CreateWindow("Game of life", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                              SGL_WIDTH_DEFAULT, SGL_HEIGHT_DEFAULT, 0);
    if (WINDOW == nullptr)
    {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    RENDERER = SDL_CreateRenderer(WINDOW, -1, SDL_RENDERER_ACCELERATED);
    if (RENDERER == nullptr)
    {
        fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        return -1;
    }

    TEXTURE = SDL_CreateTexture(RENDERER, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
                                          SGL_WIDTH_DEFAULT, SGL_HEIGHT_DEFAULT);
    if (TEXTURE == nullptr)
    {
        fprintf(stderr, "SDL_CreateTexture Error: %s\n", SDL_GetError());
        return -1;
    }

    SDL_SetRenderDrawColor(RENDERER, 0, 0, 0, 0);
    SDL_RenderClear(RENDERER);

    srand(time(nullptr));

    return 0;
}

void close()
{
    SDL_Event event;

    while (1)
    {
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;
    }

    SDL_DestroyTexture(TEXTURE);
    SDL_DestroyRenderer(RENDERER);
    SDL_DestroyWindow(WINDOW);
    SDL_Quit();
}

void flush()
{
    if (!BUFFER_WAS_USED) // cannot flush is nothing was done
        return;

    std::cout << "DUMP FLUSH START\n";
    SDL_UpdateTexture(TEXTURE, nullptr, curr_buffer, SGL_WIDTH_DEFAULT * sizeof(int));
    SDL_RenderCopy(RENDERER, TEXTURE, nullptr, nullptr);

    SDL_RenderPresent(RENDERER);

    SDL_PumpEvents();
    assert(SDL_TRUE != SDL_HasEvent(SDL_QUIT) && "User-requested quit");

    Uint32 cur_ticks = SDL_GetTicks() - N_TICKS;
    if (cur_ticks < FRAME_TICKS)
    {
        SDL_Delay(FRAME_TICKS - cur_ticks);
    }

    std::memcpy(prev_buffer, curr_buffer, sizeof(PIXEL_BUFFER1));
    std::swap(curr_buffer, prev_buffer);
    std::cout << "DUMP FLUSH\n";
}

uint8_t *get_prev_frame()
{
    return reinterpret_cast<uint8_t *>(prev_buffer);
}

uint8_t *get_curr_frame()
{
    return reinterpret_cast<uint8_t *>(curr_buffer);
}

uint8_t *get_init_frame()
{
    if (!BUFFER_WAS_USED) {
        BUFFER_WAS_USED = true;
    }

    return reinterpret_cast<uint8_t *>(PIXEL_BUFFER1);
}

bool is_init_frame(uint8_t *addr)
{
    return (addr >= reinterpret_cast<uint8_t *>(PIXEL_BUFFER1)) &&
           (addr < reinterpret_cast<uint8_t *>(PIXEL_BUFFER1) + sizeof(PIXEL_BUFFER1));
}

long rand(long seed)
{
    (void)seed;
    return std::rand();
}

} // namespace rvsim::graphics
