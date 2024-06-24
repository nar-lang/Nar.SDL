#ifndef PACKAGE_H
#define PACKAGE_H

#include <nar.h>
#include <nar-package.h>
#include "include/Nar.SDL.h"
#include <SDL.h>

#define NAR_META__Nar_SDL_onQuit_sub "Nar.SDL.onQuit:sub"
#define NAR_META__Nar_SDL_Video_windowHitTest_data "Nar.SDL.Video.windowHitTest:data"

extern nar_t *nar;

void init_blend_mode(nar_runtime_t rt);
void init_message_box(nar_runtime_t rt);
void init_pixels(nar_runtime_t rt);
void init_rect(nar_runtime_t rt);
void init_render(nar_runtime_t rt);
void init_sdl(nar_runtime_t rt);
void init_video(nar_runtime_t rt);


void map_Color(nar_runtime_t rt, nar_object_t obj, nar_cstring_t key, SDL_Color *result);

void map_Point(nar_runtime_t rt, nar_cstring_t key, nar_object_t value, SDL_Point *point);

nar_object_t make_Point(nar_runtime_t rt, const SDL_Point *point);

void map_FPoint(nar_runtime_t rt, nar_cstring_t key, nar_object_t value, SDL_FPoint *point);

nar_object_t make_FPoint(nar_runtime_t rt, const SDL_FPoint *point);

void map_Rect(nar_runtime_t rt, nar_cstring_t key, nar_object_t value, SDL_Rect *rect);

nar_object_t make_Rect(nar_runtime_t rt, const SDL_Rect *rect);

void map_FRect(nar_runtime_t rt, nar_cstring_t key, nar_object_t value, SDL_FRect *rect);

nar_object_t make_FRect(nar_runtime_t rt, const SDL_FRect *rect);

#endif //PACKAGE_H
