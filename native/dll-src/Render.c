#include "_package.h"
#include <Nar.Program.h>
#include "Nar.Base.h"

nar_object_t make_render_info(nar_runtime_t rt, SDL_RendererInfo *info) {
    nar_cstring_t keys[] = {
            "name", "flags", "numTextureFormats", "maxTextureWidth", "maxTextureHeight"};
    nar_object_t values[] = {
            nar->make_string(rt, info->name),
            nar->make_int(rt, info->flags),
            nar->make_int(rt, info->num_texture_formats),
            nar->make_int(rt, info->max_texture_width),
            nar->make_int(rt, info->max_texture_height)};
    return nar->make_record(rt, 5, keys, values);
}

nar_object_t /* Result[String, Int] */ native__Render_numRenderDrivers(nar_runtime_t rt) {
    int n = SDL_GetNumRenderDrivers();
    if (n < 0) {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    } else {
        nar_object_t result = nar->make_int(rt, n);
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &result);
    }
}

nar_object_t /* Result[String, RendererInfo] */ native__Render_getRenderDriverInfo(nar_runtime_t rt,
        nar_object_t index /* Int */) {
    SDL_RendererInfo info;
    if (0 == SDL_GetRenderDriverInfo((int) nar->to_int(rt, index), &info)) {
        nar_object_t result = make_render_info(rt, &info);
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &result);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

void create_window_and_renderer(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    SDL_Window *window;
    SDL_Renderer *renderer;
    nar_tuple_t data = nar->to_tuple(rt, payload);
    Uint32 flags = nar->to_enum_option_flags(rt, data.values[2]);

    int status = SDL_CreateWindowAndRenderer(
            (int) nar->to_int(rt, data.values[0]), (int) nar->to_int(rt, data.values[1]), flags,
            &window, &renderer);
    if (0 == status) {
        nar_object_t window_obj = nar->make_native(rt, window, NULL);
        nar_object_t renderer_obj = nar->make_native(rt, renderer, NULL);
        nar_object_t result = nar->make_tuple(rt, 2, (nar_object_t[]) {window_obj, renderer_obj});
        resolve(rt, result, task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, (Window, Renderer)] */ native__Render_createWindowAndRenderer(
        nar_runtime_t rt,
        nar_object_t width /* Int */,
        nar_object_t height /* Int */,
        nar_object_t flags /* List[WindowFlag] */ ) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 3, (nar_object_t[]) {width, height, flags});
    return task_new(rt, data, &create_window_and_renderer);
}

void create_renderer(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    SDL_Window *window = nar->to_native(rt, data.values[0]).ptr;
    int index = (int) nar->to_int(rt, data.values[1]);
    Uint32 flags = nar->to_enum_option_flags(rt, data.values[2]);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, index, flags);
    if (renderer) {
        nar_object_t renderer_obj = nar->make_native(rt, renderer, NULL);
        resolve(rt, renderer_obj, task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, Renderer] */ native__Render_createRenderer(nar_runtime_t rt,
        nar_object_t window /* Window */,
        nar_object_t index /* Maybe[Int] */,
        nar_object_t flags /* List[RendererFlag] */ ) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 3, (nar_object_t[]) {window, index, flags});
    return task_new(rt, data, &create_renderer);
}

void create_software_renderer(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    SDL_Surface *surface = nar->to_native(rt, payload).ptr;
    SDL_Renderer *renderer = SDL_CreateSoftwareRenderer(surface);
    if (renderer) {
        nar_object_t renderer_obj = nar->make_native(rt, renderer, NULL);
        resolve(rt, renderer_obj, task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, Renderer] */ native__Render_createSoftwareRenderer(nar_runtime_t rt,
        nar_object_t surface /* Surface */ ) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, surface, &create_software_renderer);
}

