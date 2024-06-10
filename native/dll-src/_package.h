#ifndef PACKAGE_H
#define PACKAGE_H

#include <nar.h>
#include <nar-package.h>
#include "include/Nar.SDL.h"

extern nar_t *nar;

nar_object_t native__init(nar_runtime_t rt, nar_object_t subSystems);

nar_object_t native__initSubSystem(nar_runtime_t rt, nar_object_t subSystem);

nar_object_t native__quitSubSystem(nar_runtime_t rt, nar_object_t subSystem);

nar_object_t native__wasInit(nar_runtime_t rt, nar_object_t subSystems, nar_object_t toMsg);

nar_object_t native__quit(nar_runtime_t rt);

nar_object_t native__MessageBox_showSimpleMessageBox(nar_runtime_t rt,
        nar_object_t icon, nar_object_t title, nar_object_t message, nar_object_t window);

nar_object_t native__MessageBox_showMessageBox(nar_runtime_t rt, nar_object_t data);

nar_object_t native__createWindow(nar_runtime_t rt, nar_object_t title, nar_object_t position,
        nar_object_t width, nar_object_t height, nar_object_t flags);

nar_object_t native__destroyWindow(nar_runtime_t rt, nar_object_t window);

#endif //PACKAGE_H
