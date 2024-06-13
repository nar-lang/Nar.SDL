#include "_package.h"

nar_t *nar;

nar_int_t init(nar_t *n, nar_runtime_t rt) {
    nar = n;
    nar->register_def(rt, "Nar.SDL", "init", &native__init, 1);
    nar->register_def(rt, "Nar.SDL", "initSubSystem", &native__initSubSystem, 1);
    nar->register_def(rt, "Nar.SDL", "quitSubSystem", &native__quitSubSystem, 1);
    nar->register_def(rt, "Nar.SDL", "wasInit", &native__wasInit, 1);
    nar->register_def(rt, "Nar.SDL", "quit", &native__quit, 0);
    nar->register_def(rt, "Nar.SDL", "onQuit", &native__onQuit, 1);

    nar->register_def(rt, "Nar.SDL.MessageBox", "showSimpleMessageBox",
            &native__MessageBox_showSimpleMessageBox, 4);
    nar->register_def(rt, "Nar.SDL.MessageBox", "showMessageBox",
            &native__MessageBox_showMessageBox, 1);

    nar->register_def(rt, "Nar.SDL.Video", "createWindow", &native__createWindow, 5);
    nar->register_def(rt, "Nar.SDL.Video", "destroyWindow", &native__destroyWindow, 1);

    return 0;
}
