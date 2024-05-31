#ifndef SIMULATOR_HART_GRAPHICS_H
#define SIMULATOR_HART_GRAPHICS_H

#include <cstddef>
#include <cstdint>

namespace rvsim::graphics {

static constexpr size_t SGL_WIDTH_DEFAULT = 1080;
static constexpr size_t SGL_HEIGHT_DEFAULT = 720;

int initialize();
void close();

uint8_t *get_prev_frame();
uint8_t *get_curr_frame();
uint8_t *get_init_frame();
void flush();
long rand(long seed);

bool is_init_frame(uint8_t *addr);

} // namespace rvsim::graphics

#endif // SIMULATOR_HART_GRAPHICS_H
