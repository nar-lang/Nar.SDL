#include "_package.h"

nar_t *nar;

nar_int_t init(nar_t *n, nar_runtime_t rt) {
    nar = n;
    nar->register_def_dynamic(rt, "Nar.SDL", "init", "SDL_init", 1);
    nar->register_def_dynamic(rt, "Nar.SDL", "initSubSystem", "SDL_initSubSystem", 1);
    nar->register_def_dynamic(rt, "Nar.SDL", "quitSubSystem", "SDL_quitSubSystem", 1);
    nar->register_def_dynamic(rt, "Nar.SDL", "wasInit", "SDL_wasInit", 1);
    nar->register_def_dynamic(rt, "Nar.SDL", "quit", "SDL_quit", 0);

    nar->register_def_dynamic(rt, "Nar.SDL.MessageBox", "showSimpleMessageBox", "native__MessageBox_showSimpleMessageBox", 4);
    nar->register_def_dynamic(rt, "Nar.SDL.MessageBox", "showMessageBox", "native__MessageBox_showMessageBox", 1);

    return 0;
}