nar_object_t /* Result[String, Renderer] */ native__Render_getRenderer(nar_runtime_t rt,
        nar_object_t window /* Window */) {
    SDL_Window *win = nar->to_native(rt, window).ptr;
    SDL_Renderer *renderer = SDL_GetRenderer(win);
    if (renderer) {
        nar_object_t renderer_obj = nar->make_native(rt, renderer, NULL);
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &renderer_obj);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Result[String, Window] */ native__Render_renderGetWindow(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */) {
    SDL_Renderer *r = nar->to_native(rt, renderer).ptr;
    SDL_Window *window = SDL_RenderGetWindow(r);
    if (window) {
        nar_object_t window_obj = nar->make_native(rt, window, NULL);
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &window_obj);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Result[String, RendererInfo] */ native__Render_getRendererInfo(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */) {
    SDL_RendererInfo info;
    if (0 == SDL_GetRendererInfo(nar->to_native(rt, renderer).ptr, &info)) {
        nar_object_t result = make_render_info(rt, &info);
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &result);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Result[String, (Int, Int)] */ native__Render_getRendererOutputSize(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */) {
    int w, h;
    if (0 == SDL_GetRendererOutputSize(nar->to_native(rt, renderer).ptr, &w, &h)) {
        nar_object_t result = nar->make_tuple(rt, 2, (nar_object_t[]) {
                nar->make_int(rt, w), nar->make_int(rt, h)});
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &result);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

void create_texture(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    SDL_Renderer *renderer = nar->to_native(rt, data.values[0]).ptr;
    Uint32 format = nar->to_enum_option(rt, data.values[1]);
    Uint32 access = nar->to_enum_option(rt, data.values[2]);
    int w = (int) nar->to_int(rt, data.values[3]);
    int h = (int) nar->to_int(rt, data.values[4]);
    SDL_Texture *texture = SDL_CreateTexture(renderer, format, (int) access, w, h);
    if (NULL != texture) {
        nar_object_t texture_obj = nar->make_native(rt, texture, NULL);
        resolve(rt, texture_obj, task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, Texture] */ native__Render_createTexture(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t format /* PixelFormat */,
        nar_object_t access /* TextureAccess */,
        nar_object_t w /* Int */,
        nar_object_t h /* Int */ ) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 5, (nar_object_t[]) {renderer, format, access, w, h});
    return task_new(rt, data, &create_texture);
}

void create_texture_from_surface(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    SDL_Renderer *renderer = nar->to_native(rt, data.values[0]).ptr;
    SDL_Surface *surface = nar->to_native(rt, data.values[1]).ptr;
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (NULL != texture) {
        nar_object_t texture_obj = nar->make_native(rt, texture, NULL);
        resolve(rt, texture_obj, task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, Texture] */ native__Render_createTextureFromSurface(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t surface /* Surface */ ) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 2, (nar_object_t[]) {renderer, surface});
    return task_new(rt, data, &create_texture_from_surface);
}

void query_texture(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    SDL_Texture *texture = nar->to_native(rt, payload).ptr;
    Uint32 format;
    int access, w, h;
    if (0 == SDL_QueryTexture(texture, &format, &access, &w, &h)) {
        nar_cstring_t keys[] = {"format", "access", "w", "h"};
        nar_object_t values[] = {
                nar->make_int(rt, format),
                nar->make_int(rt, access),
                nar->make_int(rt, w),
                nar->make_int(rt, h)
        };
        nar_object_t result = nar->make_record(rt, 4, keys, values);
        resolve(rt, result, task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Result[String, TextureInfo] */ native__Render_queryTexture(nar_runtime_t rt,
        nar_object_t texture /* Texture */ ) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, texture, &query_texture);
}

nar_object_t /* Result[String, Texture] */ native__Render_setTextureColorMod(nar_runtime_t rt,
        nar_object_t texture /* Texture */,
        nar_object_t r /* Int */,
        nar_object_t g /* Int */,
        nar_object_t b /* Int */ ) {
    SDL_Texture *tex = nar->to_native(rt, texture).ptr;
    Uint8 cR = (Uint8) nar->to_int(rt, r);
    Uint8 cG = (Uint8) nar->to_int(rt, g);
    Uint8 cB = (Uint8) nar->to_int(rt, b);
    if (0 == SDL_SetTextureColorMod(tex, cR, cG, cB)) {
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &texture);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Result[String, (Int, Int, Int)] */
native__Render_getTextureColorMod(nar_runtime_t rt,
        nar_object_t texture /* Texture */ ) {
    Uint8 r, g, b;
    if (0 == SDL_GetTextureColorMod(nar->to_native(rt, texture).ptr, &r, &g, &b)) {
        nar_object_t result = nar->make_tuple(rt, 3, (nar_object_t[]) {
                nar->make_int(rt, r), nar->make_int(rt, g), nar->make_int(rt, b)});
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &result);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Result[String, Texture] */ native__Render_setTextureAlphaMod(nar_runtime_t rt,
        nar_object_t texture /* Texture */,
        nar_object_t alpha /* Int */ ) {
    SDL_Texture *tex = nar->to_native(rt, texture).ptr;
    Uint8 a = (Uint8) nar->to_int(rt, alpha);
    if (0 == SDL_SetTextureAlphaMod(tex, a)) {
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &texture);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Result[String, Int] */ native__Render_getTextureAlphaMod(nar_runtime_t rt,
        nar_object_t texture /* Texture */ ) {
    Uint8 alpha;
    if (0 == SDL_GetTextureAlphaMod(nar->to_native(rt, texture).ptr, &alpha)) {
        nar_object_t result = nar->make_int(rt, alpha);
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &result);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Result[String, Texture] */ native__Render_setTextureBlendMode(nar_runtime_t rt,
        nar_object_t texture /* Texture */,
        nar_object_t blendMode /* BlendMode */ ) {
    SDL_Texture *tex = nar->to_native(rt, texture).ptr;
    SDL_BlendMode mode = nar->to_enum_option(rt, blendMode);
    if (0 == SDL_SetTextureBlendMode(tex, mode)) {
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &texture);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Result[String, BlendMode] */ native__Render_getTextureBlendMode(nar_runtime_t rt,
        nar_object_t texture /* Texture */ ) {
    SDL_BlendMode mode;
    if (0 == SDL_GetTextureBlendMode(nar->to_native(rt, texture).ptr, &mode)) {
        nar_object_t result = nar->make_enum_option(rt, Nar_SDL_Blend_BlendMode, mode, 0, NULL);
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &result);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Result[String, Texture] */ native__Render_setTextureScaleMode(nar_runtime_t rt,
        nar_object_t texture /* Texture */,
        nar_object_t scaleMode /* ScaleMode */ ) {
    SDL_Texture *tex = nar->to_native(rt, texture).ptr;
    SDL_ScaleMode mode = nar->to_enum_option(rt, scaleMode);
    if (0 == SDL_SetTextureScaleMode(tex, mode)) {
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &texture);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Result[String, ScaleMode] */ native__Render_getTextureScaleMode(nar_runtime_t rt,
        nar_object_t texture /* Texture */ ) {
    SDL_ScaleMode mode;
    if (0 == SDL_GetTextureScaleMode(nar->to_native(rt, texture).ptr, &mode)) {
        nar_object_t result = nar->make_enum_option(rt, Nar_SDL_Render_ScaleMode, mode, 0, NULL);
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &result);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Result[String, Texture] */ native__Render_setTextureUserData(nar_runtime_t rt,
        nar_object_t texture /* Texture */,
        nar_object_t userData /* Ptr */ ) {
    SDL_Texture *tex = nar->to_native(rt, texture).ptr;
    void *value = nar->to_native(rt, userData).ptr;
    if (0 == SDL_SetTextureUserData(tex, value)) {
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &texture);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Result[String, Ptr] */ native__Render_getTextureUserData(nar_runtime_t rt,
        nar_object_t texture /* Texture */ ) {
    void *value = SDL_GetTextureUserData(nar->to_native(rt, texture).ptr);
    if (value) {
        nar_object_t result = nar->make_native(rt, value, NULL);
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &result);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Result[String, Texture] */ native__Render_updateTexture(nar_runtime_t rt,
        nar_object_t texture /* Texture */,
        nar_object_t rect /* Rect */,
        nar_object_t pixels /* Ptr */,
        nar_object_t pitch /* Int */ ) {
    SDL_Texture *tex = nar->to_native(rt, texture).ptr;
    SDL_Rect r;
    nar->map_record(rt, rect, &r, (nar_map_record_cb_fn_t) map_Rect);
    void *p = nar->to_native(rt, pixels).ptr;
    int pch = (int) nar->to_int(rt, pitch);
    if (0 == SDL_UpdateTexture(tex, &r, p, pch)) {
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &texture);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Result[String, Texture] */ native__Render_updateYUVTexture(nar_runtime_t rt,
        nar_object_t texture /* Texture */,
        nar_object_t rect /* Rect */,
        nar_object_t yPlane /* Ptr */,
        nar_object_t yPitch /* Int */,
        nar_object_t uPlane /* Ptr */,
        nar_object_t uPitch /* Int */,
        nar_object_t vPlane /* Ptr */,
        nar_object_t vPitch /* Int */ ) {
    SDL_Texture *tex = nar->to_native(rt, texture).ptr;
    SDL_Rect r;
    nar->map_record(rt, rect, &r, (nar_map_record_cb_fn_t) map_Rect);
    void *y = nar->to_native(rt, yPlane).ptr;
    int yPch = (int) nar->to_int(rt, yPitch);
    void *u = nar->to_native(rt, uPlane).ptr;
    int uPch = (int) nar->to_int(rt, uPitch);
    void *v = nar->to_native(rt, vPlane).ptr;
    int vPch = (int) nar->to_int(rt, vPitch);
    if (0 == SDL_UpdateYUVTexture(tex, &r, y, yPch, u, uPch, v, vPch)) {
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &texture);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

void lock_texture(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    SDL_Texture *texture = nar->to_native(rt, data.values[0]).ptr;
    SDL_Rect rect;
    nar->map_record(rt, data.values[1], &rect, (nar_map_record_cb_fn_t) map_Rect);
    void *pixels;
    int pitch;
    if (0 == SDL_LockTexture(texture, &rect, &pixels, &pitch)) {
        nar_object_t pixels_obj = nar->make_native(rt, pixels, NULL);
        nar_object_t pitch_obj = nar->make_int(rt, pitch);
        nar_object_t result = nar->make_tuple(rt, 2, (nar_object_t[]) {pixels_obj, pitch_obj});
        resolve(rt, result, task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, (Ptr, Int)] */ native__Render_lockTexture(nar_runtime_t rt,
        nar_object_t texture /* Texture */,
        nar_object_t rect /* Rect */ ) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 2, (nar_object_t[]) {texture, rect});
    return task_new(rt, data, &lock_texture);
}

void lock_texture_to_surface(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    SDL_Texture *texture = nar->to_native(rt, data.values[0]).ptr;
    SDL_Rect rect;
    nar->map_record(rt, data.values[1], &rect, (nar_map_record_cb_fn_t) map_Rect);
    SDL_Surface *surface;
    if (0 == SDL_LockTextureToSurface(texture, &rect, &surface)) {
        nar_object_t surface_obj = nar->make_native(rt, surface, NULL);
        resolve(rt, surface_obj, task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, Surface] */ native__Render_lockTextureToSurface(nar_runtime_t rt,
        nar_object_t texture /* Texture */,
        nar_object_t rect /* Rect */ ) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 2, (nar_object_t[]) {texture, rect});
    return task_new(rt, data, &lock_texture_to_surface);
}

void unlock_texture(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    SDL_Texture *texture = nar->to_native(rt, payload).ptr;
    SDL_UnlockTexture(texture);
    resolve(rt, payload, task_state);
}

nar_object_t /* Task[String, ()] */ native__Render_unlockTexture(nar_runtime_t rt,
        nar_object_t texture /* Texture */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, texture, &unlock_texture);
}

nar_object_t /* Bool */ native__Render_renderTargetSupported(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */) {
    return nar->to_bool(rt, SDL_RenderTargetSupported(nar->to_native(rt, renderer).ptr));
}

nar_object_t /* Result[String, Renderer] */ native__Render_setRenderTarget(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t texture /* Maybe[Texture] */ ) {
    SDL_Renderer *r = nar->to_native(rt, renderer).ptr;
    SDL_Texture *tex = NULL;
    nar_option_t texture_opt = nar->to_option(rt, texture);
    if (0 == strcmp(texture_opt.name, Nar_Base_Maybe_Maybe__Just)) {
        tex = nar->to_native(rt, texture_opt.values[0]).ptr;
    }
    if (0 == SDL_SetRenderTarget(r, tex)) {
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &renderer);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Maybe[Texture] */ native__Render_getRenderTarget(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */ ) {
    SDL_Texture *texture = SDL_GetRenderTarget(nar->to_native(rt, renderer).ptr);
    if (NULL != texture) {
        nar_object_t texture_obj = nar->make_native(rt, texture, NULL);
        return nar->make_option(rt, Nar_Base_Maybe_Maybe__Just, 1, &texture_obj);
    } else {
        return nar->make_option(rt, Nar_Base_Maybe_Maybe__Nothing, 0, NULL);
    }
}

nar_object_t /* Result[String, Renderer] */ native__Render_renderSetLogicalSize(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t w /* Int */,
        nar_object_t h /* Int */ ) {
    SDL_Renderer *r = nar->to_native(rt, renderer).ptr;
    int width = (int) nar->to_int(rt, w);
    int height = (int) nar->to_int(rt, h);
    if (0 == SDL_RenderSetLogicalSize(r, width, height)) {
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &renderer);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* (Int, Int) */ native__Render_renderGetLogicalSize(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */ ) {
    int w, h;
    SDL_RenderGetLogicalSize(nar->to_native(rt, renderer).ptr, &w, &h);
    return nar->make_tuple(rt, 2, (nar_object_t[]) {nar->make_int(rt, w), nar->make_int(rt, h)});
}

nar_object_t /* Result[String, Renderer] */ native__Render_renderSetIntegerScale(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t enable /* Bool */ ) {
    SDL_Renderer *r = nar->to_native(rt, renderer).ptr;
    SDL_bool e = nar->to_bool(rt, enable);
    if (0 == SDL_RenderSetIntegerScale(r, e)) {
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &renderer);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Bool */ native__Render_renderGetIntegerScale(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */ ) {
    return nar->to_bool(rt, SDL_RenderGetIntegerScale(nar->to_native(rt, renderer).ptr));
}

nar_object_t /* Result[String, Renderer] */ native__Render_renderSetViewport(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t rect /* Rect */ ) {
    SDL_Renderer *r = nar->to_native(rt, renderer).ptr;
    SDL_Rect viewport;
    nar->map_record(rt, rect, &viewport, (nar_map_record_cb_fn_t) map_Rect);
    if (0 == SDL_RenderSetViewport(r, &viewport)) {
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &renderer);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Rect */ native__Render_renderGetViewport(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */ ) {
    SDL_Rect rect;
    SDL_RenderGetViewport(nar->to_native(rt, renderer).ptr, &rect);
    return make_Rect(rt, &rect);
}

nar_object_t /* Result[String, Renderer] */ native__Render_renderSetClipRect(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t rect /* Rect */ ) {
    SDL_Renderer *r = nar->to_native(rt, renderer).ptr;
    SDL_Rect clip;
    nar->map_record(rt, rect, &clip, (nar_map_record_cb_fn_t) map_Rect);
    if (0 == SDL_RenderSetClipRect(r, &clip)) {
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &renderer);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* Rect */ native__Render_renderGetClipRect(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */ ) {
    SDL_Rect rect;
    SDL_RenderGetClipRect(nar->to_native(rt, renderer).ptr, &rect);
    return make_Rect(rt, &rect);
}

nar_object_t /* Bool */ native__Render_renderIsClipEnabled(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */) {
    return nar->to_bool(rt, SDL_RenderIsClipEnabled(nar->to_native(rt, renderer).ptr));
}

nar_object_t /* Result[String, Renderer] */ native__Render_renderSetScale(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t scaleX /* Float */,
        nar_object_t scaleY /* Float */ ) {
    SDL_Renderer *r = nar->to_native(rt, renderer).ptr;
    float x = (float) nar->to_float(rt, scaleX);
    float y = (float) nar->to_float(rt, scaleY);
    if (0 == SDL_RenderSetScale(r, x, y)) {
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &renderer);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* (Float, Float) */ native__Render_renderGetScale(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */ ) {
    float scaleX, scaleY;
    SDL_RenderGetScale(nar->to_native(rt, renderer).ptr, &scaleX, &scaleY);
    return nar->make_tuple(rt, 2,
            (nar_object_t[]) {nar->make_float(rt, scaleX), nar->make_float(rt, scaleY)});
}

nar_object_t /* (Float, Float) */ native__Render_renderWindowToLogical(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t x /* Float */,
        nar_object_t y /* Float */ ) {
    float outX, outY;
    SDL_RenderWindowToLogical(nar->to_native(rt, renderer).ptr,
            (int) nar->to_int(rt, x), (int) nar->to_int(rt, y), &outX, &outY);
    return nar->make_tuple(rt, 2,
            (nar_object_t[]) {nar->make_float(rt, outX), nar->make_float(rt, outY)});
}

nar_object_t /* (Float, Float) */ native__Render_renderLogicalToWindow(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t x /* Int */,
        nar_object_t y /* Int */ ) {
    int outX, outY;
    SDL_RenderLogicalToWindow(nar->to_native(rt, renderer).ptr,
            (float) nar->to_float(rt, x), (float) nar->to_float(rt, y), &outX, &outY);
    return nar->make_tuple(rt, 2,
            (nar_object_t[]) {nar->make_int(rt, outX), nar->make_int(rt, outY)});
}

nar_object_t /* Result[String, Renderer] */ native__Render_setRenderDrawColor(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t r /* Int */,
        nar_object_t g /* Int */,
        nar_object_t b /* Int */,
        nar_object_t a /* Int */ ) {
    SDL_Renderer *rdr = nar->to_native(rt, renderer).ptr;
    Uint8 cR = (Uint8) nar->to_int(rt, r);
    Uint8 cG = (Uint8) nar->to_int(rt, g);
    Uint8 cB = (Uint8) nar->to_int(rt, b);
    Uint8 cA = (Uint8) nar->to_int(rt, a);
    if (0 == SDL_SetRenderDrawColor(rdr, cR, cG, cB, cA)) {
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &renderer);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* (Int, Int, Int, Int) */ native__Render_getRenderDrawColor(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */ ) {
    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(nar->to_native(rt, renderer).ptr, &r, &g, &b, &a);
    return nar->make_tuple(rt, 4,
            (nar_object_t[]) {nar->make_int(rt, r), nar->make_int(rt, g), nar->make_int(rt, b),
                              nar->make_int(rt, a)});
}

nar_object_t /* Result[String, Renderer] */ native__Render_setRenderDrawBlendMode(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t blendMode /* BlendMode */ ) {
    SDL_Renderer *rdr = nar->to_native(rt, renderer).ptr;
    SDL_BlendMode mode = nar->to_enum_option(rt, blendMode);
    if (0 == SDL_SetRenderDrawBlendMode(rdr, mode)) {
        return nar->make_option(rt, Nar_Base_Result_Result__Ok, 1, &renderer);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        return nar->make_option(rt, Nar_Base_Result_Result__Err, 1, &error);
    }
}

nar_object_t /* BlendMode */ native__Render_getRenderDrawBlendMode(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */ ) {
    SDL_BlendMode mode;
    SDL_GetRenderDrawBlendMode(nar->to_native(rt, renderer).ptr, &mode);
    return nar->make_enum_option(rt, Nar_SDL_Blend_BlendMode, mode, 0, NULL);
}

void render_clear(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    SDL_Renderer *renderer = nar->to_native(rt, payload).ptr;
    if (0 == SDL_RenderClear(renderer)) {
        resolve(rt, payload, task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, Renderer] */ native__Render_renderClear(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, renderer, &render_clear);
}

void render_draw_point(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    SDL_Renderer *renderer = nar->to_native(rt, data.values[0]).ptr;
    int x = (int) nar->to_int(rt, data.values[1]);
    int y = (int) nar->to_int(rt, data.values[2]);
    if (0 == SDL_RenderDrawPoint(renderer, x, y)) {
        resolve(rt, data.values[0], task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, Renderer] */ native__Render_renderDrawPoint(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t x /* Int */,
        nar_object_t y /* Int */ ) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 3, (nar_object_t[]) {renderer, x, y});
    return task_new(rt, data, &render_draw_point);
}

void render_draw_points(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    SDL_Renderer *renderer = nar->to_native(rt, data.values[0]).ptr;
    nar_list_t points = nar->to_list(rt, data.values[1]);

    SDL_Point *pts = nar->alloc(sizeof(SDL_Point) * points.size);
    for (int i = 0; i < points.size; i++) {
        nar_object_t point = points.items[i];
        nar->map_record(rt, point, pts + i, (nar_map_record_cb_fn_t) map_Point);
    }
    if (0 == SDL_RenderDrawPoints(renderer, pts, (int) points.size)) {
        resolve(rt, data.values[0], task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
    nar->free(pts);
}

nar_object_t /* Task[String, Renderer] */ native__Render_renderDrawPoints(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t points /* List[Point] */ ) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 2, (nar_object_t[]) {renderer, points});
    return task_new(rt, data, &render_draw_points);
}

void render_draw_line(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    SDL_Renderer *renderer = nar->to_native(rt, data.values[0]).ptr;
    int x1 = (int) nar->to_int(rt, data.values[1]);
    int y1 = (int) nar->to_int(rt, data.values[2]);
    int x2 = (int) nar->to_int(rt, data.values[3]);
    int y2 = (int) nar->to_int(rt, data.values[4]);
    if (0 == SDL_RenderDrawLine(renderer, x1, y1, x2, y2)) {
        resolve(rt, data.values[0], task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, Renderer] */ native__Render_renderDrawLine(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t x1 /* Int */,
        nar_object_t y1 /* Int */,
        nar_object_t x2 /* Int */,
        nar_object_t y2 /* Int */ ) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 5, (nar_object_t[]) {renderer, x1, y1, x2, y2});
    return task_new(rt, data, &render_draw_line);
}

void render_draw_lines(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    SDL_Renderer *renderer = nar->to_native(rt, data.values[0]).ptr;
    nar_list_t points = nar->to_list(rt, data.values[1]);

    SDL_Point *pts = nar->alloc(sizeof(SDL_Point) * points.size);
    for (int i = 0; i < points.size; i++) {
        nar_object_t point = points.items[i];
        nar->map_record(rt, point, pts + i, (nar_map_record_cb_fn_t) map_Point);
    }
    if (0 == SDL_RenderDrawLines(renderer, pts, (int) points.size)) {
        resolve(rt, data.values[0], task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
    nar->free(pts);
}

nar_object_t /* Task[String, Renderer] */ native__Render_renderDrawLines(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t points /* List[Point] */ ) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 2, (nar_object_t[]) {renderer, points});
    return task_new(rt, data, &render_draw_lines);
}

void render_draw_rect(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    SDL_Renderer *renderer = nar->to_native(rt, data.values[0]).ptr;
    SDL_Rect rect;
    nar->map_record(rt, data.values[1], &rect, (nar_map_record_cb_fn_t) map_Rect);
    if (0 == SDL_RenderDrawRect(renderer, &rect)) {
        resolve(rt, data.values[0], task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, Renderer] */ native__Render_renderDrawRect(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t rect /* Rect */ ) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 2, (nar_object_t[]) {renderer, rect});
    return task_new(rt, data, &render_draw_rect);
}

void render_draw_rects(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    SDL_Renderer *renderer = nar->to_native(rt, data.values[0]).ptr;
    nar_list_t rects = nar->to_list(rt, data.values[1]);

    SDL_Rect *rs = nar->alloc(sizeof(SDL_Rect) * rects.size);
    for (int i = 0; i < rects.size; i++) {
        nar_object_t rect = rects.items[i];
        nar->map_record(rt, rect, rs + i, (nar_map_record_cb_fn_t) map_Rect);
    }
    if (0 == SDL_RenderDrawRects(renderer, rs, (int) rects.size)) {
        resolve(rt, data.values[0], task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
    nar->free(rs);
}

nar_object_t /* Task[String, Renderer] */ native__Render_renderDrawRects(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t rects /* List[Rect] */ ) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 2, (nar_object_t[]) {renderer, rects});
    return task_new(rt, data, &render_draw_rects);
}

void render_fill_rect(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    SDL_Renderer *renderer = nar->to_native(rt, data.values[0]).ptr;
    SDL_Rect rect;
    nar->map_record(rt, data.values[1], &rect, (nar_map_record_cb_fn_t) map_Rect);
    if (0 == SDL_RenderFillRect(renderer, &rect)) {
        resolve(rt, data.values[0], task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, Renderer] */ native__Render_renderFillRect(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t rect /* Rect */ ) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 2, (nar_object_t[]) {renderer, rect});
    return task_new(rt, data, &render_fill_rect);
}

void render_fill_rects(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    SDL_Renderer *renderer = nar->to_native(rt, data.values[0]).ptr;
    nar_list_t rects = nar->to_list(rt, data.values[1]);

    SDL_Rect *rs = nar->alloc(sizeof(SDL_Rect) * rects.size);
    for (int i = 0; i < rects.size; i++) {
        nar_object_t rect = rects.items[i];
        nar->map_record(rt, rect, rs + i, (nar_map_record_cb_fn_t) map_Rect);
    }
    if (0 == SDL_RenderFillRects(renderer, rs, (int) rects.size)) {
        resolve(rt, data.values[0], task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
    nar->free(rs);
}

nar_object_t /* Task[String, Renderer] */ native__Render_renderFillRects(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t rects /* List[Rect] */ ) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 2, (nar_object_t[]) {renderer, rects});
    return task_new(rt, data, &render_fill_rects);
}

void render_copy(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    SDL_Renderer *renderer = nar->to_native(rt, data.values[0]).ptr;
    SDL_Texture *texture = nar->to_native(rt, data.values[1]).ptr;
    SDL_Rect srcRect;
    SDL_Rect *pSrcRect = NULL;
    nar_option_t srcRectOpt = nar->to_option(rt, data.values[2]);
    if (0 == strcmp(srcRectOpt.name, Nar_Base_Maybe_Maybe__Just)) {
        nar->map_record(rt, srcRectOpt.values[0], &srcRect, (nar_map_record_cb_fn_t) map_Rect);
        pSrcRect = &srcRect;
    }
    SDL_Rect dstRect;
    SDL_Rect *pDstRect = NULL;
    nar_option_t dstRectOpt = nar->to_option(rt, data.values[3]);
    if (0 == strcmp(dstRectOpt.name, Nar_Base_Maybe_Maybe__Just)) {
        nar->map_record(rt, dstRectOpt.values[0], &dstRect, (nar_map_record_cb_fn_t) map_Rect);
        pDstRect = &dstRect;
    }
    nar->map_record(rt, data.values[3], &dstRect, (nar_map_record_cb_fn_t) map_Rect);
    if (0 == SDL_RenderCopy(renderer, texture, pSrcRect, pDstRect)) {
        resolve(rt, data.values[0], task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, Renderer] */ native__Render_renderCopy(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t texture /* Texture */,
        nar_object_t srcRect /* Maybe[Rect] */,
        nar_object_t dstRect /* Maybe[Rect] */ ) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 4,
            (nar_object_t[]) {renderer, texture, srcRect, dstRect});
    return task_new(rt, data, &render_copy);
}

void render_copy_ex(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    SDL_Renderer *renderer = nar->to_native(rt, data.values[0]).ptr;
    SDL_Texture *texture = nar->to_native(rt, data.values[1]).ptr;
    SDL_Rect srcRect;
    SDL_Rect *pSrcRect = NULL;
    nar_option_t srcRectOpt = nar->to_option(rt, data.values[2]);
    if (0 == strcmp(srcRectOpt.name, Nar_Base_Maybe_Maybe__Just)) {
        nar->map_record(rt, srcRectOpt.values[0], &srcRect, (nar_map_record_cb_fn_t) map_Rect);
        pSrcRect = &srcRect;
    }

    SDL_Rect dstRect;
    SDL_Rect *pDstRect = NULL;
    nar_option_t dstRectOpt = nar->to_option(rt, data.values[3]);
    if (0 == strcmp(dstRectOpt.name, Nar_Base_Maybe_Maybe__Just)) {
        nar->map_record(rt, dstRectOpt.values[0], &dstRect, (nar_map_record_cb_fn_t) map_Rect);
        pDstRect = &dstRect;
    }

    float angle = (float) nar->to_float(rt, data.values[4]);
    SDL_Point center;
    SDL_Point *pCenter = NULL;
    nar_option_t centerOpt = nar->to_option(rt, data.values[5]);
    if (0 == strcmp(centerOpt.name, Nar_Base_Maybe_Maybe__Just)) {
        nar->map_record(rt, centerOpt.values[0], &center, (nar_map_record_cb_fn_t) map_Point);
        pCenter = &center;
    }

    SDL_RendererFlip flip = nar->to_enum_option(rt, data.values[6]);

    if (0 == SDL_RenderCopyEx(renderer, texture, pSrcRect, pDstRect, angle, pCenter, flip)) {
        resolve(rt, data.values[0], task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, Renderer] */ native__Render_renderCopyEx(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t texture /* Texture */,
        nar_object_t srcRect /* Maybe[Rect] */,
        nar_object_t dstRect /* Maybe[Rect] */,
        nar_object_t angle /* Float */,
        nar_object_t center /* Maybe[Point] */,
        nar_object_t flip /* RendererFlip */ ) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 7,
            (nar_object_t[]) {renderer, texture, srcRect, dstRect, angle, center, flip});
    return task_new(rt, data, &render_copy_ex);
}

void render_draw_point_f(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    SDL_Renderer *renderer = nar->to_native(rt, data.values[0]).ptr;
    float x = (float) nar->to_float(rt, data.values[1]);
    float y = (float) nar->to_float(rt, data.values[2]);
    if (0 == SDL_RenderDrawPointF(renderer, x, y)) {
        resolve(rt, data.values[0], task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, Renderer] */ native__Render_renderDrawPointF(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t x /* Float */,
        nar_object_t y /* Float */ ) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 3, (nar_object_t[]) {renderer, x, y});
    return task_new(rt, data, &render_draw_point_f);
}

void render_draw_points_f(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    SDL_Renderer *renderer = nar->to_native(rt, data.values[0]).ptr;
    nar_list_t points = nar->to_list(rt, data.values[1]);

    SDL_FPoint *pts = nar->alloc(sizeof(SDL_FPoint) * points.size);
    for (int i = 0; i < points.size; i++) {
        nar_object_t point = points.items[i];
        nar->map_record(rt, point, pts + i, (nar_map_record_cb_fn_t) map_FPoint);
    }
    if (0 == SDL_RenderDrawPointsF(renderer, pts, (int) points.size)) {
        resolve(rt, data.values[0], task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
    nar->free(pts);
}

nar_object_t /* Task[String, Renderer] */ native__Render_renderDrawPointsF(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t points /* List[FPoint] */ ) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 2, (nar_object_t[]) {renderer, points});
    return task_new(rt, data, &render_draw_points_f);
}

void render_draw_line_f(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    SDL_Renderer *renderer = nar->to_native(rt, data.values[0]).ptr;
    float x1 = (float) nar->to_float(rt, data.values[1]);
    float y1 = (float) nar->to_float(rt, data.values[2]);
    float x2 = (float) nar->to_float(rt, data.values[3]);
    float y2 = (float) nar->to_float(rt, data.values[4]);
    if (0 == SDL_RenderDrawLineF(renderer, x1, y1, x2, y2)) {
        resolve(rt, data.values[0], task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, Renderer] */ native__Render_renderDrawLineF(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t x1 /* Float */,
        nar_object_t y1 /* Float */,
        nar_object_t x2 /* Float */,
        nar_object_t y2 /* Float */ ) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 5, (nar_object_t[]) {renderer, x1, y1, x2, y2});
    return task_new(rt, data, &render_draw_line_f);
}

void render_draw_lines_f(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    SDL_Renderer *renderer = nar->to_native(rt, data.values[0]).ptr;
    nar_list_t points = nar->to_list(rt, data.values[1]);

    SDL_FPoint *pts = nar->alloc(sizeof(SDL_FPoint) * points.size);
    for (int i = 0; i < points.size; i++) {
        nar_object_t point = points.items[i];
        nar->map_record(rt, point, pts + i, (nar_map_record_cb_fn_t) map_FPoint);
    }
    if (0 == SDL_RenderDrawLinesF(renderer, pts, (int) points.size)) {
        resolve(rt, data.values[0], task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
    nar->free(pts);
}

nar_object_t /* Task[String, Renderer] */ native__Render_renderDrawLinesF(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t points /* List[FPoint] */ ) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 2, (nar_object_t[]) {renderer, points});
    return task_new(rt, data, &render_draw_lines_f);
}

void render_draw_rect_f(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    SDL_Renderer *renderer = nar->to_native(rt, data.values[0]).ptr;
    SDL_FRect rect;
    nar->map_record(rt, data.values[1], &rect, (nar_map_record_cb_fn_t) map_FRect);
    if (0 == SDL_RenderDrawRectF(renderer, &rect)) {
        resolve(rt, data.values[0], task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, Renderer] */ native__Render_renderDrawRectF(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t rect /* FRect */ ) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 2, (nar_object_t[]) {renderer, rect});
    return task_new(rt, data, &render_draw_rect_f);
}

void render_draw_rects_f(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    SDL_Renderer *renderer = nar->to_native(rt, data.values[0]).ptr;
    nar_list_t rects = nar->to_list(rt, data.values[1]);

    SDL_FRect *rs = nar->alloc(sizeof(SDL_FRect) * rects.size);
    for (int i = 0; i < rects.size; i++) {
        nar_object_t rect = rects.items[i];
        nar->map_record(rt, rect, rs + i, (nar_map_record_cb_fn_t) map_FRect);
    }
    if (0 == SDL_RenderDrawRectsF(renderer, rs, (int) rects.size)) {
        resolve(rt, data.values[0], task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
    nar->free(rs);
}

nar_object_t /* Task[String, Renderer] */ native__Render_renderDrawRectsF(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t rects /* List[FRect] */ ) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 2, (nar_object_t[]) {renderer, rects});
    return task_new(rt, data, &render_draw_rects_f);
}

void render_fill_rect_f(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    SDL_Renderer *renderer = nar->to_native(rt, data.values[0]).ptr;
    SDL_FRect rect;
    nar->map_record(rt, data.values[1], &rect, (nar_map_record_cb_fn_t) map_FRect);
    if (0 == SDL_RenderFillRectF(renderer, &rect)) {
        resolve(rt, data.values[0], task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, Renderer] */ native__Render_renderFillRectF(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t rect /* FRect */ ) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 2, (nar_object_t[]) {renderer, rect});
    return task_new(rt, data, &render_fill_rect_f);
}

void render_fill_rects_f(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    SDL_Renderer *renderer = nar->to_native(rt, data.values[0]).ptr;
    nar_list_t rects = nar->to_list(rt, data.values[1]);

    SDL_FRect *rs = nar->alloc(sizeof(SDL_FRect) * rects.size);
    for (int i = 0; i < rects.size; i++) {
        nar_object_t rect = rects.items[i];
        nar->map_record(rt, rect, rs + i, (nar_map_record_cb_fn_t) map_FRect);
    }
    if (0 == SDL_RenderFillRectsF(renderer, rs, (int) rects.size)) {
        resolve(rt, data.values[0], task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
    nar->free(rs);
}

nar_object_t /* Task[String, Renderer] */ native__Render_renderFillRectsF(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t rects /* List[FRect] */ ) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 2, (nar_object_t[]) {renderer, rects});
    return task_new(rt, data, &render_fill_rects_f);
}

void render_copy_f(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    SDL_Renderer *renderer = nar->to_native(rt, data.values[0]).ptr;
    SDL_Texture *texture = nar->to_native(rt, data.values[1]).ptr;
    SDL_Rect srcRect;
    SDL_Rect *pSrcRect = NULL;
    nar_option_t srcRectOpt = nar->to_option(rt, data.values[2]);
    if (0 == strcmp(srcRectOpt.name, Nar_Base_Maybe_Maybe__Just)) {
        nar->map_record(rt, srcRectOpt.values[0], &srcRect, (nar_map_record_cb_fn_t) map_Rect);
        pSrcRect = &srcRect;
    }
    SDL_FRect dstRect;
    SDL_FRect *pDstRect = NULL;
    nar_option_t dstRectOpt = nar->to_option(rt, data.values[3]);
    if (0 == strcmp(dstRectOpt.name, Nar_Base_Maybe_Maybe__Just)) {
        nar->map_record(rt, dstRectOpt.values[0], &dstRect, (nar_map_record_cb_fn_t) map_FRect);
        pDstRect = &dstRect;
    }
    if (0 == SDL_RenderCopyF(renderer, texture, pSrcRect, pDstRect)) {
        resolve(rt, data.values[0], task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, Renderer] */ native__Render_renderCopyF(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t texture /* Texture */,
        nar_object_t srcRect /* Maybe[Rect] */,
        nar_object_t dstRect /* Maybe [FRect] */ ) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 4,
            (nar_object_t[]) {renderer, texture, srcRect, dstRect});
    return task_new(rt, data, &render_copy_f);
}

void render_copy_ex_f(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    SDL_Renderer *renderer = nar->to_native(rt, data.values[0]).ptr;
    SDL_Texture *texture = nar->to_native(rt, data.values[1]).ptr;
    SDL_Rect srcRect;
    SDL_Rect *pSrcRect = NULL;
    nar_option_t srcRectOpt = nar->to_option(rt, data.values[2]);
    if (0 == strcmp(srcRectOpt.name, Nar_Base_Maybe_Maybe__Just)) {
        nar->map_record(rt, srcRectOpt.values[0], &srcRect, (nar_map_record_cb_fn_t) map_Rect);
        pSrcRect = &srcRect;
    }

    SDL_FRect dstRect;
    SDL_FRect *pDstRect = NULL;
    nar_option_t dstRectOpt = nar->to_option(rt, data.values[3]);
    if (0 == strcmp(dstRectOpt.name, Nar_Base_Maybe_Maybe__Just)) {
        nar->map_record(rt, dstRectOpt.values[0], &dstRect, (nar_map_record_cb_fn_t) map_FRect);
        pDstRect = &dstRect;
    }

    float angle = (float) nar->to_float(rt, data.values[4]);
    SDL_FPoint center;
    SDL_FPoint *pCenter = NULL;
    nar_option_t centerOpt = nar->to_option(rt, data.values[5]);
    if (0 == strcmp(centerOpt.name, Nar_Base_Maybe_Maybe__Just)) {
        nar->map_record(rt, centerOpt.values[0], &center, (nar_map_record_cb_fn_t) map_FPoint);
        pCenter = &center;
    }

    SDL_RendererFlip flip = nar->to_enum_option(rt, data.values[6]);

    if (0 == SDL_RenderCopyExF(renderer, texture, pSrcRect, pDstRect, angle, pCenter, flip)) {
        resolve(rt, data.values[0], task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, Renderer] */ native__Render_renderCopyExF(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t texture /* Texture */,
        nar_object_t srcRect /* Maybe[Rect] */,
        nar_object_t dstRect /* Maybe [FRect] */,
        nar_object_t angle /* Float */,
        nar_object_t center /* Maybe[Point] */,
        nar_object_t flip /* RendererFlip */ ) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 7,
            (nar_object_t[]) {renderer, texture, srcRect, dstRect, angle, center, flip});
    return task_new(rt, data, &render_copy_ex_f);
}

void map_Vertex(nar_runtime_t rt, nar_cstring_t key, nar_object_t value, SDL_Vertex *vertex) {
    if (0 == strcmp(key, "position")) {
        nar->map_record(rt, value, &vertex->position, (nar_map_record_cb_fn_t) map_FPoint);
    } else if (0 == strcmp(key, "color")) {
        nar->map_record(rt, value, &vertex->color, (nar_map_record_cb_fn_t) map_Color);
    } else if (0 == strcmp(key, "uv")) {
        nar->map_record(rt, value, &vertex->tex_coord, (nar_map_record_cb_fn_t) map_FPoint);
    }
}

void render_geometry(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    SDL_Renderer *renderer = nar->to_native(rt, data.values[0]).ptr;
    SDL_Texture *texture = nar->to_native(rt, data.values[1]).ptr;
    nar_list_t vertices = nar->to_list(rt, data.values[2]);
    nar_list_t indices = nar->to_list(rt, data.values[3]);

    SDL_Vertex *vts = nar->alloc(sizeof(SDL_Vertex) * vertices.size);
    for (int i = 0; i < vertices.size; i++) {
        nar_object_t vertex = vertices.items[i];
        nar->map_record(rt, vertex, vts + i, (nar_map_record_cb_fn_t) map_Vertex);
    }

    int *inds = nar->alloc(sizeof(int) * indices.size);
    for (int i = 0; i < indices.size; i++) {
        inds[i] = (int) nar->to_int(rt, indices.items[i]);
    }

    int status = SDL_RenderGeometry(
            renderer, texture, vts, (int) vertices.size, inds, (int) indices.size);

    if (0 == status) {
        resolve(rt, data.values[0], task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
    nar->free(vts);
    nar->free(inds);
}

nar_object_t /* Task[String, Renderer] */ native__Render_renderGeometry(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t texture /* Texture */,
        nar_object_t vertices /* List[Vertex] */,
        nar_object_t indices /* List [Int] */ ) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 4,
            (nar_object_t[]) {renderer, texture, vertices, indices});
    return task_new(rt, data, &render_geometry);
}

void render_geometry_raw(nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    SDL_Renderer *renderer = nar->to_native(rt, data.values[0]).ptr;
    SDL_Texture *texture = nar->to_native(rt, data.values[1]).ptr;
    const float *xy = nar->to_native(rt, data.values[2]).ptr;
    int xyStride = (int) nar->to_int(rt, data.values[3]);
    const SDL_Color *color = nar->to_native(rt, data.values[4]).ptr;
    int colorStride = (int) nar->to_int(rt, data.values[5]);
    float *uv = nar->to_native(rt, data.values[6]).ptr;
    int uvStride = (int) nar->to_int(rt, data.values[7]);
    int numVertices = (int) nar->to_int(rt, data.values[8]);
    const int *indices = nar->to_native(rt, data.values[9]).ptr;
    int numIndices = (int) nar->to_int(rt, data.values[10]);
    int sizeIndices = (int) nar->to_int(rt, data.values[11]);
    int status = SDL_RenderGeometryRaw(
            renderer, texture, xy, xyStride, color, colorStride, uv, uvStride, numVertices,
            indices, numIndices, sizeIndices);
    if (0 == status) {
        resolve(rt, data.values[0], task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, Renderer] */ native__Render_renderGeometryRaw(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t texture /* Texture */,
        nar_object_t xy /* Ptr */,
        nar_object_t xyStride /* Int */,
        nar_object_t color /* Ptr */,
        nar_object_t colorStride /* Int */,
        nar_object_t uv /* Ptr */,
        nar_object_t uvStride /* Int */,
        nar_object_t numVertices /* Int */,
        nar_object_t indices /* Ptr */,
        nar_object_t numIndices /* Int */,
        nar_object_t sizeIndices /* Int */ ) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 12,
            (nar_object_t[]) {renderer, texture, xy, xyStride, color, colorStride, uv, uvStride,
                              numVertices, indices, numIndices, sizeIndices});
    return task_new(rt, data, &render_geometry_raw);
}

void render_read_pixels(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    SDL_Renderer *renderer = nar->to_native(rt, data.values[0]).ptr;
    SDL_Rect rect;
    nar->map_record(rt, data.values[1], &rect, (nar_map_record_cb_fn_t) map_Rect);
    Uint32 format = nar->to_enum_option(rt, data.values[2]);
    void *pixels = nar->to_native(rt, data.values[3]).ptr;
    int pitch = (int) nar->to_int(rt, data.values[4]);
    int status = SDL_RenderReadPixels(renderer, &rect, format, &pixels, pitch);
    if (0 == status) {
        resolve(rt, data.values[0], task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, Ptr] */ native__Render_renderReadPixels(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t rect /* Rect */,
        nar_object_t format /* PixelFormat */,
        nar_object_t pixels /* Ptr */,
        nar_object_t pitch /* Int */ ) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 5,
            (nar_object_t[]) {renderer, rect, format, pixels, pitch});
    return task_new(rt, data, &render_read_pixels);
}

void render_present(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    SDL_Renderer *renderer = nar->to_native(rt, payload).ptr;
    SDL_RenderPresent(renderer);
    resolve(rt, payload, task_state);
}

nar_object_t /* Task[String, Renderer] */ native__Render_renderPresent(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, renderer, &render_present);
}

void destroy_texture(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    SDL_Texture *texture = nar->to_native(rt, payload).ptr;
    SDL_DestroyTexture(texture);
    resolve(rt, nar->make_unit(rt), task_state);
}

nar_object_t /* Task[String, ()] */ native__Render_destroyTexture(nar_runtime_t rt,
        nar_object_t texture /* Texture */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, texture, &destroy_texture);
}

void destroy_renderer(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    SDL_Renderer *renderer = nar->to_native(rt, payload).ptr;
    SDL_DestroyRenderer(renderer);
    resolve(rt, nar->make_unit(rt), task_state);
}

nar_object_t /* Task[String, ()] */ native__Render_destroyRenderer(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, renderer, &destroy_renderer);
}

void render_flush(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    SDL_Renderer *renderer = nar->to_native(rt, payload).ptr;
    if (0 == SDL_RenderFlush(renderer)) {
        resolve(rt, payload, task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, Renderer] */ native__Render_renderFlush(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, renderer, &render_flush);
}

void gl_bind_texture(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    SDL_Texture *tex = nar->to_native(rt, payload).ptr;
    float x, y;
    if (0 == SDL_GL_BindTexture(tex, &x, &y)) {
        nar_object_t result = nar->make_tuple(
                rt, 2, (nar_object_t[]) {nar->make_float(rt, x), nar->make_float(rt, y)});
        resolve(rt, result, task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, (Float, Float)] */ native__Render_glBindTexture(nar_runtime_t rt,
        nar_object_t texture /* Texture */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, texture, &gl_bind_texture);
}

void gl_unbind_texture(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    SDL_Texture *tex = nar->to_native(rt, payload).ptr;
    if (0 == SDL_GL_UnbindTexture(tex)) {
        resolve(rt, payload, task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, ()] */ native__Render_glUnbindTexture(nar_runtime_t rt,
        nar_object_t texture /* Texture */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    return task_new(rt, texture, &gl_unbind_texture);
}

nar_object_t /* Maybe[Ptr] */ native__Render_renderGetMetalLayer(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */) {
    void *layer = SDL_RenderGetMetalLayer(nar->to_native(rt, renderer).ptr);
    if (layer) {
        nar_object_t result = nar->make_native(rt, layer, NULL);
        return nar->make_option(rt, Nar_Base_Maybe_Maybe__Just, 1, &result);
    } else {
        return nar->make_option(rt, Nar_Base_Maybe_Maybe__Nothing, 0, NULL);
    }
}

nar_object_t /* Maybe[Ptr] */ native__Render_renderGetMetalCommandEncoder(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */) {
    void *encoder = SDL_RenderGetMetalCommandEncoder(nar->to_native(rt, renderer).ptr);
    if (encoder) {
        nar_object_t result = nar->make_native(rt, encoder, NULL);
        return nar->make_option(rt, Nar_Base_Maybe_Maybe__Just, 1, &result);
    } else {
        return nar->make_option(rt, Nar_Base_Maybe_Maybe__Nothing, 0, NULL);
    }
}

void render_set_vsync(
        nar_runtime_t rt,
        nar_object_t payload,
        nar_program_task_resolve_fn_t resolve,
        nar_program_task_reject_fn_t reject,
        nar_task_state_t *task_state) {
    nar_tuple_t data = nar->to_tuple(rt, payload);
    SDL_Renderer *renderer = nar->to_native(rt, data.values[0]).ptr;
    int enable = (int) nar->to_int(rt, data.values[1]);
    if (0 == SDL_RenderSetVSync(renderer, enable)) {
        resolve(rt, data.values[0], task_state);
    } else {
        nar_object_t error = nar->make_string(rt, SDL_GetError());
        reject(rt, error, task_state);
    }
}

nar_object_t /* Task[String, Renderer] */ native__Render_renderSetVSync(nar_runtime_t rt,
        nar_object_t renderer /* Renderer */,
        nar_object_t enable /* Int */) {
    nar_program_task_new_fn_t task_new = nar->get_metadata(rt, NAR_META__Nar_Program_task_new);
    nar_object_t data = nar->make_tuple(rt, 2, (nar_object_t[]) {renderer, enable});
    return task_new(rt, data, &render_set_vsync);
}

void init_render(nar_runtime_t rt) {
    nar->register_def(rt, "Nar.SDL.Render", "numRenderDrivers",
            &native__Render_numRenderDrivers, 0);
    nar->register_def(rt, "Nar.SDL.Render", "getRenderDriverInfo",
            &native__Render_getRenderDriverInfo, 1);
    nar->register_def(rt, "Nar.SDL.Render", "createWindowAndRenderer",
            &native__Render_createWindowAndRenderer, 3);
    nar->register_def(rt, "Nar.SDL.Render", "createRenderer",
            &native__Render_createRenderer, 3);
    nar->register_def(rt, "Nar.SDL.Render", "createSoftwareRenderer",
            &native__Render_createSoftwareRenderer, 1);
    nar->register_def(rt, "Nar.SDL.Render", "getRenderer",
            &native__Render_getRenderer, 1);
    nar->register_def(rt, "Nar.SDL.Render", "renderGetWindow",
            &native__Render_renderGetWindow, 1);
    nar->register_def(rt, "Nar.SDL.Render", "getRendererInfo",
            &native__Render_getRendererInfo, 1);
    nar->register_def(rt, "Nar.SDL.Render", "getRendererOutputSize",
            &native__Render_getRendererOutputSize, 1);
    nar->register_def(rt, "Nar.SDL.Render", "createTexture",
            &native__Render_createTexture, 5);
    nar->register_def(rt, "Nar.SDL.Render", "createTextureFromSurface",
            &native__Render_createTextureFromSurface, 2);
    nar->register_def(rt, "Nar.SDL.Render", "queryTexture",
            &native__Render_queryTexture, 1);
    nar->register_def(rt, "Nar.SDL.Render", "setTextureColorMod",
            &native__Render_setTextureColorMod, 4);
    nar->register_def(rt, "Nar.SDL.Render", "getTextureColorMod",
            &native__Render_getTextureColorMod, 1);
    nar->register_def(rt, "Nar.SDL.Render", "setTextureAlphaMod",
            &native__Render_setTextureAlphaMod, 2);
    nar->register_def(rt, "Nar.SDL.Render", "getTextureAlphaMod",
            &native__Render_getTextureAlphaMod, 1);
    nar->register_def(rt, "Nar.SDL.Render", "setTextureBlendMode",
            &native__Render_setTextureBlendMode, 2);
    nar->register_def(rt, "Nar.SDL.Render", "getTextureBlendMode",
            &native__Render_getTextureBlendMode, 1);
    nar->register_def(rt, "Nar.SDL.Render", "setTextureScaleMode",
            &native__Render_setTextureScaleMode, 2);
    nar->register_def(rt, "Nar.SDL.Render", "getTextureScaleMode",
            &native__Render_getTextureScaleMode, 1);
    nar->register_def(rt, "Nar.SDL.Render", "setTextureUserData",
            &native__Render_setTextureUserData, 2);
    nar->register_def(rt, "Nar.SDL.Render", "getTextureUserData",
            &native__Render_getTextureUserData, 1);
    nar->register_def(rt, "Nar.SDL.Render", "updateTexture",
            &native__Render_updateTexture, 4);
    nar->register_def(rt, "Nar.SDL.Render", "updateYUVTexture",
            &native__Render_updateYUVTexture, 8);
    nar->register_def(rt, "Nar.SDL.Render", "lockTexture",
            &native__Render_lockTexture, 2);
    nar->register_def(rt, "Nar.SDL.Render", "unlockTexture",
            &native__Render_unlockTexture, 1);
    nar->register_def(rt, "Nar.SDL.Render", "renderTargetSupported",
            &native__Render_renderTargetSupported, 1);
    nar->register_def(rt, "Nar.SDL.Render", "setRenderTarget",
            &native__Render_setRenderTarget, 2);
    nar->register_def(rt, "Nar.SDL.Render", "getRenderTarget",
            &native__Render_getRenderTarget, 1);
    nar->register_def(rt, "Nar.SDL.Render", "renderSetLogicalSize",
            &native__Render_renderSetLogicalSize, 3);
    nar->register_def(rt, "Nar.SDL.Render", "renderGetLogicalSize",
            &native__Render_renderGetLogicalSize, 1);
    nar->register_def(rt, "Nar.SDL.Render", "renderSetIntegerScale",
            &native__Render_renderSetIntegerScale, 2);
    nar->register_def(rt, "Nar.SDL.Render", "renderGetIntegerScale",
            &native__Render_renderGetIntegerScale, 1);
    nar->register_def(rt, "Nar.SDL.Render", "renderSetViewport",
            &native__Render_renderSetViewport, 2);
    nar->register_def(rt, "Nar.SDL.Render", "renderGetViewport",
            &native__Render_renderGetViewport, 1);
    nar->register_def(rt, "Nar.SDL.Render", "renderSetClipRect",
            &native__Render_renderSetClipRect, 2);
    nar->register_def(rt, "Nar.SDL.Render", "renderGetClipRect",
            &native__Render_renderGetClipRect, 1);
    nar->register_def(rt, "Nar.SDL.Render", "renderIsClipEnabled",
            &native__Render_renderIsClipEnabled, 1);
    nar->register_def(rt, "Nar.SDL.Render", "renderSetScale",
            &native__Render_renderSetScale, 3);
    nar->register_def(rt, "Nar.SDL.Render", "renderGetScale",
            &native__Render_renderGetScale, 1);
    nar->register_def(rt, "Nar.SDL.Render", "renderWindowToLogical",
            &native__Render_renderWindowToLogical, 3);
    nar->register_def(rt, "Nar.SDL.Render", "renderLogicalToWindow",
            &native__Render_renderLogicalToWindow, 3);
    nar->register_def(rt, "Nar.SDL.Render", "setRenderDrawColor",
            &native__Render_setRenderDrawColor, 5);
    nar->register_def(rt, "Nar.SDL.Render", "getRenderDrawColor",
            &native__Render_getRenderDrawColor, 1);
    nar->register_def(rt, "Nar.SDL.Render", "setRenderDrawBlendMode",
            &native__Render_setRenderDrawBlendMode, 2);
    nar->register_def(rt, "Nar.SDL.Render", "getRenderDrawBlendMode",
            &native__Render_getRenderDrawBlendMode, 1);
    nar->register_def(rt, "Nar.SDL.Render", "renderClear",
            &native__Render_renderClear, 1);
    nar->register_def(rt, "Nar.SDL.Render", "renderDrawPoint",
            &native__Render_renderDrawPoint, 3);
    nar->register_def(rt, "Nar.SDL.Render", "renderDrawPoints",
            &native__Render_renderDrawPoints, 2);
    nar->register_def(rt, "Nar.SDL.Render", "renderDrawLine",
            &native__Render_renderDrawLine, 5);
    nar->register_def(rt, "Nar.SDL.Render", "renderDrawLines",
            &native__Render_renderDrawLines, 2);
    nar->register_def(rt, "Nar.SDL.Render", "renderDrawRect",
            &native__Render_renderDrawRect, 2);
    nar->register_def(rt, "Nar.SDL.Render", "renderDrawRects",
            &native__Render_renderDrawRects, 2);
    nar->register_def(rt, "Nar.SDL.Render", "renderFillRect",
            &native__Render_renderFillRect, 2);
    nar->register_def(rt, "Nar.SDL.Render", "renderFillRects",
            &native__Render_renderFillRects, 2);
    nar->register_def(rt, "Nar.SDL.Render", "renderCopy",
            &native__Render_renderCopy, 4);
    nar->register_def(rt, "Nar.SDL.Render", "renderCopyEx",
            &native__Render_renderCopyEx, 7);
    nar->register_def(rt, "Nar.SDL.Render", "renderDrawPointF",
            &native__Render_renderDrawPointF, 3);
    nar->register_def(rt, "Nar.SDL.Render", "renderDrawPointsF",
            &native__Render_renderDrawPointsF, 2);
    nar->register_def(rt, "Nar.SDL.Render", "renderDrawLineF",
            &native__Render_renderDrawLineF, 5);
    nar->register_def(rt, "Nar.SDL.Render", "renderDrawLinesF",
            &native__Render_renderDrawLinesF, 2);
    nar->register_def(rt, "Nar.SDL.Render", "renderDrawRectF",
            &native__Render_renderDrawRectF, 2);
    nar->register_def(rt, "Nar.SDL.Render", "renderDrawRectsF",
            &native__Render_renderDrawRectsF, 2);
    nar->register_def(rt, "Nar.SDL.Render", "renderFillRectF",
            &native__Render_renderFillRectF, 2);
    nar->register_def(rt, "Nar.SDL.Render", "renderFillRectsF",
            &native__Render_renderFillRectsF, 2);
    nar->register_def(rt, "Nar.SDL.Render", "renderCopyF",
            &native__Render_renderCopyF, 4);
    nar->register_def(rt, "Nar.SDL.Render", "renderCopyExF",
            &native__Render_renderCopyExF, 7);
    nar->register_def(rt, "Nar.SDL.Render", "renderGeometry",
            &native__Render_renderGeometry, 4);
    nar->register_def(rt, "Nar.SDL.Render", "renderGeometryRaw",
            &native__Render_renderGeometryRaw, 12);
    nar->register_def(rt, "Nar.SDL.Render", "renderReadPixels",
            &native__Render_renderReadPixels, 5);
    nar->register_def(rt, "Nar.SDL.Render", "renderPresent",
            &native__Render_renderPresent, 1);
    nar->register_def(rt, "Nar.SDL.Render", "destroyTexture",
            &native__Render_destroyTexture, 1);
    nar->register_def(rt, "Nar.SDL.Render", "destroyRenderer",
            &native__Render_destroyRenderer, 1);
    nar->register_def(rt, "Nar.SDL.Render", "renderFlush",
            &native__Render_renderFlush, 1);
    nar->register_def(rt, "Nar.SDL.Render", "glBindTexture",
            &native__Render_glBindTexture, 1);
    nar->register_def(rt, "Nar.SDL.Render", "glUnbindTexture",
            &native__Render_glUnbindTexture, 1);
    nar->register_def(rt, "Nar.SDL.Render", "renderGetMetalLayer",
            &native__Render_renderGetMetalLayer, 1);
    nar->register_def(rt, "Nar.SDL.Render", "renderGetMetalCommandEncoder",
            &native__Render_renderGetMetalCommandEncoder, 1);
    nar->register_def(rt, "Nar.SDL.Render", "renderSetVSync",
            &native__Render_renderSetVSync, 2);

    nar->enum_def(Nar_SDL_Render_RendererFlag, Nar_SDL_Render_RendererFlag__RendererFlagSoftware,
            SDL_RENDERER_SOFTWARE);
    nar->enum_def(Nar_SDL_Render_RendererFlag, Nar_SDL_Render_RendererFlag__RendererFlagAccelerated,
            SDL_RENDERER_ACCELERATED);
    nar->enum_def(Nar_SDL_Render_RendererFlag, Nar_SDL_Render_RendererFlag__RendererFlagPresentVSync,
            SDL_RENDERER_PRESENTVSYNC);
    nar->enum_def(Nar_SDL_Render_RendererFlag, Nar_SDL_Render_RendererFlag__RendererFlagTargetTexture,
            SDL_RENDERER_TARGETTEXTURE);

    nar->enum_def(Nar_SDL_Render_ScaleMode, Nar_SDL_Render_ScaleMode__ScaleModeNearest, SDL_ScaleModeNearest);
    nar->enum_def(Nar_SDL_Render_ScaleMode, Nar_SDL_Render_ScaleMode__ScaleModeLinear, SDL_ScaleModeLinear);
    nar->enum_def(Nar_SDL_Render_ScaleMode, Nar_SDL_Render_ScaleMode__ScaleModeBest, SDL_ScaleModeBest);

    nar->enum_def(Nar_SDL_Render_TextureAccess, Nar_SDL_Render_TextureAccess__TextureAccessStatic,
            SDL_TEXTUREACCESS_STATIC);
    nar->enum_def(Nar_SDL_Render_TextureAccess, Nar_SDL_Render_TextureAccess__TextureAccessStreaming,
            SDL_TEXTUREACCESS_STREAMING);
    nar->enum_def(Nar_SDL_Render_TextureAccess, Nar_SDL_Render_TextureAccess__TextureAccessTarget,
            SDL_TEXTUREACCESS_TARGET);

    nar->enum_def(Nar_SDL_Render_TextureModulate, Nar_SDL_Render_TextureModulate__TextureModulateNone,
            SDL_TEXTUREMODULATE_NONE);
    nar->enum_def(Nar_SDL_Render_TextureModulate, Nar_SDL_Render_TextureModulate__TextureModulateColor,
            SDL_TEXTUREMODULATE_COLOR);
    nar->enum_def(Nar_SDL_Render_TextureModulate, Nar_SDL_Render_TextureModulate__TextureModulateAlpha,
            SDL_TEXTUREMODULATE_ALPHA);

    nar->enum_def(Nar_SDL_Render_RendererFlip, Nar_SDL_Render_RendererFlip__RendererFlipNone, 0);
    nar->enum_def(Nar_SDL_Render_RendererFlip, Nar_SDL_Render_RendererFlip__RendererFlipHorizontal,
            SDL_FLIP_HORIZONTAL);
    nar->enum_def(Nar_SDL_Render_RendererFlip, Nar_SDL_Render_RendererFlip__RendererFlipVertical,
            SDL_FLIP_VERTICAL);
}