#include "_package.h"

nar_t *nar;

nar_int_t init(nar_t *n, nar_runtime_t rt) {
    nar = n;
    init_blend_mode(rt);
    init_message_box(rt);
    init_pixels(rt);
    init_rect(rt);
    init_render(rt);
    init_sdl(rt);
    init_video(rt);
    return 0;
}
